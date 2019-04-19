/*
  ==============================================================================

    PianoRollComponent.cpp
    Created: 3 Jan 2019 6:19:18pm
    Author:  Adam Shield

  ==============================================================================
*/

#include "PianoRollComponent.h"

OwnedArray<PianoRollComponent::Preset> PianoRollComponent::presets = []()->OwnedArray<PianoRollComponent::Preset>{
    OwnedArray<Preset> output;
    for(int preset=0;preset<=PianoRollComponent::numOfPresets;preset++)
        output.add(new Preset);
    return output;
}();

bool PianoRollComponent::isChildOfBeatCanvas = false;
bool PianoRollComponent::isDoubleClick = false;

constexpr Note& PianoRollComponent::getMonoNote(const int col, const int beatSwitch) const{
    switch(beatSwitch){
        case 0: return presets[currentPreset]->tracks[currentTrack]->sixteenthNotes.getReference(col); break;
        case 1: return presets[currentPreset]->tracks[currentTrack]->tripletNotes.getReference(col); break;
        default: return getMonoNote(0, 0);
    }
}

constexpr PolyNote& PianoRollComponent::getPolyNote(const int col, const int beatSwitch) const{
    switch(beatSwitch){
        case 0: return {presets[currentPreset]->tracks[currentTrack]->polySixteenths.getReference(col)}; break;
        case 1: return {presets[currentPreset]->tracks[currentTrack]->polyTriplets.getReference(col)}; break;
        default : return getPolyNote(0, 0);
    }
}

void PianoRollComponent::updateNote(const int col, const int pitch, const int beatSwitch){
    updateNote(col, pitch, beatSwitch, true);
}

void PianoRollComponent::updateNote(const int col, const int pitch, const int beatSwitch, const bool isActive){
    if (isMono()){
        auto& [thisPitch, thisVol, active] = getMonoNote(col, beatSwitch);
        
        thisPitch = pitch;
        active = isActive;
    }else{ //isPoly

        auto& [pitches, vol] = getPolyNote(col, beatSwitch);
        if (isActive){
            if(pitches.size()<=12)
                pitches.addIfNotAlreadyThere(pitch);
        }else{
            pitches.removeAllInstancesOf(pitch);
        }
    }
}

void PianoRollComponent::updateVolume(const int col, const int vol, const int beatSwitch){
    auto& [thisPitch, thisVol, active] = getMonoNote(col, beatSwitch);
    thisVol = vol;
}

void PianoRollComponent::updateBeatSwitch(const int beat, const int switchVal){
    presets[currentPreset]->tracks[currentTrack]->beatSwitch.set(beat,switchVal);
}

void PianoRollComponent::updateNumOfBeats(const int beats){
    updateNumOfBeats(beats, currentPreset);
}

void PianoRollComponent::updateNumOfBeats(const int beats, const int preset){
    presets[currentPreset]->numOfBeats = beats;
}

void PianoRollComponent::changeRhythmDiv(const int track, const int beat, const int beatSwitch){
    presets[currentPreset]->tracks[track]->beatSwitch.set(beat, beatSwitch);
}

void PianoRollComponent::updatePreset(const int preset){
    currentPreset = preset;
}

void PianoRollComponent::updateTrack(const int track){
    currentTrack = track;
}

void PianoRollComponent::noteOnOff(const int track, const int div, const int note, const int onOff){
    int beatSwitch = (div==4) ? 0 : 1;
    auto& [thisPitch, thisVol, active] = getMonoNote(note, beatSwitch);
    active = (onOff) ? true : false;
}

void PianoRollComponent::copyPreset(const int presetSource,const  int presetReplaced){
    presets.set(presetReplaced, presets[presetSource]);
}

//From Java
//public void setPitch(int track, int div, int note, int pitch, int preset)
void PianoRollComponent::setPitch(const int track, const int div, const int note, const int pitch, const int preset){
    auto& [thisPitch, thisVol, active] = getMonoNote(note, divToBeatSwitch(div));
    thisPitch = pitch;
}

void PianoRollComponent::drawColumnLine(const PaintData p, const int subDiv, const int col, const float noteWidth) const{
    const int lineWidth = (subDiv==0) ? 3 : 1;
    const float xPosition = 0.0f + ( static_cast<float>(col)*noteWidth );
    
    p.g.setColour(Colours::black);
    p.g.drawLine(xPosition, 0., xPosition, p.height, lineWidth);
}

int PianoRollComponent::divToBeatSwitch(int div) const{
    switch(div){
        case 4: return 0;
        case 3: return 1;
        default: jassertfalse; return -1;
    }
}

int PianoRollComponent::beatSwitchToDiv(int beatSwitch) const{
    switch(beatSwitch){
        case 0: return 4;
        case 1: return 3;
        default: jassertfalse; return -1;
    }
}

bool PianoRollComponent::isMono() const{
    return presets[currentPreset]->isMono;
}

std::pair<bool, bool> PianoRollComponent::getClicks(MouseEvent event, bool isDoubleClick) const{
    const bool isMac = (SystemStats::getOperatingSystemType() & juce::SystemStats::MacOSX) != 0;
    return std::make_pair(event.mods.isLeftButtonDown(),
                          event.mods.isRightButtonDown() ||
                          isDoubleClick                  ||
                          (isMac ? event.mods.isCommandDown()
                                 : event.mods.isCtrlDown()));

}

