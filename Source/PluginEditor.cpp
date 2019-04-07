/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PianoRoll1AudioProcessorEditor::PianoRoll1AudioProcessorEditor (PianoRoll1AudioProcessor& p)
: AudioProcessorEditor (&p), processor (p), midiLabel("0"), pianoRoll(auditionStaff, pianoKeys), pianoKeys(pianoRoll, auditionStaff),
  playCursorWindow(processor.lastPosInfo)
{
    setVisible(true);
    setResizable(true, true);
    isChildOfBeatCanvas = false;
    setOpaque(true);
    
    currentBeat = 0.0f;
    currentNumOfBeats = 4;
    previousVal = -1.0f;
    beatIndex = 0;
    sixteenthCounter = 3;
    tripletCounter = 2;
    previousOrder = {0};
    arpeggioDirection = "Ascending";
    midiNoteNum = 0;
    
    playPosition = "";
    generateButton.setButtonText("Generate");
    
    //Setup Note Name in the top right corner of window==========================
    noteName = "";
    pianoRoll.noteName = &noteName; //Allow piano roll to edit this value.
    pianoRoll.midiNoteNum = &midiNoteNum; //Allow piano roll to edit this value.
    noteName.addListener(this);
    
    //Slider look and feels======================================================
    getLookAndFeel().setColour(Slider::thumbColourId, whiteBlue);
    getLookAndFeel().setColour(Slider::textBoxOutlineColourId, Colours::black);
    getLookAndFeel().setColour(Slider::textBoxTextColourId, Colours::black);
    getLookAndFeel().setColour(Slider::trackColourId, whiteBlue);
    getLookAndFeel().setColour(Slider::backgroundColourId, Colours::black);
    getLookAndFeel().setColour(TextButton::buttonColourId, PianoRollerColours::greyOff);
    getLookAndFeel().setColour(TextButton::textColourOffId, Colours::black);
    
    
    //Add and make visibles======================================================
    const Array<Component *> makeVisible{&pianoRoll, &pianoKeys, &auditionStaff, &scaleDisplayStaff,
                                         &volumePanel, &playCursorWindow, &midiLabel, &presetSlider,
                                         &trackSlider, &beatSlider, &rootMenu, &scaleMenu, &generateButton,
                                         &monoPolyMenu, &generatorMenu, &arpDirectionMenu, &arpSlider };
    for(auto& component : makeVisible) addAndMakeVisible(component);
    
    arpDirectionMenu.setVisible(false);

    
    //Setup Sliders===============================================================
    presetSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>  (processor.treeState,PRESET_ID,presetSlider);
    trackSliderAttach =  std::make_unique<AudioProcessorValueTreeState::SliderAttachment>  (processor.treeState,TRACK_ID,trackSlider);
    beatSliderAttach =   std::make_unique<AudioProcessorValueTreeState::SliderAttachment>  (processor.treeState,BEATS_ID,beatSlider);
    rootAttach =         std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.treeState,ROOT_ID,rootMenu);
    scaleAttach =        std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.treeState,SCALE_ID,scaleMenu);
    monoPolyAttach =     std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(processor.treeState,MONOPOLY_ID,monoPolyMenu);
    
    presetSlider.setRange(1,numOfPresets,(int)1);
    trackSlider.setRange(1,numOfTracks,(int)1);
    beatSlider.setRange(1,maxBeats,(int)1);
    
    presetSlider.setTextBoxStyle(Slider::TextBoxLeft, true, getParentWidth()*0.025, getParentHeight()*topBorder*0.333);
    trackSlider.setTextBoxStyle(Slider::TextBoxLeft, true, getParentWidth()*0.025, getParentHeight()*topBorder*0.333);
    beatSlider.setTextBoxStyle(Slider::TextBoxLeft, true, getParentWidth()*0.025, getParentHeight()*topBorder*0.333);
    
    arpSlider.setSliderStyle(juce::Slider::ThreeValueHorizontal);
    
    
    //Add Listeners===============================================================
    processor.treeState.addParameterListener(PRESET_ID, this); //Adds listener to preset slider tree value.
    processor.treeState.addParameterListener(TRACK_ID, this); //Adds listener to track slider tree value.
    processor.treeState.addParameterListener(BEATS_ID, this); //Adds listener to track slider tree value.
    presetSlider.addMouseListener(this, true);
    beatSlider.addMouseListener(this, true);
    generateButton.addListener(this);
    processor.playPosition.addListener(this);
    
    setWantsKeyboardFocus(true);
    addKeyListener(this);
    
