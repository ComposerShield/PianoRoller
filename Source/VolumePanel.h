/*
  ==============================================================================

    VolumePanel.h
    Created: 2 Jan 2019 12:47:35pm
    Author:  Adam Shield

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PianoRollComponent.h"



class VolumePanel : public Component,
                    public PianoRollComponent
{
public:
    VolumePanel();
    ~VolumePanel();
    
    void paint (Graphics&) override;
    void drawVolumes (PaintData p, bool isMono);
    void resized() override;

    void mouseDown(const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;

    
private:
    
    
    
    
    
};
