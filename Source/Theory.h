/*
  ==============================================================================

    Theory.h
    Created: 18 Jan 2019 1:13:06pm
    Author:  Adam Shield

  ==============================================================================
*/


#ifndef MUSIC_THEORY_HEADER
#define MUSIC_THEORY_HEADER
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <map>
#include <iostream>
#include "PianoRollComponent.h"
#define for_indexed(...) for_indexed_v(i, __VA_ARGS__)
#define for_indexed_v(v, ...) for(bool _i_ = true, _break_ = false; _i_;) for(size_t v = 0; _i_; _i_ = false) for(__VA_ARGS__) if(_break_) break; else for(bool _j_ = true; _j_;) for(_break_ = true; _j_; _j_ = false) for(bool _k_ = true; _k_; v++, _k_ = false, _break_ = false)

enum Clef{
    TREBLE,
    BASS,
    TREBLE_8VA,
    TREBLE_15MA,
    BASS_8VA,
    BASS_15MA
};


enum Accidental{
    SHARP,
    FLAT,
    DOUBLE_SHARP,
    DOUBLE_FLAT,
    NO_PREFERENCE,
    NATURAL
};

namespace Theory{
    
    struct Mode{
        Array<int> mode;
        Array<int> enharmIndex;
        Array<int> intervals;
        Mode(const Array<int> Array1, const Array<int> Array2, const Array<int> Array3){
            mode = Array1;
            enharmIndex = Array2;
            intervals = Array3;
        }
        Mode(){}
        
        const Array<int> getMode()        { return mode;        }
        const Array<int> getEnharmIndex() { return enharmIndex; }
        const Array<int> getIntervals()   { return intervals;   }
    };
    
    
    struct Scale : public Mode{
        Scale(const Array<int> Array1, const Array<int> Array2, const Array<int> Array3) : Mode(Array1, Array2, Array3)
        {
        }
    };
    
    struct Chord : public Mode{
        Chord(const Array<int> Array1, const Array<int> Array2, const Array<int> Array3) : Mode(Array1, Array2, Array3)
        {
        }
    };
    
    const int blackKeys[5] = {1,3,6,8,10};
    
    /*
     * Scales are built using Arrays of
     *      1. Their pitch classes.
     *      2. Their "enharmonic index" (i.e. which note deviates from the major scale, -1 would be flat for instance)
     *      3. Their intervals (which degrees of the diatonic scale do they draw from)
     *
     */
    namespace Scales{
        
                                       //---mode-----------------enharmIndex-------intervals-------//
        inline const Scale  major =      { {0,2,4,5,7,9,11},    {0,0,0,0,0,0,0},        {1,2,3,4,5,6,7} },
                            natMinor =   { {0,2,3,5,7,8,10},    {0,0,-1,0,0,-1,-1},     {1,2,3,4,5,6,7} },
                            harmMinor =  { {0,2,3,5,7,8,11},    {0,0,-1,0,0,-1,0},      {1,2,3,4,5,6,7} },
                            melMinor =   { {0,2,3,5,7,9,11},    {0,0,-1,0,0,0,0},       {1,2,3,4,5,6,7} },
                            locrian =    { {0,1,3,5,6,8,1},     {0,-1,-1,0,-1,-1,-1},   {1,2,3,4,5,6,7} },
                            dorian =     { {0,2,3,5,7,8,10},    {0,-1,-1,0,0,-1,-1},    {1,2,3,4,5,6,7} },
                            phrygian =   { {0,1,3,5,7,8,10},    {0,-1,-1,0,0,-1,-1},    {1,2,3,4,5,6,7} },
                            lydian =     { {0,2,4,6,7,9,11},    {0,0,0,1,0,0,0},        {1,2,3,4,5,6,7} },
                            mixolydian = { {0,2,4,5,7,9,10},    {0,-1,-1,0,-1,-1,-1},   {1,2,3,4,5,6,7} },
                            wholeTone =  { {0,2,4,6,8,10},      {0,0,0,1,1,1},          {1,2,3,4,5,6} },
                            pentaMaj =   { {0,2,4,7,9},         {0,0,0,0,0},            {1,2,3,5,6} },
                            pentaMin =   { {0,3,5,7,10},        {0,-1,0,0,-1},          {1,3,4,5,7} },
                            pentaMajB =  { {0,2,3,4,7,9},       {0,0,-1,0,0,0},         {1,2,3,3,5,6} },
                            pentaMinB =  { {0,3,5,6,7,10},      {0,-1,0,-1,0,-1},       {1,3,4,5,5,7} },
                            dimWholeHalf={ {0,2,3,5,6,8,9,11},  {0,0,-1,0,-1,-1,0,0},   {1,2,3,4,5,6,6,7} },
                            dimHalfWhole={ {0,1,3,4,6,7,9,10},  {0,-1,-1,-1,-1,0,0,-1}, {1,2,3,4,5,5,6,7} },
                            phrygDom =   { {0,1,4,5,7,8,10},    {0,-1,0,0,0,-1,-1},     {1,2,3,4,5,6,7} },
                            lydDom =     { {0,2,4,6,7,9,10},    {0,0,0,1,0,0,-1},       {1,2,3,4,5,6,7} },
                            yo =         { {0,2,5,7,9},         {0,0,0,0,-1},           {1,2,4,5,6} },
                            insen =      { {0,1,5,7,10},        {0,-1,0,0,-1},          {1,2,4,5,7} },
                            hungarMinor= { {0,2,3,6,7,8,11},    {0,0,-1,1,0,-1,0},      {1,2,3,4,5,6,7} },
                            bebopDom =   { {0,2,4,5,7,9,10,11}, {0,0,0,0,0,0,-1,0},     {1,2,3,4,5,6,7,7} },
                            bebopMaj =   { {0,2,4,5,7,8,9,11},  {0,0,0,0,0,1,0,0},      {1,2,3,4,5,5,6,7} };
        
         
        
    };
    
    
    namespace Chords{
        
