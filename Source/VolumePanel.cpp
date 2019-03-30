/*
  ==============================================================================

    VolumePanel.cpp
    Created: 2 Jan 2019 12:47:35pm
    Author:  Adam Shield

  ==============================================================================
*/

#include "VolumePanel.h"


VolumePanel::VolumePanel(OwnedArray<Preset> * processorPresetLocation){
    processorPresets = processorPresetLocation;
    setOpaque(true);
    
    if (! sender.connect ("127.0.0.1", 9001)) // [4]
        showConnectionErrorMessage ("Error: could not connect to UDP port 9001.");
    
    setVisible(true);
    setSize(getWidth(), getHeight());
    
}

VolumePanel::~VolumePanel(){

}

void VolumePanel::paint (Graphics& g)
{
    const int numOfBeats = presets[currentPreset]->numOfBeats;
    const float width = getWidth();
    const float height = getHeight();
    const float noteWidth = (width / ((float)numOfBeats * 4.0f) );
    const float tripNoteWidth = width / ((float)numOfBeats * 3.0f);
    
    PaintData paintData{g, width, height, 0, noteWidth, tripNoteWidth, (float)numOfBeats, 0, 0.0f};
    
    g.fillAll (PianoRollerColours::greyOff); //BACKGROUND COLOR
    
    drawVolumes(paintData, isMono());
    
    g.drawLine(width, 0.0f, width, height, 3); //Right side line.
}

void VolumePanel::drawVolumes(PaintData p, bool mono){
    auto thisTrack = presets[currentPreset]->tracks[currentTrack];
    mono = presets[currentPreset]->isMono;
    
    for(int beat=0;beat<p.numOfBeats;beat++){
        const int currentBeatSwitch = thisTrack->beatSwitch[beat];
        auto [div, thisNoteWidth] = [currentBeatSwitch, p](){
            if(currentBeatSwitch==0) return std::make_pair(4, p.noteWidth);
            else                     return std::make_pair(3, p.tripNoteWidth);
        }();
        
        for(int subDiv=0;subDiv<div;subDiv++){
            const int col = (beat*div) + subDiv;
            auto& thisVol =
                (mono) ? getMonoNote(col, currentBeatSwitch).vol
                       : getPolyNote(col, currentBeatSwitch).vol;
            
            Point<float> volSlider{col * thisNoteWidth,
                                   p.height - ( p.height/127.f * (float)thisVol ) };
            
            p.g.setColour (PianoRollerColours::whiteBlue);
            p.g.fillRect(volSlider.getX(), volSlider.getY(), thisNoteWidth, p.height);
            
            drawColumnLine(p, subDiv, col, thisNoteWidth);
        }
    }
}

void VolumePanel::resized(){

}

void VolumePanel::mouseDrag(const MouseEvent& event){
    mouseDown(event);
}

void VolumePanel::mouseDown(const MouseEvent &event){
    const auto [leftClick, rightClick] = getClicks(event, false);
    
    const int numOfBeats = presets[currentPreset]->numOfBeats;
    const float x = getMouseXYRelative().getX();
    const float y = getMouseXYRelative().getY();
    const int vol = rightClick ? 96 
                  : midiLimit( (int)( 127.f - y/(float)getHeight() * 127.f ) ); //Final (int) cast rounds it down.
    const int col = midiLimit( (int)(x/(float)getWidth() * (float) (numOfBeats*4)) ); //Final (int) cast rounds it down.
    const int tripCol = (int) (x/(float)getWidth() * (float) (numOfBeats*3)); //Final (int) cast rounds it down.
    const int currentBeat = col / 4;
    const int beatSwitch = presets[currentPreset]->tracks[currentTrack]->beatSwitch[currentBeat];
    auto [beatDiv, thisCol] = [&](){
        if(beatSwitch==0) return std::make_pair(4, col);
        else              return std::make_pair(3, tripCol);
    }();
    
    if(isMono()){
        auto& [thisPitch, thisVol, active] =
            (beatSwitch==0) ? getMonoNote(col, 0)
                            : getMonoNote(col, 1);
        thisVol = vol;
    }else{ //isPoly
        auto [pitches, polyVol] =
        (beatSwitch==0) ? getPolyNote(col, 0)
                        : getPolyNote(col, 1);
        polyVol = vol;
    }
    
    //========Send to BeatCanvasJava.Java=======
    //public void setVol(int track, int div, int note, int vol)
    BeatCanvasOSC_MessageOut("/BeatCanvas/setVol",currentTrack, beatDiv, thisCol, vol);
    
    repaint();
}


