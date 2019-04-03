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
    
    void paint(Graphics&) override;
    void setPlayCursor(float val);
    float playCursorLine;
    AudioPlayHead::CurrentPositionInfo& currentPositionInfo;
    
    DrawableRectangle currentPositionMarker;
    
private:
    void timerCallback() override;
};


