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
    
    NoteHead(const uint8 pitch, const int _diatonicNoteValue, const int _diatonicModValue){
        notePitch = pitch;
        diatonicNoteValue = _diatonicNoteValue;
        diatonicModValue = _diatonicModValue;
        accidental = Theory::diatonicMatrixToAccidentals(diatonicModValue);
    }
    
    NoteHead(const uint8 pitch){
        NoteHead(pitch, -1, -1);
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

//==========================================================================================

class Staff : public Component,
public PianoRollComponent
{
public:
    std::vector<NoteHead> notes;
    
    Staff(OwnedArray<Preset> * processorPresetLocation, int * currentPresetPointer);
    
    
    
private:
    Clef clef = TREBLE;
    OpusLookAndFeel opusLookAndFeel;
    int * currentPreset;
    
    void paint (Graphics& g) override;
};
