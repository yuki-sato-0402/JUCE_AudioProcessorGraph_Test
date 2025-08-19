/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FirFilter_JUCEAudioProcessorEditor::FirFilter_JUCEAudioProcessorEditor (FirFilter_JUCEAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), valueTreeState(vts)// 参照メンバーを初期化（必須）
{
  lightLookAndFeel.setColourScheme(juce::LookAndFeel_V4::getLightColourScheme());

  selectComboBox.addItem("Rectangular", 1);
  selectComboBox.addItem("Triangular", 2);
  selectComboBox.addItem("Hann", 3); 
  selectComboBox.addItem("Hamming", 4); 
  selectComboBox.addItem("Blackman", 5); 
  selectComboBox.addItem("Blackman-Harris", 6); 
  selectComboBox.addItem("Flat Top", 7); 
  selectComboBox.addItem("Kaiser", 8); 
  selectComboBox.setSelectedItemIndex(0);
  comboBoxAttachment.reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "window", selectComboBox));
    
  selectComboBox.setLookAndFeel(&lightLookAndFeel);
  addAndMakeVisible(selectComboBox);
  label3.setText ("window function", juce::dontSendNotification);
  label3.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(label3);
  
  std::cout << "cutoff freq" << std::endl;
  dial1Attachment.reset (new SliderAttachment (valueTreeState, "freq", dial1Slider));
  dial1Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  dial1Slider.setTextValueSuffix (" hz");     
  dial1Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial1Slider.getTextBoxWidth(), dial1Slider.getTextBoxHeight());
  addAndMakeVisible(dial1Slider);
  dial1Slider.setLookAndFeel(&lightLookAndFeel);
  dial1Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
 
  label1.setText ("cutoff freq", juce::dontSendNotification);
  label1.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(label1);
  
  std::cout << "order" << std::endl;
  dial2Attachment.reset (new SliderAttachment (valueTreeState, "order", dial2Slider));
  dial2Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);    
  dial2Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial2Slider.getTextBoxWidth(), dial2Slider.getTextBoxHeight());
  addAndMakeVisible(dial2Slider);
  dial2Slider.setLookAndFeel(&lightLookAndFeel);
  dial2Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);

  label2.setText ("order", juce::dontSendNotification);
  label2.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(label2);

  setSize(480, 220);
}

//==============================================================================
void FirFilter_JUCEAudioProcessorEditor::paint (juce::Graphics& g)
{
  g.fillAll(juce::Colours::grey);
}

void FirFilter_JUCEAudioProcessorEditor::resized()
{
  auto area = getLocalBounds();
  auto componentWidth = (area.getWidth() - 80)/3;
  auto componentHeight = area.getHeight()- 40 ;
  auto padding = 20;          

  selectComboBox.setBounds(padding,  padding + 50, componentWidth ,  componentHeight / 4);
  dial1Slider.setBounds(selectComboBox.getRight() + padding, padding,  componentWidth, componentHeight);
  dial2Slider.setBounds(dial1Slider.getRight() + padding, padding ,  componentWidth , componentHeight);

  label1.setBounds(dial1Slider.getX(), padding, dial1Slider.getWidth(),dial1Slider.getTextBoxHeight() );
  label2.setBounds(dial2Slider.getX(), padding, dial2Slider.getWidth(),dial2Slider.getTextBoxHeight() );
  label3.setBounds(selectComboBox.getX(), padding, selectComboBox.getWidth(),selectComboBox.getHeight() );

}