//    presetSlider.setValue(processor.currentPreset);
//    trackSlider.setValue(processor.currentTrack);
//    beatSlider.setValue(processor.presets[currentPreset]->numOfBeats);
    

    //Fill Menus=================================================================
    for(int i=0;i<17;i++) rootMenu.addItem(Theory::rootNames[i], i+1);
    
    for_indexed(auto& mode : Theory::modeMap)
        scaleMenu.addItem(mode.first, static_cast<int>(i)+1);
    
    addItemsToMenu(monoPolyMenu,    {"mono", "poly"});
    addItemsToMenu(generatorMenu,   {"random","arp16th", "arp16th Broken", "arp8th",
                                    "arp8th Broken", "arpTriplet", "arpTriplet Broken"});
    addItemsToMenu(arpDirectionMenu,{"ascend","descend", "expand", "seq1", "seq2", "seq3", "seq4", "seq5"});
    
    //Dropdown look&feel and onChange============================================
    getLookAndFeel().setColour(ComboBox::backgroundColourId, PianoRollerColours::greyOff);
    getLookAndFeel().setColour(ComboBox::outlineColourId, Colours::black);
    getLookAndFeel().setColour(ComboBox::textColourId, Colours::black);
    getLookAndFeel().setColour(ComboBox::arrowColourId, Colours::black);
    
    int val = processor.treeState.getParameterAsValue(SCALE_ID).getValue();
    
    DBG("THE SCALE INDEX VALUE IS " + static_cast<String>(val));
    
    rootMenu.setSelectedItemIndex(processor.treeState.getParameterAsValue(ROOT_ID).getValue(), NotificationType::dontSendNotification);
    monoPolyMenu.setSelectedItemIndex(processor.treeState.getParameterAsValue(MONOPOLY_ID).getValue(), NotificationType::dontSendNotification);
    //scaleMenu.setSelectedItemIndex(5, NotificationType::dontSendNotification);
    String initScaleText = scaleMenu.getItemText( processor.treeState.getParameterAsValue(SCALE_ID).getValue() );
    scaleMenu.setText(initScaleText);
    DBG("Your scale is: " + initScaleText);
    
    generatorMenu.setText("random", NotificationType::dontSendNotification);
    arpDirectionMenu.setText("ascend", NotificationType::dontSendNotification);
    rootMenu.onChange = [this] { rootMenuChanged(); };
    scaleMenu.onChange = [this] { scaleMenuChanged(); };
    monoPolyMenu.onChange = [this] { monoPolyMenuChanged(); };
    generatorMenu.onChange = [this]{ generatorMenuChanged(); };
    arpDirectionMenu.onChange = [this]{ arpDirectionMenuChanged(); };
    
    
    auto monitor = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    auto [monitorWidth, monitorHeight] = std::make_pair(monitor.getWidth(), monitor.getHeight());
    
    presetToBeUpdated = 0;
    beatsToBeUpdated = 0;
    setSize (monitorWidth*0.85, monitorHeight*0.85);
    noteLabel.setText("foo", dontSendNotification);
    noteLabel.setColour(Label::backgroundColourId, PianoRollerColours::greyOff);
    random.setSeedRandomly();
}

PianoRoll1AudioProcessorEditor::~PianoRoll1AudioProcessorEditor()
{
    processor.treeState.removeParameterListener(PRESET_ID, this);
    processor.treeState.removeParameterListener(TRACK_ID, this);
    processor.treeState.removeParameterListener(BEATS_ID, this);
    processor.treeState.removeParameterListener(ROOT_ID, this);
    processor.treeState.removeParameterListener(SCALE_ID, this);
    processor.treeState.removeParameterListener(MONOPOLY_ID, this);
}
//==============================================================================

void PianoRoll1AudioProcessorEditor::addItemsToMenu(ComboBox &comboBox, Array<String> list){
    for_indexed(auto& item : list)
        comboBox.addItem(item, (int)i+1);
}


