#pragma once

#include "ProcessorBase.h"

class FIRProcessor : public ProcessorBase
{
public:
    FIRProcessor()
    {
        cutoff = 4400.0f;
        order = 21;
        window = juce::dsp::WindowingFunction<float>::blackman;
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        lastSampleRate = sampleRate;
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
        spec.numChannels = (juce::uint32) getTotalNumOutputChannels();

        updateFilter();
        fir.prepare (spec);
    }

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        fir.process (context);
    }

    const juce::String getName() const override { return "FIR Filter"; }

    void updateFilter()
    {
        if (lastSampleRate > 0)
        {
            *fir.state = *juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod (cutoff, lastSampleRate, (size_t)order, window);
        }
    }

    void setParams (float newCutoff, int newOrder, juce::dsp::WindowingFunction<float>::WindowingMethod newWindow)
    {
        cutoff = newCutoff;
        order = newOrder;
        window = newWindow;
        updateFilter();
    }

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter<float>, juce::dsp::FIR::Coefficients<float>> fir;
    float cutoff;
    int order;
    juce::dsp::WindowingFunction<float>::WindowingMethod window;
    double lastSampleRate = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FIRProcessor)
};
