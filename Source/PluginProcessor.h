#pragma once
#include <JuceHeader.h>
#include <array>
#include <cmath>

class NeonModelDAudioProcessor final : public juce::AudioProcessor {
public:
 NeonModelDAudioProcessor();
 void prepareToPlay(double, int) override;
 void releaseResources() override {}
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override { return true; }
 const juce::String getName() const override { return "Neon Model D"; }
 bool acceptsMidi() const override { return true; }
 bool producesMidi() const override { return false; }
 bool isMidiEffect() const override { return false; }
 double getTailLengthSeconds() const override { return 0.0; }
 int getNumPrograms() override { return 1; }
 int getCurrentProgram() override { return 0; }
 void setCurrentProgram(int) override {}
 const juce::String getProgramName(int) override { return {}; }
 void changeProgramName(int,const juce::String&) override {}
 void getStateInformation(juce::MemoryBlock&) override;
 void setStateInformation(const void*,int) override;
 juce::AudioProcessorValueTreeState apvts;
 static juce::AudioProcessorValueTreeState::ParameterLayout makeParams();
private:
 double sr=44100.0, env=0.0, filterEnv=0.0, ladder[4]={}, phases[3]={}, lfoPhase=0.0, glideHz=110.0;
 int note=-1; bool gate=false; float velocity=0.0f;
 enum class Stage { idle, attack, decay, sustain, release } ampStage=Stage::idle, filterStage=Stage::idle;
 double nextEnvelope(double current,Stage& stage,double attack,double decay,double sustain,double release);
 static double oscillator(double phase,int wave);
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NeonModelDAudioProcessor)
};
