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
        //preset(1),
        //treeState(*this, nullptr)
        treeState(*this, nullptr, "PARAMETERS", {
            std::make_unique<AudioParameterInt> (PRESET_ID, PRESET_NAME, 1, numOfPresets, 1),
            std::make_unique<AudioParameterInt> (TRACK_ID, TRACK_NAME, 1, numOfTracks, 1),
            std::make_unique<AudioParameterInt> (BEATS_ID, BEATS_NAME, 1, maxBeats, 1)
        })
#endif
{
    processorPresets = &presets;
    
    NormalisableRange<float> presetRange(1.0f,(float) PianoRollComponent::numOfPresets);
    NormalisableRange<float> trackRange(1.0f,(float) PianoRollComponent::numOfTracks);
    NormalisableRange<float> beatsRange(1.0f, (float) PianoRollComponent::maxBeats);
    
    //Value Tree State===========================================================
    treeState.addParameterListener(PRESET_ID, this); //Adds listener to preset slider tree value.
    treeState.addParameterListener(TRACK_ID, this); //Adds listener to track slider tree value.
    treeState.addParameterListener(BEATS_ID, this); //Adds listener to track slider tree value.
    
    //OSC MESSAGES===============================================================
    if (! connect(6449)){}; //Connect to OSC messages from Max.
    juce::OSCReceiver::addListener(this, "/juce");

}

