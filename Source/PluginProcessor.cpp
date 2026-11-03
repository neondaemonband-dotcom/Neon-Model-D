#include "PluginProcessor.h"
#include "PluginEditor.h"

NeonModelDAudioProcessor::NeonModelDAudioProcessor()
 : AudioProcessor(BusesProperties().withOutput("Output",juce::AudioChannelSet::stereo(),true)),
 apvts(*this,nullptr,"PARAMETERS",makeParams()) {}

juce::AudioProcessorValueTreeState::ParameterLayout NeonModelDAudioProcessor::makeParams() {
 std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
 auto add=[&](const char* id,const char* name,float low,float high,float def){
  p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},name,juce::NormalisableRange<float>(low,high),def));
 };
 add("osc1","OSC 1 LEVEL",0,1,0.85f); add("osc2","OSC 2 LEVEL",0,1,0.65f); add("osc3","OSC 3 LEVEL",0,1,0.55f);
 add("wave1","OSC 1 WAVE",0,3,1); add("wave2","OSC 2 WAVE",0,3,1); add("wave3","OSC 3 WAVE",0,3,2);
 add("oct1","OSC 1 OCT",-2,2,0); add("oct2","OSC 2 OCT",-2,2,0); add("oct3","OSC 3 OCT",-2,2,-1);
 add("detune","DETUNE",0,30,5); add("glide","GLIDE",0,1,0.0f);
 add("cutoff","CUTOFF",30,12000,1300); add("emphasis","EMPHASIS",0,0.95f,0.32f); add("contour","CONTOUR AMT",-4,4,1.8f);
 add("attack","ATTACK",0.001f,2,0.008f); add("decay","DECAY",0.02f,3,0.35f); add("sustain","SUSTAIN",0,1,0.65f); add("release","RELEASE",0.02f,3,0.22f);
 add("fattack","FILTER ATTACK",0.001f,2,0.005f); add("fdecay","FILTER DECAY",0.02f,3,0.3f); add("fsustain","FILTER SUSTAIN",0,1,0.1f);
 add("drive","DRIVE",0.5f,5,1.35f); add("noise","NOISE",0,0.4f,0); add("volume","VOLUME",0,0.8f,0.3f);
 add("lforate","LFO RATE",0.1f,20,4); add("lfopitch","LFO PITCH",0,1,0); add("lfocutoff","LFO FILTER",0,2,0);
 return {p.begin(),p.end()};
}
void NeonModelDAudioProcessor::prepareToPlay(double sampleRate,int) {
 sr=sampleRate; env=filterEnv=0; std::fill(std::begin(ladder),std::end(ladder),0.0);
 phases[0]=phases[1]=phases[2]=lfoPhase=0; glideHz=110; note=-1; gate=false; ampStage=filterStage=Stage::idle;
}
bool NeonModelDAudioProcessor::isBusesLayoutSupported(const BusesLayout& b) const {
 return b.getMainOutputChannelSet()==juce::AudioChannelSet::stereo() || b.getMainOutputChannelSet()==juce::AudioChannelSet::mono();
}
double NeonModelDAudioProcessor::oscillator(double p,int wave) {
 switch(wave) { case 0: return 2*p-1; case 1: return 1-4*std::abs(p-0.5); case 2: return p<0.5?1.0:-1.0; default: return p<0.25?1.0:-1.0; }
}
double NeonModelDAudioProcessor::nextEnvelope(double v,Stage& stage,double a,double d,double s,double r) {
 auto coeff=[&](double t){return std::exp(-1.0/(std::max(0.001,t)*sr));};
 switch(stage) {
 case Stage::idle: return 0;
 case Stage::attack: v=1+(v-1)*coeff(a); if(v>=0.999) {v=1;stage=Stage::decay;} break;
 case Stage::decay: v=s+(v-s)*coeff(d); if(std::abs(v-s)<0.001) {v=s;stage=Stage::sustain;} break;
 case Stage::sustain: v=s; break;
 case Stage::release: v*=coeff(r); if(v<0.00001) {v=0;stage=Stage::idle;} break;
 } return v;
}
void NeonModelDAudioProcessor::processBlock(juce::AudioBuffer<float>& audio,juce::MidiBuffer& midi) {
 juce::ScopedNoDenormals guard; audio.clear();
 auto v=[&](const char* id){return static_cast<double>(apvts.getRawParameterValue(id)->load());};
 const double levels[]={v("osc1"),v("osc2"),v("osc3")};
 const int waves[]={static_cast<int>(std::round(v("wave1"))),static_cast<int>(std::round(v("wave2"))),static_cast<int>(std::round(v("wave3")))};
 const double octs[]={std::round(v("oct1")),std::round(v("oct2")),std::round(v("oct3"))};
 const double detune=v("detune"),glide=v("glide"),cutoff=v("cutoff"),res=v("emphasis"),contour=v("contour");
 const double atk=v("attack"),dec=v("decay"),sus=v("sustain"),rel=v("release");
 const double fatk=v("fattack"),fdec=v("fdecay"),fsus=v("fsustain");
 const double drive=v("drive"),noise=v("noise"),volume=v("volume"),lforate=v("lforate"),lfopitch=v("lfopitch"),lfocutoff=v("lfocutoff");
 juce::MidiBuffer::Iterator it(midi); juce::MidiMessage msg; int eventSample=0; bool hasEvent=it.getNextEvent(msg,eventSample);
 for(int s=0;s<audio.getNumSamples();++s) {
  while(hasEvent && eventSample<=s) {
   if(msg.isNoteOn()) {note=msg.getNoteNumber();velocity=msg.getFloatVelocity();gate=true;ampStage=Stage::attack;filterStage=Stage::attack;}
   else if(msg.isAllNotesOff() || msg.isAllSoundOff() || (msg.isNoteOff() && msg.getNoteNumber()==note)) {gate=false;ampStage=Stage::release;filterStage=Stage::release;}
   hasEvent=it.getNextEvent(msg,eventSample);
  }
  env=nextEnvelope(env,ampStage,atk,dec,sus,rel);
  filterEnv=nextEnvelope(filterEnv,filterStage,fatk,fdec,fsus,rel);
  lfoPhase+=lforate/sr; lfoPhase-=std::floor(lfoPhase);
  const double lfo=std::sin(2*juce::MathConstants<double>::pi*lfoPhase);
  const double targetHz=note>=0?juce::MidiMessage::getMidiNoteInHertz(note):110.0;
  const double glideCoeff=std::exp(-1.0/(std::max(0.0001,glide*glide)*sr));
  glideHz=targetHz+(glideHz-targetHz)*glideCoeff;
  double signal=0;
  if(note>=0 && env>0.000001) {
   for(int i=0;i<3;++i) {
    const double cents=(i-1)*detune+lfopitch*35*lfo;
    const double freq=glideHz*std::pow(2.0,octs[i]+cents/1200.0);
    phases[i]+=std::min(freq,sr*0.45)/sr; phases[i]-=std::floor(phases[i]);
    signal+=levels[i]*oscillator(phases[i],waves[i]);
   }
  }
  // Deterministic noise generator: no allocation, no external dependencies.
  static thread_local uint32_t rng=0x51A7C3u;
  rng^=rng<<13; rng^=rng>>17; rng^=rng<<5;
  signal+=noise*(2.0*(rng/4294967295.0)-1.0);
  signal=std::tanh(signal*drive*0.5);
  const double fc=juce::jlimit(20.0,sr*0.20,cutoff*std::pow(2.0,contour*filterEnv+lfocutoff*lfo));
  // Four cascaded nonlinear one-pole stages with bounded resonance feedback.
  // Ladder-inspired approximation, not a circuit-exact Moog model.
  const double a=1.0-std::exp(-2.0*juce::MathConstants<double>::pi*fc/sr);
  double x=std::tanh(signal-3.8*res*ladder[3]);
  for(auto& stage:ladder) {stage+=a*(std::tanh(x)-std::tanh(stage)); x=stage;}
  const float out=static_cast<float>(std::tanh(ladder[3]*2.0)*env*velocity*volume);
  for(int ch=0;ch<audio.getNumChannels();++ch) audio.setSample(ch,s,out);
 }
}
void NeonModelDAudioProcessor::getStateInformation(juce::MemoryBlock& dest) {
 auto xml=apvts.copyState().createXml(); if(xml) copyXmlToBinary(*xml,dest);
}
void NeonModelDAudioProcessor::setStateInformation(const void* data,int bytes) {
 auto xml=getXmlFromBinary(data,bytes); if(xml && xml->hasTagName(apvts.state.getType())) apvts.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessorEditor* NeonModelDAudioProcessor::createEditor() {return new NeonModelDAudioProcessorEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {return new NeonModelDAudioProcessor();}