//==============================================================================
void PianoRoll1AudioProcessorEditor::paint (Graphics& g)
{
    const float width = getWidth();
    const float height = getHeight();
    const int numOfBeats = processor.presets[currentPreset]->numOfBeats;

    //g.fillAll(Colour(156,168,152));
    g.fillAll(greyOff);
    
    tripletSwitches.clear();
    drawTripletSwitches(g, numOfBeats, height, width);

    if(topBorder){
        //PRESET SLIDER "PRESET" LABEL
        const float sliderWidth = presetSlider.getWidth();
        const float sliderHeight = presetSlider.getHeight();
        const float textWidth = presetSlider.getWidth() - presetSlider.getTextBoxWidth();
        const float textHeight = presetSlider.getHeight();
        const float x = presetSlider.getX() + (sliderWidth * presetSlider.getTextBoxWidth()/sliderWidth);
        const float y = presetSlider.getY()-  (topBorder*height*0.2);
        const float spacing = sliderSpacing*width;
        
        g.setFont(textHeight * 0.333);
        g.setColour(Colours::black);
        g.drawText("PRESET", x, y, textWidth, textHeight, Justification::centred);
        
        //TRACK SLIDER "TRACK" LABEL
        g.drawText("TRACK", x+spacing, y, textWidth, textHeight, Justification::centred);
        
        //BEAT SLIDER "BEAT" LABEL
        g.drawText("BEATS", x+(spacing*2), y, textWidth, textHeight, Justification::centred);
        
    }
    
    //String playTime = (String) processor.lastPosInfo.ppqPosition;
    
    
    g.setColour(Colours::black);
    
    
    if(!isChildOfBeatCanvas){
        
        if(pianoKeyPanel){
            const float noteNameX = 0.0f;
            const float noteNameY = height*0.82;
            const float noteNameHeight = height * topBorder * 0.25;
            const float midiNoteNumY = noteNameY + height*0.03;
            
            g.setFont((height + width) / 100);
            g.drawText(noteName.getValue().toString(), noteNameX, noteNameY, pianoKeyWidth*width, noteNameHeight, Justification::centred);
            if (midiNoteNum){
                g.setFont((height + width) / 120);
                g.drawText((String)midiNoteNum, noteNameX, midiNoteNumY, pianoKeyWidth*width, noteNameHeight, Justification::centred);
            }
            g.drawRoundedRectangle(noteNameX + pianoKeyWidth*width*0.04, noteNameY, pianoKeyWidth*width*0.92, noteNameHeight, 10.0f, 1.0f);
            
        }else{
            g.setFont((height + width) / 80);
            
            const float noteNameX = 0.91*width; // X Position of displayed note name.
            const float noteNameY = height*0.004;
            const float noteNameHeight = height * topBorder * 0.25;
            
            g.drawText(noteName.getValue().toString(), noteNameX, noteNameY, width/10, noteNameHeight, Justification::centred);
            g.drawRoundedRectangle(noteNameX + (0.015*width), noteNameY, width/10 - (0.03*width), noteNameHeight, 10.0f, 1.0f);
        }
    }
}

//==============================================================================

void PianoRoll1AudioProcessorEditor::drawTripletSwitches(Graphics& g, const int numOfBeats, const float height, const float width){
    const float beatWidth = (pianoRoll.getWidth() / ((float)numOfBeats) );
    const float ellipseWidth = beatWidth*0.333;
    const float ellipseHeight = height * topBorder * 0.225;
    const float y = height * topBorder * 0.66;
    
    for(int beat=0; beat<numOfBeats;beat++){
        const float x = (beat * beatWidth) + beatWidth*0.333 + pianoKeyWidth*width;
        
        
        if(presets[currentPreset]->tracks[currentTrack]->beatSwitch[beat] == 0){
            g.setColour(Colour(156,168,152)); //If not a triplet
        }else{g.setColour(Colours::limegreen);} //If a triplet.
        
        g.fillEllipse(x, y, ellipseWidth, ellipseHeight);
        g.setColour(Colours::black);
        g.drawEllipse(x, y, ellipseWidth, ellipseHeight, 1.);
        tripletSwitches.add(Array<float>{x+(ellipseWidth/2), y + (ellipseHeight/2)});
    }
    
    
}




//==============================================================================