PianoRoll1AudioProcessor::~PianoRoll1AudioProcessor()
{
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
    midiStream.clear();
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
    jassert (buffer.getNumChannels() == 0); //Set number of audio channels to 0
    const float numOfSamps = buffer.getNumSamples();
    const float refreshCounter = (2048 / numOfSamps); //The frequency our current beat position is sent to the Plugin
                                                //Editor.
    const int midiStart = midiMessages.getFirstEventTime();
    buffer.clear();
    
    if (auto* ph = getPlayHead())
    {
        AudioPlayHead::CurrentPositionInfo newTime;
        if (ph->getCurrentPosition (newTime))
        {
            lastPosInfo = newTime;  // Successfully got the current time from the host..
        }
    }
    
    //Update current beat position from host into PluginEditor to draw vertical play line.
    //Not performed on every processBlock callback,
    if(updateCounter==0){
        //playPosition.setValue(lastPosInfo.ppqPosition);
        //playPositionToSendPlayheadUpdate.setValue(lastPosInfo.ppqPosition);
    }
    //updateCounter = (updateCounter + 1) % (int)refreshCounter;
    
    
    internalPlayPosition.setValue(lastPosInfo.ppqPosition);
    
    //Add notes played on an external midi instrument into the sequencer when playing.
    if(lastPosInfo.isPlaying){
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
        //midiMessages.clear(); //Clear all midi messages because they will be added to the sequencer.
    }
    
    
    sequencerCheck(internalPlayPosition); //Sends current position to the sequencer to see if notes need to be played.

    //Midi notes are put into a "midi stream" in the PluginEditor and extracted below to send out.
    if(lastPosInfo.isPlaying){
        
        for(int newMidiNote = 0; newMidiNote<midiStream.size(); newMidiNote++){
            Array<int> toPlay = midiStream[newMidiNote];
            uint8 pitch = toPlay[0];
            uint8 velocity = toPlay[1];
            
            if(notesToIgnore.contains(pitch)){
                notesToIgnore.removeAllInstancesOf(pitch);
            }else{
                midiMessages.addEvent(MidiMessage::noteOff(1, pitch), midiStart);
                midiMessages.addEvent(MidiMessage::noteOn(1, pitch, (uint8) velocity), midiStart);
            }
        }
        midiStream.clear();
        
    }else{ //Not playing
        notesToIgnore.clear();
    }
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



void PianoRoll1AudioProcessor::prepToPlayNote(const int note, const int div){
    int pitch;
    int vol;
    const int beatSwitch = [&]() -> int{
        if(div==4){return presets[currentPreset]->tracks[currentTrack]->beatSwitch[note/4];}
        else if(div==3){return presets[currentPreset]->tracks[currentTrack]->beatSwitch[note/3];}
        else{return 0;}
    }();
    
    if(presets[currentPreset]->isMono){
        if (div == 4 && beatSwitch==0){
            pitch = presets[currentPreset]->tracks[currentTrack]->sixteenths[note];
            vol = presets[currentPreset]->tracks[currentTrack]->sixteenthVols[note];
        }
        else if(div == 3 && beatSwitch==1){
            pitch = presets[currentPreset]->tracks[currentTrack]->triplets[note];
            vol = presets[currentPreset]->tracks[currentTrack]->tripletVols[note];
        }else{pitch=0;vol=0;}
        playNote(pitch, vol);
        
    }else{ //isPoly
        if (div == 4 && beatSwitch==0){
            Array<Array<int>> * noteArrays = &presets[currentPreset]->tracks[currentTrack]->polySixteenths;
            Array<int> thisNoteArray = noteArrays->operator[](note);
            vol = presets[currentPreset]->tracks[currentTrack]->polySixteenthVols[note];
            for(int i=0;i<thisNoteArray.size();i++){
                pitch = thisNoteArray[i];
                playNote(pitch, vol);
            }
        }
        else if(div == 3 && beatSwitch==1){
            Array<Array<int>> * noteArrays = &presets[currentPreset]->tracks[currentTrack]->polyTriplets;
            Array<int> thisNoteArray = noteArrays->operator[](note);
            vol = presets[currentPreset]->tracks[currentTrack]->polyTripletVols[note];
            for(int i=0;i<thisNoteArray.size();i++){
                pitch = thisNoteArray[i];
                playNote(pitch, vol);
            }
        }else{pitch=0;vol=0;}
    }
}


void PianoRoll1AudioProcessor::playNote(int pitch, int volume){
    if (pitch > 0){
        Array<int> toPlay;
        toPlay.add(pitch);
        toPlay.add(volume);
        midiStream.add(toPlay);
    }
}


void PianoRoll1AudioProcessor::sequencerCheck(juce::Value &value){
    auto val = value.getValue();
    float floatVal = val.toString().getFloatValue();
    float valDecimals = std::fmod(floatVal, 1.0f);
    bool isPlaying = lastPosInfo.isPlaying;
    int sixteenth, triplet;
    
    //Check if beat has changed (example: 1st to 2nd beat of measure.)
    if(previousVal != -1.0f && floor(previousVal) < floor(floatVal)){
        beatIndex = (beatIndex+1) % presets[currentPreset]->numOfBeats;
    }
    
    //
    
    
    if (isPlaying == false){
        beatIndex = 0;
        valDecimals = 0.0f;
        sixteenthCounter = 3;
        tripletCounter = 2;
    }
    
    currentBeat = (float)beatIndex + valDecimals;
    
    //THIS CODE IS CALLED WHEN THE USER IS PLAYING A MIDI INSTRUMENT.
    //Move incoming midi instrument stream into the actual sequencer. Performed notes are thus saved.
    while(midiInstrumentStream.size()>0){
        int beatSwitch = presets[currentPreset]->tracks[currentTrack]->beatSwitch[beatIndex];
        uint8 pitch = midiInstrumentStream[0].first;
        uint8 vol = midiInstrumentStream[0].second;
        Array<Array<int>> * polyNotes;
        Array<int>newPitchArray;
        
        if(beatSwitch==0){
            float sixteenth = currentBeat*4.0f;
            int roundedSixteenth = (int)std::round(sixteenth) % (presets[currentPreset]->numOfBeats*4);
            if (std::fmod(sixteenth, 1.0f) >= 0.5){ //Rounding into the upcoming note on the grid.
                notesToIgnore.add(pitch);
            }
            
            if (presets[currentPreset]->isMono){
                presets[currentPreset]->tracks[currentTrack]->sixteenths.set(roundedSixteenth, pitch);
                presets[currentPreset]->tracks[currentTrack]->sixteenthVols.set(roundedSixteenth, vol);
            }else{ //isPoly
                polyNotes = &presets[currentPreset]->tracks[currentTrack]->polySixteenths;
                newPitchArray = (*polyNotes).operator[](roundedSixteenth);
                newPitchArray.add(pitch);
                presets[currentPreset]->tracks[currentTrack]->polySixteenths.set(roundedSixteenth, newPitchArray);
                //presets[currentPreset]->tracks[currentTrack]->polySixteenths[roundedSixteenth].add(pitch);
                presets[currentPreset]->tracks[currentTrack]->polySixteenthVols.set(roundedSixteenth, vol);
            }
        }else if(beatSwitch==1){
            float triplet = currentBeat*3.0f;
            int roundedTriplet = (int)std::round(triplet) % (presets[currentPreset]->numOfBeats*3);
            if (presets[currentPreset]->isMono){
                presets[currentPreset]->tracks[currentTrack]->sixteenths.set(roundedTriplet, pitch);
                presets[currentPreset]->tracks[currentTrack]->tripletVols.set(roundedTriplet, vol);
            }else{ //isPoly
                polyNotes = &presets[currentPreset]->tracks[currentTrack]->polyTriplets;
                newPitchArray = (*polyNotes).operator[](roundedTriplet);
                newPitchArray.add(pitch);
                presets[currentPreset]->tracks[currentTrack]->polyTriplets.set(roundedTriplet, newPitchArray);
                //presets[currentPreset]->tracks[currentTrack]->polyTriplets[roundedTriplet].add(pitch);
                presets[currentPreset]->tracks[currentTrack]->polyTripletVols.set(roundedTriplet, vol);
            }
        }
        
        midiInstrumentStream.remove(0);
        
    }
    
    if(isPlaying){
        
        if(valDecimals>=0.0f && valDecimals<0.25f && sixteenthCounter == 3){
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
        
        
        if(valDecimals>=0.0f && valDecimals<0.33f && tripletCounter == 2){
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
    
    //playPosition = std::to_string(beatIndex) + "  " + std::to_string(currentBeat);
    
    int currentNumOfBeats = presets[currentPreset]->numOfBeats;
    
    playPosition.setValue(std::fmod(currentBeat, (float)currentNumOfBeats) / (float)currentNumOfBeats);
    previousVal = floatVal;
}





void PianoRoll1AudioProcessor::parameterChanged(const juce::String &parameterID, float newValue){
    
}

void PianoRoll1AudioProcessor::oscMessageReceived(const juce::OSCMessage &Message){
    //"Function" calls.
    if (Message.size() > 0 && Message[0].isString()){
        
        //setPreset(int preset)
        if(Message[0].getString() == "setPreset"){
            int preset = Message[1].getInt32();
            updatePreset(preset);
        }
        
        //updateNote(int col, int pitch, int beatSwitch)
        else if(Message[0].getString() == "updateNote"){
            int col = Message[1].getInt32();
            int pitch = Message[2].getInt32();
            int beatSwitch = Message[3].getInt32();
            updateNote(col, pitch, beatSwitch);
        }
        
        //void PianoRoll::updateBeatSwitch(int beat)
        else if(Message[0].getString() == "updateBeatSwitch"){

        }
        
        else if(Message[0].getString() == "updateVolume"){
            int col = Message[1].getInt32();
            int vol = Message[2].getInt32();
            int beatSwitch = Message[3].getInt32();
            updateVolume(col, vol, beatSwitch);
        }
        
        else if(Message[0].getString() == "updateNumOfBeats"){
            int beats = Message[1].getInt32();
            int preset;
            if (Message.size() == 3){preset = Message[2].getInt32();}
            else{preset = currentPreset;}
            updateNumOfBeats(beats, preset);
        }
        
        
        if(Message[0].getString() == "changeRhythmDiv"){
            int track = Message[1].getInt32();
            int beat = Message[2].getInt32();
            int beatSwitch = Message[3].getInt32();
            changeRhythmDiv(track, beat, beatSwitch);
        }
        
        if(Message[0].getString() == "currentPreset"){
            int preset = Message[1].getInt32();
            updatePreset(preset);
        }
        if(Message[0].getString() == "currentTrack"){
            int track = Message[1].getInt32();
            updateTrack(track);

        }
        
        //void PianoRollComponent::noteOnOff(int track, int div, int note, int onOff)
        if(Message[0].getString() == "noteOnOff"){
            int track = Message[1].getInt32();
            int div = Message[2].getInt32();
            int note = Message[3].getInt32();
            int onOff = Message[4].getInt32();
            noteOnOff(track, div, note, onOff);
        }
        
        //void PianoRollComponent::copyPreset(int presetSource, int presetReplaced)
        if(Message[0].getString() == "copyPresets"){
            int presetSource = Message[1].getInt32();
            int presetReplaced = Message[2].getInt32();
            copyPreset(presetSource, presetReplaced);
        }
        
        //setPitch(const int track, const int div, const int note, const int pitch, const int preset)
        if(Message[0].getString() == "setPitch"){
            int track = Message[1].getInt32();
            int div = Message[2].getInt32();
            int note = Message[3].getInt32();
            int pitch = Message[4].getInt32();
            int preset = Message[5].getInt32();
            setPitch(track, div, note, pitch, preset);
        }
        
        

    }
}

void PianoRoll1AudioProcessor::resetAll(){
    presets.clear();
    for(int preset=0;preset<=numOfPresets;preset++){
        presets.add(new Preset);
    }
    currentPreset=1;
    currentTrack=1;
}

void PianoRoll1AudioProcessor::octaveShift(int numOfOctaves){
    int currentOctaveShift = presets[currentPreset]->tracks[currentTrack]->octaveShift;
    if(currentOctaveShift + numOfOctaves < 6 && currentOctaveShift + numOfOctaves > -6){
        for(int beat=0;beat<presets[currentPreset]->numOfBeats;beat++){
            int beatSwitch = presets[currentPreset]->tracks[currentTrack]->beatSwitch[beat];
            int pitchShift = numOfOctaves*12;
            if(beatSwitch == 0){
                for(int sixteenthIndex=0;sixteenthIndex<4;sixteenthIndex++){
                    int noteIndex = beat*4+sixteenthIndex;
                    previousVal = presets[currentPreset]->tracks[currentTrack]->sixteenths[noteIndex];
                    presets[currentPreset]->tracks[currentTrack]->sixteenths.set(noteIndex, previousVal+pitchShift);
                }
            }else if(beatSwitch == 1){
                for(int tripletIndex=0;tripletIndex<3;tripletIndex++){
                    int noteIndex = beat*3+tripletIndex;
                    previousVal = presets[currentPreset]->tracks[currentTrack]->triplets[noteIndex];
                    presets[currentPreset]->tracks[currentTrack]->triplets.set(noteIndex, previousVal+pitchShift);
                }
            }
        }
        
        presets[currentPreset]->tracks[currentTrack]->octaveShift += numOfOctaves;
    }
}