        inline const Chord  majChord =          { {0,4,7},      {0,0,0},        {1,3,5} },
                            minChord =          { {0,3,7},      {0,1,0},        {1,3,5} },
                            augChord =          { {0,4,8},      {0,0,-1},       {1,3,5} },
                            dimChord =          { {0,3,6},      {0,1,1},        {1,3,5} },
                            seventhChord =      { {0,4,7,10},   {0,0,0,-1},     {1,3,5,7} },
                            min7Chord =         { {0,3,7,10},   {0,-1,0,-1},    {1,3,5,7} },
                            min7b5Chord =       { {0,3,6,10},   {0,-1,-1,-1},   {1,3,5,7} },
                            dim7Chord =         { {0,3,6,9},    {0,-1,-1,-1},   {1,3,5,7} },
                            maj7Chord =         { {0,4,7,11},   {0,0,0},        {1,3,5,7} },
                            minMaj7Chord =      { {0,3,7,11},   {0,-1,0,0},     {1,3,5,7} },
                            ninthChord =        { {0,2,4,7,10}, {0,0,0,-1,0},   {1,3,5,7,2} },
                            min9Chord =         { {0,2,3,7,10}, {0,-1,0,-1,0},  {1,3,5,7,2} },
                            min7b9Chord=        { {0,1,3,7,10}, {0,-1,0,-1,-1}, {1,3,5,7,2} },
                            maj9Chord =         { {0,2,4,7,11}, {0,0,0,0,0},    {1,3,5,7,2} },
                            add9Chord =         { {0,2,4,7},    {0,0,0,0},      {1,3,5,} },
                            minAdd9Chord =      { {0,2,3,7},    {0,-1,0,0},     {1,3,5,} },
                            sevenSharp9Chord=   { {0,3,4,7,10}, {0,0,0,-1,1},   {1,3,5,7,} },
                            min7Flat5Flat9Chord={ {0,1,3,6,10}, {0,-1,-1,-1,-1},{1,3,5,7,} },
                            sus2Chord =         { {0,2,7},      {0,0,0},        {1,2,} },
                            sus4Chord =         { {0,5,7},      {0,0,0},        {1,4,} },
                            powerChord =        { {0,7},        {0,0},          {1,5} },
                            sixthChord =        { {0,4,7,9},    {0,0,0,0},      {1,3,5,6} },
                            minSixthChord =     { {0,3,7,8},    {0,0,0,-1},     {1,3,5,6} },
                            sixNineChord =      { {0,2,4,7,9},  {0,0,0,0,0},    {1,3,5,6,2} },
                            eleventhChord =     { {0,4,5,7,10}, {0,0,0,-1,0},   {1,3,5,7,4} },
                            sharp11Chord =      { {0,4,6,7,10}, {0,0,0,-1,1},   {1,3,5,7,4} },
                            minSharp11Chord =   { {0,3,6,7,10}, {0,-1,0,-1,1},  {1,3,5,7,4} },
                            thirteenthChord =   { {0,4,7,9,10}, {0,0,0,-1,0},   {1,3,5,7,6} },
                            min13thChord =      { {0,3,7,8,10}, {0,-1,0,-1,-1}, {1,3,5,7,6} },
                            maj13thChord =      { {0,4,7,9,11}, {0,0,0,0,0},    {1,3,5,7,6} },
                            aug7thChord =       { {0,4,8,10},   {0,0,1,0},      {1,3,5,7} },
                            petrushka=          {{0,1,4,6,7,10},{0,0,0,1,1,1},  {1,3,5,4,6,} },
                            farben =            { {0,4,8,9,11}, {0,1,0,0,0},    {1,5,7,3,6} };
         
    };
    
    
    