void PianoRoll1AudioProcessorEditor::paintOverChildren(juce::Graphics &g){
}

void PianoRoll1AudioProcessorEditor::timerCallback(){
    //paintOverChildren();
}




//==============================================================================
//==============================================================================
//==============================================================================
    
    
void PianoRoll1AudioProcessorEditor::resized()
{
    float xMargin = [this]()->float{return pianoKeyPanel ? pianoKeyWidth : 0.0f;}();
    
    pianoRoll.setBoundsRelative(xMargin, topBorder, 1.-pianoKeyWidth, 0.8-topBorder);
    volumePanel.setBoundsRelative(xMargin, 0.8, 1.-pianoKeyWidth, 0.2);
    playCursorWindow.setBoundsRelative(xMargin, 0.0f+topBorder, 1.-pianoKeyWidth, 1.);
    
    if(pianoKeyPanel){
        pianoKeys.setBoundsRelative(0.0f, topBorder, pianoKeyWidth*0.95, 0.8-topBorder);
        auditionStaff.setBoundsRelative(0.0f, 0.86f, pianoKeyWidth, 0.14f);
        scaleDisplayStaff.setBoundsRelative(0.91f, 0.0f, getWidth()*0.1, topBorder*0.5);
    }
    

    const Point<float> panelXY = {0.726f, (float)topBorder * 0.05f};
    const float halfTopBorder = topBorder/2;
    const float smallInput = 0.06f;
    const float largeInput = 0.09;
    const float inputHeight = topBorder * 0.225;
    const float inputRow = inputHeight*1.1f;
    const float rowTwo = panelXY.getY() + inputRow;
    //const float rowThree = rowTwo + inputRow;
    const float spacing = 0.001;
    
    presetSlider.setBoundsRelative(0.01f, topBorder/8, 0.2f, halfTopBorder);
    trackSlider.setBoundsRelative(0.01f+sliderSpacing, topBorder/8, 0.2f, halfTopBorder);
    beatSlider.setBoundsRelative(0.01f+(sliderSpacing*2), topBorder/8, 0.2f, halfTopBorder);
    
    if (isChildOfBeatCanvas == false){
        //Row 1
        rootMenu.setBoundsRelative(panelXY.getX(), panelXY.getY(), smallInput-spacing, inputHeight);
        monoPolyMenu.setBoundsRelative(panelXY.getX()+smallInput, panelXY.getY(), smallInput-spacing, inputHeight);
        generateButton.setBoundsRelative(panelXY.getX() + (smallInput*2), panelXY.getY(), smallInput-spacing, inputHeight);
        //Row 2
        scaleMenu.setBoundsRelative(panelXY.getX(), rowTwo, largeInput-spacing, inputHeight);
        generatorMenu.setBoundsRelative(panelXY.getX()+largeInput, rowTwo, largeInput-spacing, inputHeight);
        arpDirectionMenu.setBoundsRelative(panelXY.getX() + (largeInput*2), rowTwo, largeInput-spacing, inputHeight);
        //Row3
        //arpSlider.setBoundsRelative(panelX, rowThree, largeInput*2, topBorder/5);
    }
}
    


//==============================================================================
    


void PianoRoll1AudioProcessorEditor::setMidiDisplay(int midi){
    midiLabel.setName((String) midi);
}

float PianoRoll1AudioProcessorEditor::dbToVolume(const float db){
    return powf(10.0f, 0.05f * db);
}

float PianoRoll1AudioProcessorEditor::volumeToDB(const float vol){
    return 20.0f * log10f(vol);
}

void PianoRoll1AudioProcessorEditor::playNote(const int pitch, const int volume){
    processor.playNote(pitch, volume);
}


