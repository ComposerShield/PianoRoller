
/*
 ==============================================================================
 
 PianoRoll.cpp
 Created: 29 Dec 2018 10:30:33pm
 Author:  Adam Shield
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"
#include "PianoRoll.h"

//==============================================================================



PianoRoll::PianoRoll(Staff& auditionStaff, PianoKeys& pianoKeys) : pianoKeys(pianoKeys),
                                                                   auditionStaff(auditionStaff)
{
    topNote = 84;
    canScroll = true;
    isChildOfBeatCanvas = false;
    isDoubleClick = false;
    setOpaque(true);
    
    if (! sender.connect ("127.0.0.1", 9001))
        showConnectionErrorMessage ("Error: could not connect to UDP port 9001.");

    setSize(getWidth(), getHeight());

}

PianoRoll::~PianoRoll(){
}

void PianoRoll::paint (Graphics& g)
{
    const int numOfBeats = presets[currentPreset]->numOfBeats;
    const int rootRow = topNote-65;
    const float width = getWidth();
    const float height = getHeight();
    const float noteWidth = (width / (static_cast<float>(numOfBeats) * 4.0f) );
    const float noteHeight = ( height / static_cast<float>(numOfRows) );
    const float tripNoteWidth = width / (static_cast<float>(numOfBeats) * 3.0f);
    
    PaintData paintData{g, width, height, noteHeight, noteWidth, tripNoteWidth,
                        static_cast<float>(numOfBeats), static_cast<float>(rootRow), static_cast<float>(topNote)};
    
    g.fillAll (PianoRollerColours::beatCanvasJungleGreen); //BACKGROUND COLOR
    
    drawRows(paintData);
    
    drawNotes(paintData);
    
    drawRowLines(paintData);
}

constexpr void PianoRoll::drawRows(PaintData p){
    
    for(int row=0;row<numOfRows;row++){
        const float yPosition = 0. + (row * p.height/numOfRows);
        const int pitch = p.topNote-row;
        
        if (checkIfBlackKey(pitch)){
            p.g.setColour (PianoRollerColours::greyOff);
            p.g.fillRect(0.0f,yPosition,p.width, p.noteHeight);
        }
        
        if(row == p.rootRow && isChildOfBeatCanvas){
            p.g.setColour (Colours::white);
            p.g.setOpacity(0.5);
            p.g.fillRect(0.0f,yPosition, p.width, p.noteHeight);
            p.g.setOpacity(1.);
        }
    }

}

constexpr void PianoRoll::drawRowLines(PaintData p){
    p.g.setColour (Colours::black);
    for(int i=0;i<=numOfRows;i++){
        const float yPosition = 0. + (i*p.height/static_cast<float>(numOfRows));
        p.g.setColour(Colours::black);
        p.g.drawLine(0., yPosition, p.width, yPosition);
        if(i==numOfRows || i==0){ //Reasons to draw a thicker line.
            p.g.drawLine(0., yPosition, p.width, yPosition, 4);
        }
    }
    p.g.drawLine(p.width, 0.0f, p.width, p.height, 3); //Right side line.
}

void PianoRoll::drawNotes(PaintData p){
    auto thisTrack = presets[currentPreset]->tracks[currentTrack];
    for(int beat=0;beat<p.numOfBeats;beat++){
        const int beatSwitch = thisTrack->beatSwitch[beat];
        auto [subDivisions, thisNoteWidth] = [&](){
            switch(beatSwitch){
                case 0: return std::make_pair(4, p.noteWidth);
                case 1: return std::make_pair(3, p.tripNoteWidth);
                default: DBG("DrawNotes/subDivisions, thisNoteWidth: not a valid beatSwitch\n");
                    return std::make_pair(4, p.noteWidth);
            }
        }();
        
        for(int subDiv=0;subDiv<subDivisions;subDiv++){
            float col = beat * subDivisions + subDiv;
            
            (isMono()) ? monoNoteFill(p, col, beatSwitch, thisNoteWidth) : polyNoteFill(p, col, beatSwitch, thisNoteWidth);
            drawColumnLine(p, subDiv, col, thisNoteWidth);
        }
        
    }
}

constexpr void PianoRoll::monoNoteFill(PaintData p, const int col, const int beatSwitch, const float thisNoteWidth){
    auto& [pitch, vol, active] = getMonoNote(col, beatSwitch);
    if(active){ //If note is active.
        p.g.setColour (PianoRollerColours::whiteBlue);
        float x = ( col * thisNoteWidth );
        float y = ( ((float)topNote-(float)pitch)/(float)numOfRows * p.height );
        p.g.fillRect(x, y, thisNoteWidth, p.noteHeight);
    }
}

constexpr void PianoRoll::polyNoteFill(PaintData p, const int col, const int beatSwitch, const float thisNoteWidth){
    //TODO******
    auto& [pitches, vol] = getPolyNote(col, beatSwitch);
    
    for(int polyNote=0; polyNote<pitches.size(); polyNote++){
        int pitch = pitches[polyNote];
        p.g.setColour (PianoRollerColours::whiteBlue);
        float x = ( col * thisNoteWidth );
        float y = ( ((float)topNote-(float)pitch)/(float)numOfRows * p.height );
        p.g.fillRect(x, y, thisNoteWidth, p.noteHeight);
    }
}



void PianoRoll::resized()
{
    
}


void PianoRoll::mouseEnter(const MouseEvent& event){

}

void PianoRoll::mouseExit(const MouseEvent& event){

}

void PianoRoll::mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel){
    float verticalAdjust = wheel.deltaY * 50.0f;
    verticalAdjust = verticalAdjust < 0 ? floor(verticalAdjust) : ceil(verticalAdjust);
    
    topNote = limitRange( ( topNote + static_cast<int>(verticalAdjust) ), 8+numOfRows, 127);
    repaint();
}

void PianoRoll::mouseUp(const MouseEvent& event){
    BeatCanvasOSC_MessageOut("/BeatCanvas/reloadTrack",currentTrack);
}

void PianoRoll::mouseDrag(const MouseEvent& event){
    mouseDown(event);
}

void PianoRoll::mouseDoubleClick(const juce::MouseEvent &event){
    isDoubleClick = true;
    mouseDown(event);
}

void PianoRoll::mouseDown(const MouseEvent& event){
    auto thisTrack = presets[currentPreset]->tracks[currentTrack];
    auto [x, y]= std::make_pair(getMouseXYRelative().getX(), getMouseXYRelative().getY());
    
    const int numOfBeats = presets[currentPreset]->numOfBeats;
    const int row =     static_cast<int>( (y/(float)getHeight()* (float) (numOfRows))    ); //Final cast rounds it down.
    const int col =     static_cast<int>( (x/(float)getWidth() * (float) (numOfBeats*4)) ); //Final cast rounds it down.
    const int tripCol = static_cast<int>( (x/(float)getWidth() * (float) (numOfBeats*3)) ); //Final cast rounds it down.
    const int currentBeat = col / 4;
    const int beatSwitch = thisTrack->beatSwitch[currentBeat];
    const int pitch = topNote - row;
    const int diatonicNoteValue = Theory::setClassToDiatonic[pitch%12];
    int thisCol = (beatSwitch==0) ? col : tripCol;

    if(pitch<128 && pitch>8){
        //Scroll up and down
        if     (pitch>topNote)          {topNote = pitch;           auditionStaff.repaint();}
        else if(pitch<topNote-numOfRows){topNote = pitch+numOfRows; auditionStaff.repaint();}
        
        isMono() ? monoWriteNote(thisCol, pitch, beatSwitch, event)
                 : polyWriteNote(thisCol, pitch, beatSwitch, event);
        
        prevPitch = pitch;
    }
    
    auditionStaff.notes.clear();
    auditionStaff.notes.push_back(NoteHead(pitch, diatonicNoteValue, -1, false));
    auditionStaff.clef = clefDisplay(pitch);
    pianoKeys.selectedKey = (getClicks(event, isDoubleClick).second) ? 0 : pitch;
    
    isDoubleClick=false;
    pianoKeys.repaint();
    repaint();
}

constexpr void PianoRoll::monoWriteNote(const int thisCol, const int pitch, const int beatSwitch, const MouseEvent& event){
    const auto [leftClick, rightClick] = getClicks(event, isDoubleClick);
    const int beatDiv = beatSwitchToDiv(beatSwitch);

    if(!rightClick){
        updateNote(thisCol, pitch, beatSwitch);
        noteName->setValue(Theory::setClassToPitchName[pitch%12]);
        *midiNoteNum = pitch;
        
        //========Send to BeatCanvasJava.Java=======
        //public void setPitch(int track, int div, int note, int pitch)
        BeatCanvasOSC_MessageOut("/BeatCanvas/setPitch",currentTrack, beatDiv, thisCol, pitch);
        BeatCanvasOSC_MessageOut("/BeatCanvas/noteOnOff",currentTrack, beatDiv, thisCol, 1);
    }
    else if(rightClick &&
            ( (pitch == prevPitch) || (pitch == prevPitch+1) || (pitch == prevPitch-1) ||
             (pitch == prevPitch+2) || (pitch == prevPitch-2) )
            )
    {
        updateNote(thisCol, prevPitch, beatSwitch, false);
        
        //========Send to BeatCanvasJava.Java=======
        //public void noteOnOff(int track, int div, int note, int onOff)
        BeatCanvasOSC_MessageOut("/BeatCanvas/noteOnOff",currentTrack, beatDiv, thisCol, 0);
    }
}

constexpr void PianoRoll::polyWriteNote(const int thisCol, const int pitch,
                              const int beatSwitch, const MouseEvent& event){
    const auto [leftClick, rightClick] = getClicks(event, isDoubleClick);
    const bool isDragging = event.mouseWasDraggedSinceMouseDown();
    
    if(leftClick && !rightClick){
        if(!isDragging){
            polySelectedNote = {thisCol, pitch};
        }else{ //isDragging
            auto [prevCol, prevVol] = polySelectedNote;
            updateNote(prevCol, prevPitch, beatSwitch, false); //Remove previous note (move it to new pitch)
            polySelectedNote = {thisCol, pitch};
        }
        updateNote(thisCol, pitch, beatSwitch, true);
    }else{ //rightClick
        updateNote(thisCol, pitch, beatSwitch, false);
    }
    noteName->setValue(Theory::setClassToPitchName[pitch%12]);
}

void PianoRoll::spacebar(){
    BeatCanvasOSC_MessageOut("/BeatCanvas/spacebar");
}

void PianoRoll::changeBeatCanvasPreset(const int preset){
    BeatCanvasOSC_MessageOut("/BeatCanvas/updatePreset", preset);
}

void PianoRoll::changeBeatCanvasTrack(const int track){
    BeatCanvasOSC_MessageOut("/BeatCanvas/updateTrack", track);
}

void PianoRoll::changeBeatCanvasBeats(const int beats){
    BeatCanvasOSC_MessageOut("/BeatCanvas/updateBeat", beats);
}

void PianoRoll::changeBeatCanvasTriplet(const int beat, const int val){
    //Java code:
    //changeRhythmDiv(int track, int beatMinusOne, int divSwitch)
    BeatCanvasOSC_MessageOut("/BeatCanvas/changeRhythmDiv", currentTrack, beat, val);
}

constexpr Clef PianoRoll::clefDisplay(int pitch){
    if      (inclusiveRange(pitch, 60, 84)) return TREBLE;
    else if (inclusiveRange(pitch, 85, 96)) return TREBLE_8VA;
    else if (inclusiveRange(pitch, 85, 127)) return TREBLE_15MA;
    else if (inclusiveRange(pitch, 36, 59)) return BASS;
    else if (inclusiveRange(pitch, 24, 35)) return BASS_8VA;
    else if (inclusiveRange(pitch, 1, 35)) return BASS_15MA;
    else return TREBLE;
}


//=============================================================================================================
////=========================================PIANO KEYS========================================================
//=============================================================================================================

PianoKeys::PianoKeys(PianoRoll& pianoRollInput, Staff& auditionStaff) : pianoRoll(pianoRollInput),
                                                                        auditionStaff(auditionStaff)
{

}


void PianoKeys::paint(juce::Graphics &g){
    const float topNote = pianoRoll.topNote;
    const float width = getWidth();
    const float height = getHeight();
    const float noteHeight = ( height / (float)numOfRows );
    
    g.fillAll (PianoRollerColours::beatCanvasJungleGreen); //BACKGROUND COLOR
    
    PaintData paintData{g, width, height, noteHeight, 0.0f, 0.0f, 0, 0, topNote};
    drawRows(paintData);

    g.drawRoundedRectangle(0.0f, 0.0f, width, height, 0.0f, 4.0f);
    
}

void PianoKeys::drawRows(PaintData p){
    for(int row=0; row<numOfRows; row++){
        const int pitch = p.topNote-row;
        const float yPosition = row * p.noteHeight;
        const bool isSelectedKey = selectedKey==pitch;
        
        if (checkIfBlackKey(pitch)){
            p.g.setColour ( (isSelectedKey) ? PianoRollerColours::whiteBlue : Colours::darkgrey );
            p.g.fillRect(0.0f,yPosition,p.width*0.666, p.noteHeight);
            p.g.setColour (Colours::black);
            p.g.drawRect(0.0f,yPosition,p.width*0.66, p.noteHeight);
            
        }else{ //is a White Key
            p.g.setColour ((isSelectedKey) ? PianoRollerColours::whiteBlue : PianoRollerColours::beatCanvasJungleGreen );
            p.g.fillRect(0.0f,yPosition,p.width, p.noteHeight);
            
            p.g.setColour (Colours::black);
            if(pitch%12==4 || pitch%12==11){ //Note E or B
                p.g.drawLine(0.0f, yPosition, p.width, yPosition);
            }else{
                p.g.drawLine(p.width*0.66f, yPosition-(p.noteHeight*0.5f), p.width-0.66f, yPosition-(p.noteHeight*0.5f));
            }
        }
    }
}

void PianoKeys::mouseUp(const juce::MouseEvent &event){
    
}

void PianoKeys::mouseDown(const juce::MouseEvent &event){
    
    
}

void PianoKeys::mouseDrag(const juce::MouseEvent &event){
    
}

