/*
  ==============================================================================

    PlayCursorWindow.h
    Created: 3 Jan 2019 8:37:17pm
    Author:  Adam Shield

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PianoRollComponent.h"




class PlayCursorWindow : public Component,
                         public Timer
{
public:
    
    PlayCursorWindow(AudioPlayHead::CurrentPositionInfo& positionInfoLocation);
    

    void setPlayCursor(float val);
    
private:
    float playCursorLine;
    DrawableRectangle currentPositionMarker;
    AudioPlayHead::CurrentPositionInfo& currentPositionInfo;
    
    void timerCallback() override;
    void paint(Graphics&) override;
};


