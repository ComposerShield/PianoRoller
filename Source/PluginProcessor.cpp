/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PianoRoll1AudioProcessor::PianoRoll1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
        treeState(*this, nullptr, "PARAMETERS", {
            std::make_unique<AudioParameterInt> (PRESET_ID, PRESET_NAME, 1, numOfPresets, 1),
            std::make_unique<AudioParameterInt> (TRACK_ID, TRACK_NAME, 1, numOfTracks, 1),
            std::make_unique<AudioParameterInt> (BEATS_ID, BEATS_NAME, 1, maxBeats, 4),
            std::make_unique<AudioParameterInt> (ROOT_ID, ROOT_NAME, 0, 16, 0),
            std::make_unique<AudioParameterInt> (SCALE_ID, SCALE_NAME, 0, Theory::modeMap.size()-1, Theory::getMajorFromModeMap),
            std::make_unique<AudioParameterInt> (MONOPOLY_ID, MONOPOLY_NAME, 0, 1, 0)
        })
#endif
{
    
    NormalisableRange<float> presetRange( 1.0f, static_cast<float>(PianoRollComponent::numOfPresets) );
    NormalisableRange<float> trackRange ( 1.0f, static_cast<float>(PianoRollComponent::numOfTracks)  );
    NormalisableRange<float> beatsRange ( 1.0f, static_cast<float>(PianoRollComponent::maxBeats)     );
    
    //Value Tree State===========================================================
    treeState.addParameterListener(PRESET_ID, this); //Adds listener to preset slider tree value.
    treeState.addParameterListener(TRACK_ID, this); //Adds listener to track slider tree value.
    treeState.addParameterListener(BEATS_ID, this); //Adds listener to track slider tree value.
    treeState.addParameterListener(ROOT_ID, this);
    treeState.addParameterListener(SCALE_ID, this);
    treeState.addParameterListener(MONOPOLY_ID, this);
    
    //OSC MESSAGES===============================================================
    if (! connect(6449)){}; //Connect to OSC messages from Max.
    juce::OSCReceiver::addListener(this, "/juce");
    
    midiStream.ensureStorageAllocated(36);
    midiInstrumentStream.ensureStorageAllocated(88);
    notesToIgnore.ensureStorageAllocated(36);
    activeNotes.ensureStorageAllocated(24);
    
}

PianoRoll1AudioProcessor::~PianoRoll1AudioProcessor()
{
    //presets.clear();
    juce::OSCReceiver::removeListener(this);
    
    
}

//==============================================================================
const String PianoRoll1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PianoRoll1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PianoRoll1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PianoRoll1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif 
}

double PianoRoll1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PianoRoll1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PianoRoll1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void PianoRoll1AudioProcessor::setCurrentProgram (int index)
{
}

const String PianoRoll1AudioProcessor::getProgramName (int index)
{
    return {};
}

void PianoRoll1AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void PianoRoll1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    playPosition.setValue(0.0f);
    updateCounter = 0;
    midiStream.clearQuick();
}

void PianoRoll1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PianoRoll1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PianoRoll1AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    //if(isPlugin)
    //    jassert (buffer.getNumChannels() == 0); //Set number of audio channels to 0
    
    const float numOfSamps = buffer.getNumSamples();
   
    buffer.clear();
    
    if (auto* ph = getPlayHead())
    {
        AudioPlayHead::CurrentPositionInfo newTime;
        if (ph->getCurrentPosition (newTime))
        {
            lastPosInfo = newTime;  // Successfully got the current time from the host..
        }
    }
    
    //Add notes played on an external midi instrument into the sequencer when playing.
    if(lastPosInfo.isPlaying){
        internalPlayPosition.setValue(lastPosInfo.ppqPosition);
        MidiBuffer::Iterator iterator(midiMessages);
        juce::MidiMessage incomingMessage;
        int incomingSamplePosition;
        while(iterator.getNextEvent(incomingMessage, incomingSamplePosition)){ //While there IS another midi event, get it.
            if(incomingMessage.isNoteOn()){
                const int8 pitch = incomingMessage.getNoteNumber();
                const int8 vol = incomingMessage.getVelocity();
                midiInstrumentStream.add(std::make_pair(pitch, vol)); //Add to midi instrument stream.
            }
        }
        
        sequencerCheck(internalPlayPosition); //Sends current position to the sequencer to see if notes need to be played.
        checkIfTooManyNotes(midiMessages);
        midiStreamToMidiOutput(midiMessages);
    }
    else //Not playing
        notesToIgnore.clearQuick();
}

