#pragma once

#include "ProcessorBase.h"

class DelayProcessor : public ProcessorBase
{
public:
    DelayProcessor() : delayLine (48000 * 2) // 2 seconds max
    {
        delayTimeSamples = 44100.0f * 0.5f; // 500ms default
        feedback = 0.5f;
        mix = 0.5f;
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
        spec.numChannels = (juce::uint32) getTotalNumOutputChannels();

        delayLine.prepare (spec);
    }

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::dsp::AudioBlock<float> block (buffer);
        
        for (size_t channel = 0; channel < block.getNumChannels(); ++channel)
        {
            auto* samples = block.getChannelPointer (channel);
            for (size_t i = 0; i < block.getNumSamples(); ++i)
            {
                float input = samples[i];
                float delayed = delayLine.popSample ((int)channel, delayTimeSamples);
                delayLine.pushSample ((int)channel, input + delayed * feedback);
                samples[i] = (1.0f - mix) * input + mix * delayed;
            }
        }
    }

    const juce::String getName() const override { return "Delay"; }

    void setParams (float timeMs, float fb, float m, double sampleRate)
    {
        delayTimeSamples = (float)(timeMs * 0.001 * sampleRate);
        feedback = fb;
        mix = m;
    }

private:
    juce::dsp::DelayLine<float> delayLine;
    float delayTimeSamples;
    float feedback;
    float mix;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayProcessor)
};
