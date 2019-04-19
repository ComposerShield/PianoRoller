/*
  ==============================================================================

    OSC_Out.h
    Created: 9 Apr 2019 11:17:03am
    Author:  Adam Shield

  ==============================================================================
*/

#pragma once
#include "PianoRollComponent.h"

class OSC_Interpreter : public PianoRollComponent{
public:
    inline constexpr void OSC_Interpret(const juce::OSCMessage &Message){
        //"Function" calls.
        if (Message.size() > 0 && Message[0].isString()){
            
            //setPreset(int preset)
            if(Message[0].getString() == "setPreset"){
                int preset = Message[1].getInt32();
                updatePreset(preset);
            }
            
            //updateNote(int col, int pitch, int beatSwitch)
            else if(Message[0].getString() == "updateNote"){
                int col = Message[1].getInt32();
                int pitch = Message[2].getInt32();
                int beatSwitch = Message[3].getInt32();
                updateNote(col, pitch, beatSwitch);
            }
            
            //void PianoRoll::updateBeatSwitch(int beat)
            else if(Message[0].getString() == "updateBeatSwitch"){
                
            }
            
            else if(Message[0].getString() == "updateVolume"){
                int col = Message[1].getInt32();
                int vol = Message[2].getInt32();
                int beatSwitch = Message[3].getInt32();
                updateVolume(col, vol, beatSwitch);
            }
            
            else if(Message[0].getString() == "updateNumOfBeats"){
                int beats = Message[1].getInt32();
                int preset = [&](){
                    return (Message.size() == 3) ? Message[2].getInt32() : currentPreset;
                }();
                updateNumOfBeats(beats, preset);
            }
            
            
            else if(Message[0].getString() == "changeRhythmDiv"){
                int track = Message[1].getInt32();
                int beat = Message[2].getInt32();
                int beatSwitch = Message[3].getInt32();
                changeRhythmDiv(track, beat, beatSwitch);
            }
            
            else if(Message[0].getString() == "currentPreset"){
                int preset = Message[1].getInt32();
                updatePreset(preset);
            }
            else if(Message[0].getString() == "currentTrack"){
                int track = Message[1].getInt32();
                updateTrack(track);
                
            }
            
            //void PianoRollComponent::noteOnOff(int track, int div, int note, int onOff)
            else if(Message[0].getString() == "noteOnOff"){
                int track = Message[1].getInt32();
                int div = Message[2].getInt32();
                int note = Message[3].getInt32();
                int onOff = Message[4].getInt32();
                noteOnOff(track, div, note, onOff);
            }
            
            //void PianoRollComponent::copyPreset(int presetSource, int presetReplaced)
            else if(Message[0].getString() == "copyPresets"){
                int presetSource = Message[1].getInt32();
                int presetReplaced = Message[2].getInt32();
                copyPreset(presetSource, presetReplaced);
            }
            
            //setPitch(const int track, const int div, const int note, const int pitch, const int preset)
            else if(Message[0].getString() == "setPitch"){
                int track = Message[1].getInt32();
                int div = Message[2].getInt32();
                int note = Message[3].getInt32();
                int pitch = Message[4].getInt32();
                int preset = Message[5].getInt32();
                setPitch(track, div, note, pitch, preset);
            }
            
        }
    }
};