//==============================================================================
bool PianoRoll1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PianoRoll1AudioProcessor::createEditor()
{
    return new PianoRoll1AudioProcessorEditor (*this);
}

//==============================================================================
void PianoRoll1AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    
    ScopedPointer<XmlElement> xml(treeState.state.createXml());
    copyXmlToBinary(*xml, destData);
    
}

void PianoRoll1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    
    ScopedPointer<XmlElement> theParams(getXmlFromBinary(data, sizeInBytes));
    if(theParams != nullptr){
        if(theParams -> hasTagName(treeState.state.getType())){
            //treeState.state = ValueTree::fromXml(*theParams);
            treeState.replaceState(ValueTree::fromXml(*theParams));
        }
    }
    
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PianoRoll1AudioProcessor();
}

constexpr void PianoRoll1AudioProcessor::midiStreamToMidiOutput(juce::MidiBuffer &midiMessages){
    const int midiStart = midiMessages.getFirstEventTime();
    
    for(auto& [thisPitch, thisVol, active] : midiStream){
        if(notesToIgnore.contains(thisPitch))
            notesToIgnore.removeAllInstancesOf(thisPitch);
        else
            midiMessages.addEvent(MidiMessage::noteOn(1, thisPitch, static_cast<uint8>(thisVol)), midiStart);
        
        activeNotes.add(thisPitch);
    }
    midiStream.clearQuick();
}

constexpr void PianoRoll1AudioProcessor::checkIfTooManyNotes(juce::MidiBuffer &midiMessages){
    const int midiStart = midiMessages.getFirstEventTime();
    
    if (activeNotes.size()>maxNumOfActiveNotes)
        midiMessages.addEvent(MidiMessage::noteOff(1, activeNotes.removeAndReturn(0)), midiStart);
}

void PianoRoll1AudioProcessor::prepToPlayNote(const int note, const int div){
    const int beatSwitch = divToBeatSwitch(div);
    if(presets[currentPreset]->tracks[currentTrack]->beatSwitch[note/div] == beatSwitch){
        auto& [monoPitch, monoVol, active] = getMonoNote(note, beatSwitch);

        if(isMono() && active)
            playNote(monoPitch, monoVol);
        else if(!isMono()){ //isPoly
            auto [pitches, polyVol] = getPolyNote(note, beatSwitch);
            for(auto& pitch : pitches) playNote(pitch, polyVol);
        }
    }
}


constexpr void PianoRoll1AudioProcessor::playNote(int pitch, int volume){
    midiStream.add(Note{pitch, volume, true});
}


void PianoRoll1AudioProcessor::sequencerCheck(juce::Value &value){
    const auto val = value.getValue();
    const float floatVal = val.toString().getFloatValue();
    float valDecimals = std::fmod(floatVal, 1.0f);
    const bool isPlaying = lastPosInfo.isPlaying;
    
    //Check if beat has changed (example: 1st to 2nd beat of measure.)
    if(previousVal != -1.0f && floor(previousVal) < floor(floatVal)){
        beatIndex = (beatIndex+1) % presets[currentPreset]->numOfBeats;
    }
    
    currentBeat = static_cast<float>(beatIndex) + valDecimals;
    
    midiInputStreamToNoteArrays();
    
    if(isPlaying){
        checkIfNoteGridPassed(valDecimals);
    }else{
        beatIndex = 0;
        valDecimals = 0.0f;
        sixteenthCounter = 3;
        tripletCounter = 2;
    }
    
    const int currentNumOfBeats = presets[currentPreset]->numOfBeats;

    playPosition.setValue(std::fmod(currentBeat, static_cast<float>(currentNumOfBeats) ) / static_cast<float>(currentNumOfBeats) );
    previousVal = floatVal;
}


