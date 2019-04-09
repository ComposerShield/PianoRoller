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
#include "PluginProcessor.h"
#include <map>
#include "MusicStaff.h"

//==============================================================================


class PianoKeys;



class PianoRoll    : public Component,
                     public PianoRollComponent
{
public:
    
    PianoRoll(Staff& auditionStaffLocation, PianoKeys& pianoKeyLocation);
    ~PianoRoll();
    
    int topNote;
    
    void changeBeatCanvasPreset(const int preset);
    void changeBeatCanvasTrack(const int track);
    void changeBeatCanvasBeats(const int beats);
    void changeBeatCanvasTriplet(const int beat, int val);
    void spacebar();
    Value * noteName;
    int * midiNoteNum;
    
private:
    void paint (Graphics&) override;
    constexpr void drawRows(PaintData p);
    constexpr void drawRowLines(PaintData p);
    constexpr void monoNoteFill(PaintData p, const int col, const int beatSwitch, const float thisNoteWidth);
    constexpr void polyNoteFill(PaintData p, const int col, const int beatSwitch, const float thisNoteWidth);
    void drawNotes(PaintData p);
    constexpr void monoWriteNote(const int thisCol, const int pitch, const int beatSwitch, const MouseEvent& event);
    constexpr void polyWriteNote(const int thisCol, const int pitch, const int beatSwitch, const MouseEvent& event);
    
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
    std::pair<int, int> polySelectedNote; //col, pitch
    bool canScroll;
    bool isChildOfBeatCanvas;
    PianoKeys& pianoKeys;
    Staff& auditionStaff;
    constexpr Clef clefDisplay(int pitch);
    
    int prevPitch;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRoll)
    
};

//===================Piano Keys===================//

class PianoKeys    : public Component,
                     public PianoRollComponent
{
public:
    PianoRoll& pianoRoll;
    Staff& auditionStaff;
    
    PianoKeys(PianoRoll& pianoRollInput, Staff& auditionStaff);
    
    int selectedKey;
    
private:
    void paint (Graphics&) override;
    void drawRows(PaintData p);
    void mouseUp(const MouseEvent& event) override;
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    
    int currentSelectedKey;
    int topNote;
};

