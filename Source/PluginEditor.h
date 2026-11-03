#pragma once
#include "PluginProcessor.h"
#include <vector>
class NeonModelDAudioProcessorEditor final : public juce::AudioProcessorEditor {
public:
 explicit NeonModelDAudioProcessorEditor(NeonModelDAudioProcessor&);
 void paint(juce::Graphics&) override;
 void resized() override;
private:
 NeonModelDAudioProcessor& processor;
 std::vector<std::unique_ptr<juce::Slider>> sliders;
 std::vector<std::unique_ptr<juce::Label>> labels;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NeonModelDAudioProcessorEditor)
};