    inline const String setClassToPitchName[12] = {"C","C#/Db","D","D#/Eb","E","F","F#/Gb","G","G#/Ab","A","A#/Bb", "B"};
    
    inline const String rootNames[] = {"C","C#", "Db","D","D#","Eb","E","F","F#","Gb","G","G#","Ab","A","A#","Bb", "B"};
    
    inline const std::map<String, int> rootNameMap = {
        {"C", 0},
        {"C#", 1},
        {"Db",1},
        {"D",2},
        {"D#", 3},
        {"Eb", 3},
        {"E", 4},
        {"F", 5},
        {"F#", 6},
        {"Gb", 6},
        {"G",7},
        {"G#",8},
        {"Ab",8},
        {"A",9},
        {"A#",10},
        {"Bb",10},
        {"B", 11}
    };

    inline const std::map<String, Mode> modeMap = {
        //Scales
        {"Major", Scales::major},
        {"Natural Minor", Scales::natMinor},
        {"Harmonic Minor", Scales::harmMinor},
        {"Melodic Minor", Scales::melMinor},
        {"Locrian", Scales::locrian},
        {"Dorian", Scales::dorian},
        {"Phrygian", Scales::phrygian},
        {"Lydian", Scales::lydian},
        {"Mixolydian", Scales::mixolydian},
        {"WholeTone", Scales::wholeTone},
        {"Pentatonic Major", Scales::pentaMaj},
        {"Pentatonic Minor", Scales::pentaMin},
        {"Pentatonic Maj Blues", Scales::pentaMajB},
        {"Pentatonic Min Blues", Scales::pentaMinB},
        {"Diminished Whole-Half", Scales::dimWholeHalf},
        {"Diminished Half-Whole", Scales::dimHalfWhole},
        {"Hangarian Minor", Scales::hungarMinor},
        {"Phrygian Dominant", Scales::phrygDom},
        {"Lydian Dominant",Scales::lydDom},
        {"Yo (Japanese)", Scales::yo},
        {"Insen (Japanese)", Scales::insen},
        {"Bebop Dominant", Scales::bebopDom},
        {"Bebop Major", Scales::bebopMaj},
        //Chords
        {"Maj (Chord)", Chords::majChord},
        {"min (Chord)", Chords::minChord},
        {"Aug (Chord)", Chords::augChord},
        {"dim (Chord)", Chords::dimChord},
        {"7th (Chord)", Chords::seventhChord},
        {"min7 (Chord)", Chords::min7Chord},
        {"m7b5 (Chord)", Chords::min7b5Chord},
        {"dim7 (Chord)", Chords::dim7Chord},
        {"Maj7 (Chord)", Chords::maj7Chord},
        {"minMaj7 (Chord)", Chords::minMaj7Chord},
        {"9th (Chord)", Chords::ninthChord},
        {"m9 (Chord)", Chords::min9Chord},
        {"m7b9 (Chord)", Chords::min7b9Chord},
        {"Maj9 (Chord)", Chords::maj9Chord},
        {"add9 (Chord)", Chords::add9Chord},
        {"minAdd9 (Chord)", Chords::minAdd9Chord},
        {"7#9 (Chord)", Chords::sevenSharp9Chord},
        {"sus2 (Chord)", Chords::sus2Chord},
        {"sus4 (Chord)", Chords::sus4Chord},
        {"Power Chord", Chords::powerChord},
        {"6th (Chord)", Chords::sixthChord},
        {"m6 (Chord)", Chords::minSixthChord},
        {"6/9 (Chord)", Chords::sixNineChord},
        {"13th (Chord)", Chords::thirteenthChord},
        {"m13 (Chord)", Chords::min13thChord},
        {"Maj13 (Chord)", Chords::maj13thChord},
        {"11th (Chord)", Chords::eleventhChord},
        {"#11 (Chord)", Chords::sharp11Chord},
        {"m#11 (Chord)", Chords::minSharp11Chord},
        {"m7b5b9 (Chord)", Chords::min7Flat5Flat9Chord},
        {"aug7 (Chord)", Chords::aug7thChord},
        {"Petrushka (Chord)", Chords::petrushka},
        {"Farben (Chord)", Chords::farben}
         

    }; //TODO, change everything to scales

    
    const std::map<int, Array<String>> diatonicNoteNames = {
        {0,{"Cbb","Cb","C", "C#","Cx"}},
        {1,{"Dbb","Db","D","D#","Dx"}},
        {2,{"Ebb","Eb","E", "E#","Ex"}},
        {3,{"Fbb","Fb","F", "F#","Fx"}},
        {4,{"Gbb","Gb","G", "G#","Gx"}},
        {5,{"Abb","Ab","A", "A#","Ax"}},
        {6,{"Bbb","Bb","B", "B#","Bx"}}
    };

        
    inline std::map<String, Array<int>> circleOfFifths = {
        //0 is natural, numbers show how many semitone sharp or flat.
        {"C",{0,0,0,0,0,0,0}},
        {"C#",{1,1,1,1,1,1,1}},
        {"D",{1,0,0,1,0,0,0}},
        {"D#",{1,1,0,1,0,0,0}},
        {"E",{1,0,0,1,1,0,0}},
        {"F",{0,0,0,0,0,0,-1}},
        {"F#",{1,1,1,1,1,1,0}},
        {"G",{0,0,0,1,0,0,0}},
        {"G#",{1,1,1,2,1,1,1}},
        {"A",{1,1,0,1,1,0,0}},
        {"A#",{2,2,1,2,2,1,1}},
        {"B",{1,1,0,1,1,1,0}},
        {"Db",{0,-1,-1,0,-1,-1,-1}},
        {"Eb",{0,0,-1,0,0,-1,-1}},
        {"Gb",{-1,-1,-1,0,0,-1,-1}},
        {"Ab",{0,-1,-1,0,0,-1,-1}},
        {"Bb",{0,0,-1,0,0,0,-1}},
    };
    
