/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "PianoRoll.h"
#include "VolumePanel.h"
#include "PianoRollComponent.h"
#include "PlayCursorWindow.h"
#include "Theory.h"
#include <algorithm>
#include <random>


//==============================================================================
/**
*/


class PianoRoll1AudioProcessorEditor  : public AudioProcessorEditor,
                                        public AudioProcessorValueTreeState::Listener,
                                        public Value::Listener,
                                        public Slider::Listener,
                                        public KeyListener,
                                        public Button::Listener,
                                        public Timer,
                                        public PianoRollComponent
{
public:
    
    PianoRoll1AudioProcessorEditor (PianoRoll1AudioProcessor&);
    ~PianoRoll1AudioProcessorEditor();
    PianoRoll1AudioProcessor& processor; //Our audio processor
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> presetSliderAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> trackSliderAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> beatSliderAttach;
    
    const Colour greyOff = Colour(128,128,128);
    const Colour whiteBlue = Colour(195,223,226);
    const Colour beatCanvasJungleGreen = Colour(152,152,115);
    
    //==============================================================================
    void paintOverChildren(Graphics& g) override;
    void timerCallback() override;
    void resized() override;
    
    void setMidiDisplay(int midi);
    void updatePreset(int preset);
    void updateNoteName(const String newNoteName);
    void prepToPlayNote(int note, int div);
    float dbToVolume(const float db);
    float volumeToDB(const float vol);
    void playNote(const int pitch, const int volume);
    
    bool keyPressed(const juce::KeyPress &key, juce::Component *originatingComponent) override;
    void sliderValueChanged(Slider* slider) override;
    void parameterChanged (const String& parameterID, float newValue) override;
    void valueChanged(Value &value) override;
    void rootMenuChanged();
    void scaleMenuChanged();
    void monoPolyMenuChanged();
    void generatorMenuChanged();
    void arpDirectionMenuChanged();
    
    String playPosition;
    TextButton midiLabel;
    Label noteLabel;
    PianoRoll pianoRoll;
    PianoKeys pianoKeys;
    Staff auditionStaff;
    Staff scaleDisplayStaff;
    VolumePanel volumePanel;
    PlayCursorWindow playCursorWindow;
    ComboBox rootMenu;
    ComboBox scaleMenu;
    ComboBox monoPolyMenu;
    ComboBox generatorMenu;
    ComboBox arpDirectionMenu;
    TextButton generateButton;
    Random random;
    bool isChildOfBeatCanvas;
    
private:
    Slider presetSlider;
    Slider trackSlider;
    Slider beatSlider;
    
    int currentNumOfBeats;
    int internalBeat;
    int beatIndex;
    int sixteenthCounter;
    int tripletCounter;
    int presetToBeUpdated;
    int beatsToBeUpdated;
    int midiNoteNum;
    Value noteName;
    float currentBeat;
    float previousVal;
    const bool pianoKeyPanel = true;
    const float sliderSpacing = 0.25f;
    const static int numOfPresets = PianoRollComponent::numOfPresets;
    const static int numOfTracks = PianoRollComponent::numOfTracks;
    const static int maxBeats = PianoRollComponent::maxBeats;
    const float topBorder = 0.14f;
    const float pianoKeyWidth = 0.06;
    Array<Array<float>> tripletSwitches;
    Array<int> beatsClicked;
    String arpeggioDirection;
    Slider arpSlider;

    void mouseUp(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void buttonClicked (Button*) override;
    
    void paint (Graphics&) override;
    void drawTripletSwitches(Graphics * g, const int numOfBeats, const float height, const float width);
    
    void addItemsToMenu(ComboBox& comboBox, Array<String> list);
    
    std::vector<int> brokenArpeggio(Array<int> currentScale);
    std::vector<int> previousOrder;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRoll1AudioProcessorEditor)
};
