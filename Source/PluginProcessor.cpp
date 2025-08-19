/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FirFilter_JUCEAudioProcessor::FirFilter_JUCEAudioProcessor()
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
      juce::NormalisableRange<float>(0, 7, 1),0),
      std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "freq",  1}, "Freq",
      juce::NormalisableRange<float>(20.f, 20000.f, 0.01f),4400.f),
      std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "order",  1}, "Order",
      juce::NormalisableRange<float>(20, 60, 1),21)
    }
  )
#endif
{
     // Add parameter listeners
    parameters.addParameterListener("window", this);
    parameters.addParameterListener("freq", this);
    parameters.addParameterListener("order", this);

     // 初期値を取得
    initialWindowParam = *parameters.getRawParameterValue("window");
    initialFreqParam = *parameters.getRawParameterValue("freq");
    initialOrderParam = *parameters.getRawParameterValue("order");

    std::cout << "Initial window: " << initialWindowParam << std::endl;
    std::cout << "Initial freq: " << initialFreqParam << std::endl; 
    std::cout << "Initial order: " << initialOrderParam << std::endl;

    // 初期値を使用して設定
    windowingMethod = static_cast<juce::dsp::WindowingFunction<float>::WindowingMethod>(static_cast<int>(initialWindowParam));
    cutoffFrequency = initialFreqParam;
    filterOrder = static_cast<size_t>(initialOrderParam);

}

//==============================================================================
const juce::String FirFilter_JUCEAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FirFilter_JUCEAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FirFilter_JUCEAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FirFilter_JUCEAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FirFilter_JUCEAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FirFilter_JUCEAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FirFilter_JUCEAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FirFilter_JUCEAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String FirFilter_JUCEAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void FirFilter_JUCEAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
    // NB: This method is not used in this example, but you can implement it if you want to support program names.
    // You can also use the AudioProcessorValueTreeState to manage program names.
}

//==============================================================================
void FirFilter_JUCEAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize =  static_cast<uint32> (samplesPerBlock);
    spec.numChannels = static_cast<uint32> (getTotalNumOutputChannels());

//    fir.state = FilterDesign<float>::designFIRLowpassWindowMethod (440.0f, sampleRate, 21,
//        WindowingFunction<float>::blackman);
    fir.reset();
    fir.state = juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(
        cutoffFrequency, lastSampleRate, filterOrder, windowingMethod);
    fir.prepare (spec);
}

void FirFilter_JUCEAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FirFilter_JUCEAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FirFilter_JUCEAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedLock parameterLock (parameterUpdateLock);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    

    juce::dsp::AudioBlock<float> block (buffer);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    //fir.state = juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(
    //    cutoffFrequency, lastSampleRate, filterOrder, windowingMethod);
    fir.process(juce::dsp::ProcessContextReplacing<float> (block));
}

//==============================================================================
bool FirFilter_JUCEAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FirFilter_JUCEAudioProcessor::createEditor()
{
    return new FirFilter_JUCEAudioProcessorEditor (*this,  parameters);
}

//==============================================================================
void FirFilter_JUCEAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FirFilter_JUCEAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void FirFilter_JUCEAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{

    if (parameterID == "window")
    {
        std::cout << "Window function changed to: " << newValue << std::endl;
        auto newWindowingMethod =
            static_cast<juce::dsp::WindowingFunction<float>::WindowingMethod>(static_cast<int>(newValue));
        windowingMethod = newWindowingMethod;
    }
    else if (parameterID == "freq")
    {
        std::cout << "Cutoff frequency changed to: " << newValue << std::endl;
        cutoffFrequency = newValue;
    }
    else if (parameterID == "order")
    {
        std::cout << "Order changed to: " << newValue << std::endl;
        filterOrder = static_cast<size_t>(newValue);
    }

    juce::ScopedLock parameterLock (parameterUpdateLock);

    *fir.state = *juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(
        cutoffFrequency, lastSampleRate, filterOrder, windowingMethod);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FirFilter_JUCEAudioProcessor();
}