void PianoRoll1AudioProcessorEditor::parameterChanged(const juce::String &parameterID, const float newValue){
    if(parameterID == PRESET_ID){
        const int preset = newValue;
        String monoPoly;
        processor.currentPreset = preset;
        pianoRoll.updatePreset(preset);
        volumePanel.updatePreset(preset);
        isMono() ? monoPoly="mono": monoPoly="poly";
//        monoPolyMenu.setText(monoPoly);
        repaint();
        
        currentPreset = preset;
        //currentNumOfBeats = pianoRoll.presets[currentPreset]->numOfBeats;
        currentNumOfBeats = processor.presets[currentPreset]->numOfBeats;
        beatSlider.setValue(currentNumOfBeats);
        presetToBeUpdated = preset;
//        rootMenu.setSelectedItemIndex(processor.presets[currentPreset]->root, NotificationType::dontSendNotification);
//        scaleMenu.setText(processor.presets[processor.currentPreset]->currentMode, NotificationType::dontSendNotification);
//        generatorMenu.setText(processor.presets[processor.currentPreset]->generatorType, NotificationType::dontSendNotification);
//        arpDirectionMenu.setText(processor.presets[processor.currentPreset]->arpType, NotificationType::dontSendNotification);
        //pianoRoll.changeBeatCanvasPreset(preset);
        
    }
    
    if(parameterID == TRACK_ID){
        currentTrack = newValue;
        
        const int track = newValue;
        pianoRoll.updateTrack(track);
        volumePanel.updateTrack(track);
        processor.currentTrack = track;
        repaint();
    }
    
    if(parameterID == BEATS_ID){
        currentNumOfBeats = newValue;
        
        const int numOfBeats = newValue;
        processor.presets[currentPreset]->numOfBeats = numOfBeats;
        pianoRoll.updateNumOfBeats(numOfBeats);
        volumePanel.updateNumOfBeats(numOfBeats);
        repaint();
        
        beatsToBeUpdated = numOfBeats;
        //pianoRoll.changeBeatCanvasBeats(numOfBeats);
    }
    
}





//COMMUNICATE TO BEAT CANVAS THAT PARAMETERS HAVE CHANGED==================================

void PianoRoll1AudioProcessorEditor::mouseUp(const juce::MouseEvent &event){
    if (presetToBeUpdated > 0){
        pianoRoll.changeBeatCanvasPreset(presetToBeUpdated);
        presetToBeUpdated = 0;
    }
    if(beatsToBeUpdated > 0){
        pianoRoll.changeBeatCanvasBeats(beatsToBeUpdated);
        beatsToBeUpdated = 0;
    }
    
    for(auto beatNum : beatsClicked){
        int tripVal = processor.presets[currentPreset]->tracks[currentTrack]->beatSwitch[beatNum];
        pianoRoll.changeBeatCanvasTriplet(beatNum, tripVal);
    }
    beatsClicked.clear();
}


//FLIP TRIPLET SWITCHES=========================================================//


void PianoRoll1AudioProcessorEditor::mouseDown(const juce::MouseEvent &event){
    if (presetSlider.isMouseButtonDown() != true && beatSlider.isMouseButtonDown() != true){
    
        float x = getMouseXYRelative().getX();
        float y = getMouseXYRelative().getY();
        float width = getWidth();
        float height = getHeight();
        auto thisTrack = processor.presets[currentPreset]->tracks[currentTrack];
        
        for(int beat=0; beat<tripletSwitches.size(); beat++){
            float difX = fabsf( x - tripletSwitches[beat][0] );
            float difY = fabsf( y - tripletSwitches[beat][1] );
            
            if (beatsClicked.contains(beat) == false){ //If hasn't already been clicked or dragged over.
                if(difX < width/20 && difY < height/20){ //If close enough to the beat switch.
                    //if(pianoRoll.presets[currentPreset]->tracks[currentTrack]->beatSwitch[beat] == 0){
                    if(thisTrack->beatSwitch[beat] == 0){
                        thisTrack->beatSwitch.set(beat, 1);
                    }else{
                        thisTrack->beatSwitch.set(beat, 0);
                    }
                    
                    beatsClicked.add(beat);
                }
            }
        }
        
        repaint();
        volumePanel.repaint();
    }
}

void PianoRoll1AudioProcessorEditor::mouseDrag(const juce::MouseEvent &event){
    mouseDown(event);
}


void PianoRoll1AudioProcessorEditor::sliderValueChanged(juce::Slider *slider){
    
}


void PianoRoll1AudioProcessorEditor::valueChanged(juce::Value &value){
    if(value.refersToSameSourceAs(processor.playPosition)){
        auto val = value.getValue();
        float floatVal = val.toString().getFloatValue();
        playCursorWindow.setPlayCursor(floatVal);
    }
    else if(value.refersToSameSourceAs(noteName)){
        repaint();
    }
    
}


