/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
using namespace juce::dsp;
//==============================================================================
/**
*/
class FirFilter_JUCEAudioProcessor  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    FirFilter_JUCEAudioProcessor();
    ~FirFilter_JUCEAudioProcessor() override = default;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void parameterChanged(const juce::String& parameterID, float newValue) override;

private:
    juce::AudioProcessorValueTreeState parameters;

    float initialWindowParam;  // 初期値を直接保持
    float initialFreqParam;
    float initialOrderParam;

    double lastSampleRate;
    juce::dsp::WindowingFunction<float>::WindowingMethod windowingMethod;
    float cutoffFrequency;
    size_t filterOrder;

    //std::unique_ptr<juce::dsp::FIR::Coefficients<float>> newCoefficients;


    // クラスメンバとして宣言
    juce::CriticalSection parameterUpdateLock;

    ProcessorDuplicator<FIR::Filter<float>, FIR::Coefficients<float>> fir;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirFilter_JUCEAudioProcessor)
};
