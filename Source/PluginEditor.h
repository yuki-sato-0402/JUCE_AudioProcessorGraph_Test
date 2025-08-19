/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FirFilter_JUCEAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
  FirFilter_JUCEAudioProcessorEditor (FirFilter_JUCEAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
  ~FirFilter_JUCEAudioProcessorEditor() override = default;

  //==============================================================================
  void paint (juce::Graphics& g) override;
  void resized() override; 
  typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

private:
  juce::LookAndFeel_V4 lightLookAndFeel;
  juce::AudioProcessorValueTreeState& valueTreeState; 
  juce::Slider dial1Slider;
  juce::Slider dial2Slider;
  

  juce::ComboBox selectComboBox;

  juce::Label  label1;
  juce::Label  label2;
  juce::Label  label3;

  std::unique_ptr<SliderAttachment> dial1Attachment;
  std::unique_ptr<SliderAttachment> dial2Attachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachment;


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FirFilter_JUCEAudioProcessorEditor)
};
