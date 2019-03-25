/*
 ==============================================================================
 
 PianoRoll.h
 Created: 29 Dec 2018 10:30:33pm
 Author:  Adam Shield
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PianoRollComponent.h"
#include "Theory.h"
#include "PluginProcessor.h"
#include <map>

//==============================================================================




class PianoRoll    : public Component,
                     public PianoRollComponent
{
public:
    
    
    
    PianoRoll(OwnedArray<Preset> * processorPresetLocation, Staff * auditionStaffLocation, PianoRollComponent * pianoKeyLocation);
    ~PianoRoll();
    
    int topNote;
    
    void changeBeatCanvasPreset(const int preset);
    void changeBeatCanvasTrack(const int track);
    void changeBeatCanvasBeats(const int beats);
    void changeBeatCanvasTriplet(const int beat, int val);
    void spacebar();
    Value * noteName;
    
private:
    void paint (Graphics&) override;
    void drawRows(PaintData p);
    void drawRowLines(PaintData p);
    void monoNoteFill(PaintData p, const Array<int> * noteArray, const int col, const float thisNoteWidth);
    void polyNoteFill(PaintData p, const Array<Array<int>> * polyNoteArray, const int col, const float thisNoteWidth);
    void drawNotes(PaintData p);

    void mouseEnter(const MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;
    void mouseDoubleClick (const MouseEvent&) override;
    void resized() override;
    
    MidiMessage midiMessage;
    void (*setParentMidiDisplay)(int);
    int polySelectedNote;
    bool canScroll;
    bool isChildOfBeatCanvas;
    bool isDoubleClick;
    PianoRollComponent * pianoKeys;
    Staff * auditionStaff;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRoll)
    
    
    
};

//===================Piano Keys===================//

class PianoKeys    : public Component,
                     public PianoRollComponent
{
public:
    PianoRoll * pianoRoll;
    Staff * auditionStaff;
    
    PianoKeys(PianoRoll * pianoRollInput);
    
private:
    void paint (Graphics&) override;
    void drawRows(PaintData p);
    void mouseUp(const MouseEvent& event) override;
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    
    int currentSelectedKey;
    int topNote;
    
};

