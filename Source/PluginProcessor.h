/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PianoRollComponent.h"
#define PRESET_ID "current_preset"
#define PRESET_NAME "Current_Preset"
#define TRACK_ID "current_track"
#define TRACK_NAME "Current_Track"
#define BEATS_ID "current_beats"
#define BEATS_NAME "Current_Beats"


//==============================================================================
/**
*/


class PianoRoll1AudioProcessor  : public AudioProcessor,
                                  public PianoRollComponent,
                                  public AudioProcessorValueTreeState::Listener,
                                  public OSCReceiver,
                                  public OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
    //==============================================================================
     PianoRoll1AudioProcessor();
    ~PianoRoll1AudioProcessor();
    //==============================================================================
 
    
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    bool isStandalone = JUCEApplication::isStandaloneApp();
    bool isPlugin = !JUCEApplication::isStandaloneApp();

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    const int numOfPresets = PianoRollComponent::numOfPresets;
    const int numOfTracks = PianoRollComponent::numOfTracks;
    const int maxBeats = PianoRollComponent::maxBeats;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void updateCurrentTimeInfoFromHost();
    
    //==============================================================================
    int preset, numerator, denominator;
    AudioProcessorValueTreeState treeState;
    
    // this keeps a copy of the last set of time info that was acquired during an audio
    // callback - the UI component will read this and display it.
    AudioPlayHead::CurrentPositionInfo lastPosInfo;
    
    int updateCounter;
    Value playPosition;
    Value playPositionToSendPlayheadUpdate;
    Value internalPlayPosition;
    Array<Note> midiStream;
    Array<std::pair<int8, int8>> midiInstrumentStream;
    Array<Atomic<int>> newMidiStream[12];
    Array<int> notesToIgnore; //Just played on the midi controller. Avoid double play.
    
    void prepToPlayNote(const int note, const int div);
    void playNote(int pitch, int volume);
    
    float currentBeat;
    float previousVal;
    int sixteenthCounter;
    int tripletCounter;
    int beatIndex, sixteenth, triplet;
    void sequencerCheck(juce::Value &value);
    void resetAll();
    void rootChanged(const int root); //TODO
    void scaleChanged(const String scaleName); //TODO
    void octaveShift(int numOfOctaves);
    
    int root;
    Array<int> scale{0,2,4,5,7,9,11};

private:
    //AudioParameterFloat* playCursor;
    void parameterChanged(const String& parameterID, float newValue) override;
    void oscMessageReceived(const OSCMessage &Message) override;
    
    void midiInputStreamToNoteArrays();
    void checkIfNoteGridPassed(const float valDecimals);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRoll1AudioProcessor)
};
