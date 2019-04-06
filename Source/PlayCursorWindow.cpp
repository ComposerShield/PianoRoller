/*
  ==============================================================================

    PlayCursorWindow.cpp
    Created: 4 Jan 2019 2:15:49pm
    Author:  Adam Shield

  ==============================================================================
*/

#include "PlayCursorWindow.h"



PlayCursorWindow::PlayCursorWindow(AudioPlayHead::CurrentPositionInfo& positionInfoLocation) :
currentPositionInfo(positionInfoLocation)
{
    setInterceptsMouseClicks(false, false);
    playCursorLine = 0.0f;
    setAlwaysOnTop(true);
    
    if (!JUCEApplication::isStandaloneApp())
        startTimer(80);
    
    currentPositionMarker.setFill(Colours::yellow);
    addAndMakeVisible(&currentPositionMarker);
}

 void PlayCursorWindow::paint(Graphics& g){
     
 }

void PlayCursorWindow::setPlayCursor(float val){
    playCursorLine = val; //old version
}

void PlayCursorWindow::timerCallback(){
    bool isPlaying = currentPositionInfo.isPlaying;
    currentPositionMarker.setVisible(isPlaying);
    
    currentPositionMarker.setRectangle(Rectangle<float>(getWidth() * playCursorLine,
                                                        0.0f,
                                                        2.0f,
                                                        getHeight()));
}