constexpr void PianoRoll1AudioProcessor::midiInputStreamToNoteArrays(){
    //THIS CODE IS CALLED WHEN THE USER IS PLAYING A MIDI INSTRUMENT.
    //Move incoming midi instrument stream into the actual sequencer. Performed notes are thus saved.
    while(midiInstrumentStream.size()>0){
        int beatSwitch = presets[currentPreset]->tracks[currentTrack]->beatSwitch[beatIndex];
        int div = beatSwitchToDiv(beatSwitch);
        uint8 pitch = midiInstrumentStream[0].first;
        uint8 vol = midiInstrumentStream[0].second;
        float val = currentBeat * static_cast<float>(div);
        int roundedVal = ( static_cast<int>(std::round(val)) ) % (presets[currentPreset]->numOfBeats*div);
        
        if (isMono()){
            auto& [thisPitch, thisVol, active] = getMonoNote(roundedVal, beatSwitch);
            
            thisPitch = pitch;
            thisVol = vol;
        }else{ //isPoly
            //auto& thisPolyNote = getPolyNote(roundedVal, beatSwitch);
            
            //thisPolyNote.add(pitch);
        }
        if (std::fmod(val, 1.0f) >= 0.5) //Rounding into the upcoming note on the grid.
            notesToIgnore.add(pitch);
        midiInstrumentStream.remove(0);
    }
}

constexpr void PianoRoll1AudioProcessor::checkIfNoteGridPassed(const float valDecimals){
    if(valDecimals >= 0.0f && valDecimals < 0.25f && sixteenthCounter == 3){
        sixteenthCounter = 0;
        sixteenth = beatIndex*4;
        prepToPlayNote(sixteenth, 4);
    }else if(valDecimals>=0.25f && sixteenthCounter == 0){
        sixteenthCounter = 1;
        sixteenth = beatIndex*4 + 1;
        prepToPlayNote(sixteenth, 4);
    }else if(valDecimals>=0.50f && sixteenthCounter == 1){
        sixteenthCounter = 2;
        sixteenth = beatIndex*4 + 2;
        prepToPlayNote(sixteenth, 4);
    }else if(valDecimals>=0.75f && sixteenthCounter == 2){
        sixteenthCounter = 3;
        sixteenth = beatIndex*4 + 3;
        prepToPlayNote(sixteenth, 4);
    }
    
    if(valDecimals >= 0.0f && valDecimals < 0.33f && tripletCounter == 2){
        tripletCounter = 0;
        triplet = beatIndex*3;
        prepToPlayNote(triplet, 3);
    }else if(valDecimals>=0.33f && tripletCounter == 0){
        tripletCounter = 1;
        triplet = beatIndex*3 + 1;
        prepToPlayNote(triplet, 3);
    }else if(valDecimals>=0.66f && tripletCounter == 1){
        tripletCounter = 2;
        triplet = beatIndex*3 + 2;
        prepToPlayNote(triplet, 3);
    }
}


void PianoRoll1AudioProcessor::parameterChanged(const juce::String &parameterID, float newValue){
    
}

void PianoRoll1AudioProcessor::oscMessageReceived(const juce::OSCMessage &Message){
    osc_Interpreter.OSC_Interpret(Message);
}

void PianoRoll1AudioProcessor::resetAll(){
    presets.clear();
    for(int preset=0;preset<=numOfPresets;preset++)
        presets.add(new Preset);
    currentPreset=1;
    currentTrack=1;
}

void PianoRoll1AudioProcessor::octaveShift(int numOfOctaves){
    auto thisTrack = presets[currentPreset]->tracks[currentTrack];
    int currentOctaveShift = thisTrack->octaveShift;
    
    if(currentOctaveShift + numOfOctaves < 6 && currentOctaveShift + numOfOctaves > -6){
        if (isMono()){
            for(auto &note : thisTrack->sixteenthNotes) note.pitch+=(numOfOctaves*12);
            for(auto &note : thisTrack->tripletNotes)   note.pitch+=(numOfOctaves*12);
        }else{ //isPoly
            //TODO
        }
    }
}