void PianoRoll1AudioProcessorEditor::prepToPlayNote(const int note, const int div){
    int beatSwitch = divToBeatSwitch(div);
    auto& [thisPitch, thisVol, active] = getMonoNote(note, beatSwitch);
    
    if (active)
        playNote(thisPitch, thisVol);
}


bool PianoRoll1AudioProcessorEditor::keyPressed(const juce::KeyPress &key, juce::Component *originatingComponent){
    const int keyCode = key.getKeyCode();
    
    if(keyCode >= 49 && keyCode < 58){
        const int newPreset = keyCode - 48;
        presetSlider.setValue(newPreset);
    }else if(keyCode == 32){ //Spacebar
        pianoRoll.spacebar();
    }else if(keyCode == 63232){ //Up Arrow
        processor.octaveShift(1);
        pianoRoll.repaint();
    }else if(keyCode == 63233){ //Down Arrow
        processor.octaveShift(-1);
        pianoRoll.repaint();
    }
    pianoRoll.repaint();
    return true;
}

void PianoRoll1AudioProcessorEditor::rootMenuChanged(){
    const String rootName = rootMenu.getText();
    
    processor.root = Theory::rootNameMap.at(rootName);
    presets[currentPreset]->root = processor.root;
    presets[currentPreset]->rootName = rootName;
    scaleMenuChanged();
}

void PianoRoll1AudioProcessorEditor::scaleMenuChanged(){
    String scaleName = scaleMenu.getText();
    Theory::Mode thisMode = Theory::modeMap.at(scaleName);
    Array<int> scaleRef = thisMode.getMode();
    int root = processor.root;
    
    processor.scale.clear();
    for(auto note : scaleRef) processor.scale.add((note + root) % 12);
    processor.presets[currentPreset]->currentMode = scaleName;
    
    auto [modeNotes, enharmIndex, intervals] = Theory::modeMap.at(scaleName);
    const String rootName = presets[currentPreset]->rootName;
    
    //rootDiatonicValues are {diatonicValue, diatonicMod} e.g. {0, 1} would be C#. 0 is considered natural.
    const auto& [rootDiatonicVal, rootDiatonicMod] = Theory::noteNameToDiatonicValue(rootName);

    int previousDiatonicVal = rootDiatonicMod-7;
    
    auto majorScaleIndex = Theory::circleOfFifths[rootName];
    
    pianoRoll.repaint();
    
    //POPULATE SCALE DISPLAY STAFF WITH NOTES===============//
    scaleDisplayStaff.notes.clear();
    int prevDiatonicVal = -1;
    for(int i = 0; i<processor.scale.size(); i++){
        const int note = processor.scale[i]; //Set class value.
        const int accidental = enharmIndex[i];
        const int interval = intervals[i];
        
        //DBG("Note " + (String)note);
        
        const int diatonicVal = [&, rootDiatonicVal=rootDiatonicVal](){
            const int initVal = (rootDiatonicVal + interval-1)%12; //Get value derived from interval from root.
            return initVal + ( (initVal >= previousDiatonicVal) ? 0 : 7); //Componsate octave, ensure higher than previous note.
        }();
        const int diatonicMod = 2 +
                                majorScaleIndex[diatonicVal] + 
                                accidental;
        const bool repeatDiatonicNote = (diatonicVal==prevDiatonicVal);
        prevDiatonicVal = diatonicVal;
        
        scaleDisplayStaff.notes.push_back(NoteHead(note, diatonicVal, diatonicMod, repeatDiatonicNote));
        previousDiatonicVal = diatonicVal;
        
        //DEBUGGING
        //DBG((String)note + (String)accidental+ (String)interval + "\n");
        //DBG("previousDiatonicVal: " + (String)previousDiatonicVal + "\n");
    }
    
    buttonClicked(&generateButton); //Click the generate button when finished.
}

void PianoRoll1AudioProcessorEditor::monoPolyMenuChanged(){
    bool monoPoly = monoPolyMenu.getText() == "mono";
    presets[currentPreset]->isMono = monoPoly;
    repaint();
}

