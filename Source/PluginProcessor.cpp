/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioProcessorGraphTest::AudioProcessorGraphTest()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    parameters(*this, nullptr, juce::Identifier("PARAMETERS"),
    juce::AudioProcessorValueTreeState::ParameterLayout {
      std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "window",  1}, "Window",
      juce::NormalisableRange<float>(0, 7, 1), 0),
      std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "freq",  1}, "Freq",
      juce::NormalisableRange<float>(20.f, 20000.f, 0.01f), 4400.f),
      std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "order",  1}, "Order",
      juce::NormalisableRange<float>(20, 60, 1), 21),
      std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "bypass_0", 1 }, "Bypass FIR", false),
      std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "bypass_1", 1 }, "Bypass Delay", false),
      std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "bypass_2", 1 }, "Bypass Reverb", false),
      std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "bypass_3", 1 }, "Bypass Compressor", false)
    }
  ),

//  This specifies "which type of object to create."
  mainGraph (std::make_unique<juce::AudioProcessorGraph>())
#endif
{
    parameters.addParameterListener("window", this);
    parameters.addParameterListener("freq", this);
    parameters.addParameterListener("order", this);
    parameters.addParameterListener("bypass_0", this);
    parameters.addParameterListener("bypass_1", this);
    parameters.addParameterListener("bypass_2", this);
    parameters.addParameterListener("bypass_3", this);

    currentOrder = { 0, 1, 2, 3 }; // FIR, Delay, Reverb, Compressor
}

//==============================================================================
const juce::String AudioProcessorGraphTest::getName() const
{
    return JucePlugin_Name;
}

bool AudioProcessorGraphTest::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioProcessorGraphTest::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioProcessorGraphTest::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioProcessorGraphTest::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioProcessorGraphTest::getNumPrograms()
{
    return 1;
}

int AudioProcessorGraphTest::getCurrentProgram()
{
    return 0;
}

void AudioProcessorGraphTest::setCurrentProgram (int index)
{
}

const juce::String AudioProcessorGraphTest::getProgramName (int index)
{
    return {};
}

void AudioProcessorGraphTest::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AudioProcessorGraphTest::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mainGraph->setPlayConfigDetails (getMainBusNumInputChannels(),
                                     getMainBusNumOutputChannels(),
                                     sampleRate, samplesPerBlock);
    mainGraph->prepareToPlay (sampleRate, samplesPerBlock);

    initialiseGraph();
}

void AudioProcessorGraphTest::initialiseGraph()
{
   // Delete all connections to previously existing nodes
    mainGraph->clear();

    audioInputNode = mainGraph->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioInputNode));
    audioOutputNode = mainGraph->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioOutputNode));
    midiInputNode = mainGraph->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::midiInputNode));
    midiOutputNode = mainGraph->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::midiOutputNode));

    firNode = mainGraph->addNode (std::make_unique<FIRProcessor>());
    delayNode = mainGraph->addNode (std::make_unique<DelayProcessor>());
    reverbNode = mainGraph->addNode (std::make_unique<ReverbProcessor>());
    compressorNode = mainGraph->addNode (std::make_unique<CompressorProcessor>());

    // Initialize bypass states
    firNode->setBypassed(*parameters.getRawParameterValue("bypass_0") > 0.5f);
    delayNode->setBypassed(*parameters.getRawParameterValue("bypass_1") > 0.5f);
    reverbNode->setBypassed(*parameters.getRawParameterValue("bypass_2") > 0.5f);
    compressorNode->setBypassed(*parameters.getRawParameterValue("bypass_3") > 0.5f);

    // Initialize FIR with current parameter values
    if (auto* firProc = dynamic_cast<FIRProcessor*> (firNode->getProcessor()))
    {
        float freq = *parameters.getRawParameterValue("freq");
        int order = (int)*parameters.getRawParameterValue("order");
        int windowIdx = (int)*parameters.getRawParameterValue("window");
        auto window = static_cast<juce::dsp::WindowingFunction<float>::WindowingMethod>(windowIdx);
        firProc->setParams(freq, order, window);
    }

   // Delete or Add Elements (juce::Array<Node::Ptr>)
    effectNodes.clear();
    effectNodes.add (firNode);
    effectNodes.add (delayNode);
    effectNodes.add (reverbNode);
    effectNodes.add (compressorNode);

    updateConnections();
}

void AudioProcessorGraphTest::updateConnections()
{
    for (auto connection : mainGraph->getConnections())
        mainGraph->removeConnection (connection);

    Node::Ptr lastNode = audioInputNode;

     // These require a nodeID and a channel index for building the appropriate connections and the whole process is repeated for all the required channels.
    for (int i = 0; i < currentOrder.size(); ++i)
    {
        Node::Ptr currentNode = effectNodes[currentOrder[i]];
        
        for (int channel = 0; channel < 2; ++channel)
            mainGraph->addConnection ({ { lastNode->nodeID, channel }, { currentNode->nodeID, channel } });

        lastNode = currentNode;
    }

   
    for (int channel = 0; channel < 2; ++channel)
        mainGraph->addConnection ({ { lastNode->nodeID, channel }, { audioOutputNode->nodeID, channel } });

    // MIDI pass-through
    mainGraph->addConnection ({ { midiInputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex }, { midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });
}

// If the order of the effects changes
void AudioProcessorGraphTest::updateGraphOrder (const juce::Array<int>& newOrder)
{
    currentOrder = newOrder;
    updateConnections();
}

void AudioProcessorGraphTest::releaseResources()
{
    mainGraph->releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioProcessorGraphTest::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

/*
Plugin
 ├─ OscillatorProcessor
 ├─ FilterProcessor
 ├─ DelayProcessor
 └─ ReverbProcessor
 As shown above, each module acts as an independent AudioProcessor.
 In other words, each has its own `processBlock()` method.
 From the host's perspective, `AudioProcessorGraphTest::processBlock()` is called, 
 but `AudioProcessorGraphTest` internally calls the `processBlock()` method of each module.
*/

void AudioProcessorGraphTest::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    for (auto i = getMainBusNumInputChannels(); i < getMainBusNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    mainGraph->processBlock (buffer, midiMessages);
}

//==============================================================================
bool AudioProcessorGraphTest::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AudioProcessorGraphTest::createEditor()
{
    return new AudioProcessorGraphEditorTest (*this, parameters);
}

//==============================================================================
void AudioProcessorGraphTest::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AudioProcessorGraphTest::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void AudioProcessorGraphTest::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "bypass_0" && firNode != nullptr) firNode->setBypassed(newValue > 0.5f);
    if (parameterID == "bypass_1" && delayNode != nullptr) delayNode->setBypassed(newValue > 0.5f);
    if (parameterID == "bypass_2" && reverbNode != nullptr) reverbNode->setBypassed(newValue > 0.5f);
    if (parameterID == "bypass_3" && compressorNode != nullptr) compressorNode->setBypassed(newValue > 0.5f);

    if (firNode != nullptr)
    {
        auto* firProc = dynamic_cast<FIRProcessor*> (firNode->getProcessor());
        if (firProc != nullptr)
        {
            float freq = *parameters.getRawParameterValue("freq");
            int order = (int)*parameters.getRawParameterValue("order");
            int windowIdx = (int)*parameters.getRawParameterValue("window");
            auto window = static_cast<juce::dsp::WindowingFunction<float>::WindowingMethod>(windowIdx);
            
            firProc->setParams(freq, order, window);
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioProcessorGraphTest();
}
