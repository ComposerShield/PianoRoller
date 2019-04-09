/*
  ==============================================================================

    MusicStaff.h
    Created: 31 Mar 2019 2:03:25pm
    Author:  Adam Shield

  ==============================================================================
*/

#pragma once

#include "Theory.h"



struct NoteHead{
    uint8 notePitch;
    String noteName;
    int diatonicNoteValue;
    int diatonicModValue;
    Accidental accidental;
    
    
    NoteHead(const uint8 pitch, const int _diatonicNoteValue, const int _diatonicModValue, const bool repeatDiatonicVal){
        notePitch = pitch;
        diatonicNoteValue = _diatonicNoteValue;
        diatonicModValue = _diatonicModValue;
        accidental = (repeatDiatonicVal && _diatonicModValue==2) ? COURTESY_NATURAL
                                                                 : Theory::diatonicMatrixToAccidentals(diatonicModValue);
    }
    
    NoteHead(const uint8 pitch){
        NoteHead(pitch, -1, -1, false);
        accidental = NO_PREFERENCE;
    }
};

//==========================================================================================

class OpusLookAndFeel : public LookAndFeel_V4
{
public:
    Typeface::Ptr getTypefaceForFont(const Font& f) override{
        static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::OpusStd_otf,
                                                                        BinaryData::OpusStd_otfSize);
        return myFont;
    }
    
    static const Font& getOpus()
    {
        static Font opus (Font (Typeface::createSystemTypefaceFor (BinaryData::OpusStd_otf,
                                                                   BinaryData::OpusStd_otfSize)));
        return opus;
    }
    
    OpusLookAndFeel()
    {
        setDefaultSansSerifTypeface(Typeface::createSystemTypefaceFor(BinaryData::OpusStd_otf,
                                                                      BinaryData::OpusStd_otfSize));
    }
};
//
//==========================================================================================

class Staff : public Component,
public PianoRollComponent
{
public:
    std::vector<NoteHead> notes;
    
    Staff();
    Clef clef = TREBLE;
    
private:
    OpusLookAndFeel opusLookAndFeel;
    
    void paint(Graphics& g) override;
    void drawClef(Graphics &g, const float width, const float height, const float clefHeight, const String clefText);
    void drawStaffLines(Graphics& g, const int width, const int lineSpacing);
    void drawNotes(Graphics& g);
    
    void checkAccidental(Accidental& accidental, const Array<int> modeNotes, const Array<int> enharmIndex, const int pitchSetClass) const;
};
