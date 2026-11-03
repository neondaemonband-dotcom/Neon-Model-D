#include "PluginEditor.h"
NeonModelDAudioProcessorEditor::NeonModelDAudioProcessorEditor(NeonModelDAudioProcessor& p)
 : AudioProcessorEditor(&p),processor(p) {
 const char* ids[]={"osc1","osc2","osc3","wave1","wave2","wave3","oct1","oct2","oct3","detune","glide","cutoff","emphasis","contour","attack","decay","sustain","release","fattack","fdecay","fsustain","drive","noise","volume","lforate","lfopitch","lfocutoff"};
 const char* names[]={"OSC 1 MIX","OSC 2 MIX","OSC 3 MIX","OSC 1 WAVE","OSC 2 WAVE","OSC 3 WAVE","OSC 1 OCT","OSC 2 OCT","OSC 3 OCT","DETUNE","GLIDE","CUTOFF","EMPHASIS","CONTOUR AMT","ATTACK","DECAY","SUSTAIN","RELEASE","FILTER ATT","FILTER DEC","FILTER SUS","DRIVE","NOISE","VOLUME","LFO RATE","LFO PITCH","LFO FILTER"};
 for(int i=0;i<27;++i) {
  auto slider=std::make_unique<juce::Slider>();
  slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  slider->setTextBoxStyle(juce::Slider::TextBoxBelow,false,70,18);
  slider->setColour(juce::Slider::rotarySliderFillColourId,juce::Colour::fromRGB(210,155,82));
  slider->setColour(juce::Slider::thumbColourId,juce::Colours::whitesmoke);
  addAndMakeVisible(*slider);
  auto label=std::make_unique<juce::Label>();label->setText(names[i],juce::dontSendNotification);
  label->setJustificationType(juce::Justification::centred);
  label->setColour(juce::Label::textColourId,juce::Colours::whitesmoke);
  addAndMakeVisible(*label);
  attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,ids[i],*slider));
  sliders.push_back(std::move(slider));labels.push_back(std::move(label));
 }
 setSize(1000,700);
}
void NeonModelDAudioProcessorEditor::paint(juce::Graphics& g) {
 g.fillAll(juce::Colour::fromRGB(21,23,26));
 g.setColour(juce::Colour::fromRGB(214,162,95));
 g.setFont(juce::Font(30.0f,juce::Font::bold));
 g.drawText("NEON MODEL D",25,12,650,44,juce::Justification::centredLeft);
 g.setFont(13.0f);g.drawText("THREE OSCILLATORS  /  FOUR-POLE LADDER-INSPIRED FILTER  /  MONO SYNTH",27,58,800,20,juce::Justification::centredLeft);
 g.setColour(juce::Colour::fromRGB(70,70,70));g.drawLine(20,89,980,89,2);
}
void NeonModelDAudioProcessorEditor::resized() {
 for(int i=0;i<27;++i) {const int col=i%7,row=i/7;const int x=14+col*140,y=105+row*137;
  labels[i]->setBounds(x,y,130,20);sliders[i]->setBounds(x+5,y+22,120,100);
 }
}