    inline std::map<int, int> setClassToDiatonic{
        {0,0},
        {1,0},
        {2,1},
        {3,1},
        {4,2},
        {5,3},
        {6,3},
        {7,4},
        {8,4},
        {9,5},
        {10,5},
        {11,6}
    };
    
    inline std::pair<int,int> noteNameToDiatonicValue(String noteName){
        int diatonicNoteVal;
        int diatonicModVal;
        
        std::for_each(Theory::diatonicNoteNames.begin(),
                      Theory::diatonicNoteNames.end(), [&](std::pair<int, Array<String>> diatonicNote){
            Array<String> noteNames = diatonicNote.second;
            if (noteNames.contains(noteName)){
                diatonicNoteVal = diatonicNote.first;
                diatonicModVal = noteNames.indexOf(noteName);
            }
        });
        
        return {diatonicNoteVal, diatonicModVal};
    }
    
    inline Accidental diatonicMatrixToAccidentals(int val){
        switch(val){
            case -1: return NO_PREFERENCE;
            case 0 : return DOUBLE_FLAT;
            case 1 : return FLAT;
            case 2 : return NATURAL;
            case 3 : return SHARP;
            case 4 : return DOUBLE_SHARP;
            default: DBG("Unrecognized Accidental"); return NATURAL;
        }
    }
};







#endif /* MUSIC_THEORY_HEADER */



