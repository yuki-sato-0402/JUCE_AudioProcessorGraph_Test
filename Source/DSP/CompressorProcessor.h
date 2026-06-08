#pragma once

#include "ProcessorBase.h"

class CompressorProcessor : public ProcessorBase
{
public:
    CompressorProcessor()
    {
        compressor.setThreshold (-20.0f);
        compressor.setRatio (4.0f);
        compressor.setAttack (5.0f);
        compressor.setRelease (50.0f);
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
        spec.numChannels = (juce::uint32) getTotalNumOutputChannels();

        compressor.prepare (spec);
    }

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        compressor.process (context);
    }

    const juce::String getName() const override { return "Compressor"; }

    void setParams (float threshold, float ratio, float attack, float release)
    {
        compressor.setThreshold (threshold);
        compressor.setRatio (ratio);
        compressor.setAttack (attack);
        compressor.setRelease (release);
    }

private:
    juce::dsp::Compressor<float> compressor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorProcessor)
};
