/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "UI/EffectSlot.h"

class AudioProcessorGraphEditorTest  : public juce::AudioProcessorEditor,
                                            public juce::DragAndDropContainer
{
public:
  AudioProcessorGraphEditorTest (AudioProcessorGraphTest&, juce::AudioProcessorValueTreeState& vts);
  ~AudioProcessorGraphEditorTest() override = default;
  //==============================================================================
  void paint (juce::Graphics& g) override;
  void resized() override; 
  
  void swapSlots (int slotIndex1, int slotIndex2);
  void updateSlotNames();
  void updateAttachments();

private:
  AudioProcessorGraphTest& audioProcessor;
  juce::AudioProcessorValueTreeState& valueTreeState; 

  juce::OwnedArray<EffectSlot> slots;
  juce::Array<int> currentOrder; // Indices into effectNames

  juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachments;

  const juce::StringArray effectNames { "FIR Filter", "Delay", "Reverb", "Compressor" };

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessorGraphEditorTest)
};
