/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/FIRProcessor.h"
#include "DSP/DelayProcessor.h"
#include "DSP/ReverbProcessor.h"
#include "DSP/CompressorProcessor.h"

using namespace juce::dsp;
//==============================================================================
/**
*/
class AudioProcessorGraphTest  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    AudioProcessorGraphTest();
    ~AudioProcessorGraphTest() override = default;

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

    void updateGraphOrder (const juce::Array<int>& newOrder);

private:
    juce::AudioProcessorValueTreeState parameters;


    std::unique_ptr<juce::AudioProcessorGraph> mainGraph;

    using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
     /** A convenient typedef for referring to a pointer to a node object. */
    using Node = juce::AudioProcessorGraph::Node;
    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;
    Node::Ptr midiInputNode;
    Node::Ptr midiOutputNode;

    Node::Ptr firNode;
    Node::Ptr delayNode;
    Node::Ptr reverbNode;
    Node::Ptr compressorNode;

    juce::Array<Node::Ptr> effectNodes;
    juce::Array<int> currentOrder; // indices 0, 1, 2, 3

    void initialiseGraph();
    void updateConnections();

    juce::CriticalSection parameterUpdateLock;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessorGraphTest)
};