void PianoRoll1AudioProcessorEditor::generatorMenuChanged(){
    generatorMenu.getText() == "random" ? arpDirectionMenu.setVisible(false): arpDirectionMenu.setVisible(true);
    presets[processor.currentPreset]->generatorType = generatorMenu.getText();
    
    buttonClicked(&generateButton);
}

void PianoRoll1AudioProcessorEditor::arpDirectionMenuChanged(){
    presets[processor.currentPreset]->arpType = arpDirectionMenu.getText();
    buttonClicked(&generateButton);
}

void PianoRoll1AudioProcessorEditor::buttonClicked(Button*){
    currentNumOfBeats = presets[currentPreset]->numOfBeats;
    auto thisTrack = presets[currentPreset]->tracks[currentTrack];
    Theory::Mode thisMode = Theory::modeMap.at(presets[currentPreset]->currentMode);
    Array<int> currentScale = thisMode.getMode();
    String generatorType = generatorMenu.getText();
    int currentOctaveShift = thisTrack->octaveShift;
    
    if(generatorType == "random"){
        for(auto& [thisPitch, thisVol, active] : thisTrack->sixteenthNotes){
            
            if (random.nextInt(100) > 60 || active){ //40 percent chance of a note.
                int pitchClass = processor.scale[random.nextInt(processor.scale.size())];
                int octave = 12 * (4 + random.nextInt(2) + currentOctaveShift);
                thisPitch = pitchClass+octave;
                active = true;
            }
        }
    }else if(generatorType == "arp16th" || generatorType == "arp8th"){
        Array<int> currentScale = thisMode.getMode();
        int scaleSize = currentScale.size();
        int root = presets[currentPreset]->root;
        int arpOctave = (4 + currentOctaveShift) * 12; //How many extra octaves before arpeggio
        int rhythmDiv, stepMod;
        if(generatorType == "arp16th"){rhythmDiv=4;stepMod=0;}
        else if(generatorType == "arp8th"){rhythmDiv=2;stepMod=1;}
        else{rhythmDiv=4;}
        
        for(int sixteenth=0;sixteenth<(presets[currentPreset]->numOfBeats) * rhythmDiv;sixteenth++){;
            const int pitch = currentScale[sixteenth%scaleSize] + root + arpOctave;
            auto& [thisPitch, thisVol, active] = getMonoNote(sixteenth * (4/rhythmDiv) + stepMod, 0);
            
            thisPitch = pitch;
            active = true;
        }
    }else if(generatorType == "arpTriplet"){
        const int root = presets[currentPreset]->root;
        const int arpOctave = (4 + currentOctaveShift) * 12; //How many extra octaves before arpeggio
        
        for(int triplet=0;triplet<currentNumOfBeats*3;triplet++){;
            int pitch = currentScale[triplet%currentScale.size()] + root + arpOctave;
            auto& [thisPitch, thisVol, active] = getMonoNote(triplet, 0);
            
            thisPitch = pitch;
            active = true;
        }
        
        for_indexed(auto& beatSwitch : thisTrack->beatSwitch)
            if(i<currentNumOfBeats) beatSwitch=1;
        
        repaint();
    }
//    else if(generatorType == "arp16th Broken"){
//        std::vector<int> shuffledScale = brokenArpeggio(currentScale);
//        auto scaleSize = shuffledScale.size();
//
//        for(int sixteenth=0;sixteenth<currentNumOfBeats*4;sixteenth++){;
//            int note = shuffledScale[sixteenth%scaleSize];
//            presets[currentPreset]->tracks[processor.currentTrack]->sixteenths.set(sixteenth, note);
//        }
//        for(int beat=0;beat<processor.presets[currentPreset]->numOfBeats;beat++){
//            presets[currentPreset]->tracks[currentTrack]->beatSwitch.set(beat, 0);
//        }
//    }else if(generatorType == "arp8th Broken"){
//        std::vector<int> shuffledScale = brokenArpeggio(currentScale);
//        auto scaleSize = shuffledScale.size();
//
//        for(int eighth=0;eighth<currentNumOfBeats*2;eighth++){;
//            int note = shuffledScale[eighth%scaleSize];
//            presets[currentPreset]->tracks[processor.currentTrack]->sixteenths.set(eighth*2, note);
//            presets[currentPreset]->tracks[processor.currentTrack]->sixteenths.set(eighth*2+1, 0);
//        }
//        for(int beat=0;beat<processor.presets[currentPreset]->numOfBeats;beat++){
//            presets[currentPreset]->tracks[currentTrack]->beatSwitch.set(beat, 0);
//        }
//    }else if(generatorType == "arpTriplet Broken"){
//        std::vector<int> shuffledScale = brokenArpeggio(currentScale);
//        auto scaleSize = shuffledScale.size();
//
//        for(int triplet=0;triplet<currentNumOfBeats*3;triplet++){;
//            int note = shuffledScale[triplet%scaleSize];
//            presets[currentPreset]->tracks[processor.currentTrack]->triplets.set(triplet, note);
//        }
//        for(int beat=0;beat<processor.presets[currentPreset]->numOfBeats;beat++){
//            presets[currentPreset]->tracks[currentTrack]->beatSwitch.set(beat, 1);
//        }
//    }
    
    pianoKeys.selectedKey=0;
    repaint();
}

