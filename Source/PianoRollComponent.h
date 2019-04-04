/*
  ==============================================================================

    Setup.h
    Created: 2 Jan 2019 1:56:52pm
    Author:  Adam Shield

  ==============================================================================
*/

#pragma once


#include "../JuceLibraryCode/JuceHeader.h"
#include <map>

namespace PianoRollerColours {
    const inline Colour greyOff               = Colour(110,110,110);
    const inline Colour whiteBlue             = Colour(195,223,226);
    const inline Colour beatCanvasJungleGreen = Colour(152,152,115);
    const inline Colour calmGreen             = Colour(156,168,152);
}

struct Note{
    int pitch;
    int vol;
    bool active;
};

struct PolyNote{
    Array<int> pitches;
    int vol;
};

class PianoRollComponent
{
public:
    static constexpr int numOfTracks = 8;
    static constexpr int numOfPresets = 8;
    static constexpr int maxBeats = 16;
    static constexpr int numOfRows = 32;
    static constexpr int blackKeys[5] = {1,3,6,8,10};
    
    static inline int currentPreset = 1,
                      currentTrack  = 1;
    
    bool isDoubleClick;
    
    struct PaintData;
    
    class Track{
    public:
        Array<Note> sixteenthNotes;
        Array<Note> tripletNotes;
        Array<int> beatSwitch;
        Array<bool> sixteenthUserSelected;
        Array<bool> tripletUserSelected;
        Array<PolyNote> polySixteenths;
        Array<PolyNote> polyTriplets;
        int octaveShift;
        String selectedScale, selectedGenerator, selectedRoot, selectedArpDirection;
        
        Track(){
            octaveShift=0;
            selectedRoot = "C";
            selectedScale = "Major";
            selectedGenerator = "random";
            selectedArpDirection = "ascend";
            
            for(int note=0;note<maxBeats*4;note++){
                sixteenthNotes.add(Note{0,96,false});
                polySixteenths.add(PolyNote{{},96});
            }
            for(int note=0;note<maxBeats*3;note++){
                tripletNotes.add(Note{0,96,false});
                polyTriplets.add(PolyNote{{},96});
            }
            for(int beat=0;beat<maxBeats;beat++){
                beatSwitch.add(0);
            }
        }
    };
    
    class Preset{
        
    public:
        int numOfBeats;
        int root;
        bool isMono;
        String rootName;
        String currentMode;
        String generatorType;
        String arpType;
        Array<int> currentModeNotes;
        OwnedArray<Track> tracks;
        
        Preset(){
            numOfBeats = 4;
            isMono = true;
            currentModeNotes = {0,2,4,5,7,9,11}; //Major Scale
            rootName = "C";
            currentMode = "Major";
            generatorType = "random";
            root = 0;
            
            for(int track=0;track<=numOfTracks;track++){
                tracks.add(new Track());
            }
        }
    };
    

    ////THE ALMIGHTY PRESET ARRAY////
    static OwnedArray<Preset> presets;

    
    ////==============CONSTRUCTOR==============//
    PianoRollComponent(){
        currentPreset = 1;
        currentTrack = 1;
        
    }
    
    void updateNote(const int col, const int pitch, const int beatSwitch);
    void updateNote(const int col, const int pitch, const int beatSwitch, const bool isActive);
    void setPitch(int track, int div, int note, int pitch, int preset);
    void updateVolume(const int col, const int vol, const int beatSwitch);
    void updateBeatSwitch(int beat, int switchVal);
    void updateNumOfBeats(int beats);
    void updateNumOfBeats(int beats, const int preset);
    void changeRhythmDiv(int track, int beat, int beatSwitch);
    void updatePreset(const int preset);
    void updateTrack(const int track);
    void noteOnOff(const int track, const int div, const int note, const int onOff);
    void copyPreset(const int presetSource, const int presetReplaced);
    int midiLimit(int midiVal);
    int limitRange(int val, int low, int high);
    bool checkIfBlackKey(const int pitch);
    bool isMono();
    Note& getMonoNote(int col, int beatSwitch);
    PolyNote& getPolyNote(int col, int beatSwitch);
    int divToBeatSwitch(int div);
    int beatSwitchToDiv(int beatSwitch);
    std::pair<bool, bool> getClicks(MouseEvent event, bool isDoubleClick);
    
    template<typename T>
    constexpr bool inclusiveRange(T val, T lower, T upper){
        return (val >= lower && val <= upper);
    }
    
    template<typename T>
    constexpr bool exclusiveRange(T val, T lower, T upper){
        return (val > lower && val < upper);
    }
    
    //==================PAINTING FUNCTIONS==================
    void drawColumnLine(PaintData p, const int subDiv, const int col, const float noteWidth);
    
    
    //================BEAT CANVAS OSC MESSAGES================
    template<typename T, typename... Args>
    void BeatCanvasOSC_MessageOut(String initMessage, T t, Args... data){
        if (! sender.send (initMessage, data...))
            showConnectionErrorMessage ("Error: could not send OSC message.");
    }
    
    void BeatCanvasOSC_MessageOut(String message){
        if (! sender.send (message))
            showConnectionErrorMessage ("Error: could not send OSC message.");
    }
    
    OSCSender sender;
    
    void showConnectionErrorMessage (const String& messageText)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Connection error",
                                          messageText,
                                          "OK");
    }
    
    struct PaintData{
        Graphics& g;
        float width;
        float height;
        float noteHeight;
        float noteWidth;
        float tripNoteWidth;
        float numOfBeats;
        float rootRow;
        float topNote;
    };
    

    
private:
    
};













