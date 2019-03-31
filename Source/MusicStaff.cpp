/*
  ==============================================================================

    MusicStaff.cpp
    Created: 31 Mar 2019 2:03:25pm
    Author:  Adam Shield

  ==============================================================================
*/

#include "MusicStaff.h"

Staff::Staff(OwnedArray<Preset> * processorPresetLocation, int * currentPresetPointer){
    processorPresets = processorPresetLocation;
    currentPreset = currentPresetPointer;
}



void Staff::paint(juce::Graphics &g){
    const bool isTreble = (clef==TREBLE || clef==TREBLE_8VA || clef==TREBLE_15MA) ? true : false;
    const float numOfLines = 10.0f;
    const float height = getHeight();
    const float width = getWidth();
    const float lineSpacing = height/numOfLines;
    const float accidentalSpacing = height*0.15;
    const float clefSpacing = height * 0.45;
    const float noteSpacing = clefSpacing * 0.5f;
    const float clefHeight = [&]()->float{
        if (isTreble) return height*0.11f;
        else return height * -0.19f;
    }();
    const float noteHeight = height*0.1;
    const float noteWidth = noteHeight*1.5f;
    const String clefText = [&]()->String{
        if (isTreble) return "&";
        else return "?";
    }();
    const uint8 bottomNote = [&]()->uint8{
        uint8 returnVal;
        if(isTreble) returnVal = 60;
        else         returnVal = 0;
        
        return returnVal;
    }();
    
    const String modeName = (*processorPresets)[*currentPreset]->currentMode;
    const int root = (*processorPresets)[*currentPreset]->root;
    Theory::Mode mode = Theory::modeMap.at(modeName);
    auto [modeNotes, enharmIndex, intervals] = mode;
    
    drawClef(g, width, height, clefHeight, clefText);
    
    drawStaffLines(g, width, lineSpacing);
    
    //=================
    //====DRAW NOTEs===
    //=================
    int prevDiatonicNoteVal = -1;
    for_indexed(auto& [pitch, noteName, diatonicNoteValue, diatonicModValue, accidental] : notes){
        
        const int pitchSetClass = ((int)pitch) % 12;
        const Array<int> fooo = enharmIndex;
        const float xPos = clefSpacing + (clefSpacing * i * 0.5);
        const float yPos = height - ( (lineSpacing/2) * (diatonicNoteValue+3) );
        const float ledgerLineX = xPos - noteWidth/2;
        const float ledgerLineY = yPos+(lineSpacing/2);
        
        checkAccidental(accidental, modeNotes, enharmIndex, pitchSetClass);
        
        const auto [lowestCinStaffLineOffset, lowestCinStaffOctave] = [&](){
            switch(clef){
                case TREBLE: return std::pair<int, int>(3, 4);
                case BASS: return std::pair<int, int>(1, 2);
                case TREBLE_8VA: return std::pair<int, int>(3, 5);
                case TREBLE_15MA: return std::pair<int, int>(3, 6);
                case BASS_8VA: return std::pair<int, int>(1, 1);
                case BASS_15MA: return std::pair<int, int>(1, 0);
            }
        }();
        
        const String accidentalText = [&, accidental=accidental](){
            switch(accidental){
                case SHARP: return "#";
                case FLAT: return "b";
                case DOUBLE_SHARP: return "x";
                case DOUBLE_FLAT: return "bb";
                case NATURAL: return "";
                case COURTESY_NATURAL: DBG("dsgdfgsfh\negfergre\nsasd"); return "n";
            }
            return "";
        }();
        
        g.fillEllipse(xPos, yPos, noteWidth, noteHeight); //Draw note.
        
        g.setFont(height*0.6f);
        g.drawText(accidentalText, xPos-accidentalSpacing,yPos-(lineSpacing*0.6),noteWidth,noteHeight, Justification::right);
        
        //One ledger line below or above.
        if(diatonicNoteValue==0 || diatonicNoteValue==12) g.drawLine(ledgerLineX, ledgerLineY, ledgerLineX + noteWidth*2, ledgerLineY);
        //TODO: Two ledger lines below or above.
    }
}

void Staff::drawClef(Graphics &g, const float width, const float height, const float clefHeight, const String clefText){
    g.setFont(opusLookAndFeel.getOpus());
    g.setFont(height*0.76);
    g.drawText(clefText, 0.0f, clefHeight, width, height, Justification::left);
}

void Staff::drawStaffLines(Graphics &g, const int width, const int lineSpacing){
    g.setColour(Colours::black);
    for(int line=4; line<9;line++){
        float yPos = line*lineSpacing;
        g.drawLine(0.0f, yPos, width, yPos);
    }
}

void Staff::drawNotes(Graphics &g){
    
    
}

void Staff::checkAccidental(Accidental& accidental, const Array<int> modeNotes, const Array<int> enharmIndex, const int pitchSetClass){
    
    if (accidental == NO_PREFERENCE){
        
        accidental = [&](){
            if(modeNotes.contains(pitchSetClass)){
                switch (enharmIndex[modeNotes.indexOf(pitchSetClass)]){
                    case -1: return FLAT;
                    case 1 : return SHARP;
                    case -2: return DOUBLE_FLAT;
                    case 2 : return DOUBLE_SHARP;
                    case 0 : return NATURAL;
                }
            }
            
            return checkIfBlackKey(pitchSetClass) ? SHARP : NATURAL;
        }();
    }
}