std::vector<int> PianoRoll1AudioProcessorEditor::brokenArpeggio(Array<int> currentScale){
    int scaleSize = currentScale.size();
    int root = processor.presets[currentPreset]->root;
    int currentOctaveShift = processor.presets[currentPreset]->tracks[currentTrack]->octaveShift;
    int arpOctave = random.nextInt(2)+3 + currentOctaveShift; //How many extra octaves before arpeggio
    String arpType = processor.presets[currentPreset]->arpType;
    
    std::vector<int> order;
    std::vector<int> shuffledScale;
    for(int i=0;i<scaleSize;i++){order.push_back(i);} //Fills "order" with values 0->scaleSize
    
    //Random Shuffle
    if (arpType == "ascend" || arpType == "descend"){
        auto rng = std::default_random_engine {}; //Setup randomizer.
        auto numOfShuffles = random.nextInt(14); //Random number of shuffles.
        
        do{
            for (int i=0;i<numOfShuffles;i++){
                std::shuffle(std::begin(order), std::end(order), rng); //Shuffle the order of notes.
            }
        }while(order == previousOrder); //Always change to a different order.
    }else if(arpType == "seq1"){
        for(int i=0;i<order.size();i++){
            int indexOffset = 0;
            if(i % 2 == 0 && i != 0){ //If even number other than 0.
                order.insert(order.begin() + i-1 + indexOffset, order[i+indexOffset]);
                indexOffset++;
            }
        }
    }else if(arpType == "seq2"){
        order.push_back({scaleSize});
        order.push_back({scaleSize+1});
        currentScale.add(currentScale[0]+12); //Add octave note
        currentScale.add(currentScale[1]+12); //Add ninth
        
        for(int i=0;i<order.size();i++){
            int indexOffset = 0;
            if(i % 2 == 0 && i != 0){ //If even number other than 0.
                order.insert(order.begin() + i-1 + indexOffset, order[i+indexOffset]);
                indexOffset++;
            }
        }
        order.pop_back(); //Remove last element.
    }
    else if(arpType == "seq3"){
        order.push_back({scaleSize});
        order.push_back({scaleSize+1});
        currentScale.add(currentScale[0]+12); //Add octave note
        currentScale.add(currentScale[1]+12); //Add ninth
        
        std::vector<int> newOrder;
        
        for(int i=0;i<order.size()-2;i++){
            newOrder.push_back({order[i]});
            newOrder.push_back({order[i+1]});
            newOrder.push_back({order[i+2]});
        }
        order = newOrder;
    }
    
    
    for(int i=0;i<order.size();i++){
        int val = currentScale[order[i]]; //Determine pitch.
        shuffledScale.push_back(val + root + (arpOctave*12)); //Fill new shuffled scale with pitch.
        
        if(i>0){
            if(arpType == "ascend"){
                while(shuffledScale[i]<shuffledScale[i-1]){
                    shuffledScale[i] += 12;
                }
            }else if(arpType == "descend"){
                while(shuffledScale[i]>shuffledScale[i-1]){
                    shuffledScale[i] -= 12;
                }
                auto octaveDisplacement = (shuffledScale.size() / 4) + 1;
                
                for (auto note : shuffledScale)
                    note += 12*octaveDisplacement;
            }
        }
    }
    previousOrder = order;
    return shuffledScale;
}

