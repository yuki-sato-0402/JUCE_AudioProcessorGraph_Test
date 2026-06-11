#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioProcessorGraphEditorTest::AudioProcessorGraphEditorTest (AudioProcessorGraphTest& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor(p), valueTreeState(vts)
{
    currentOrder = { 0, 1, 2, 3 };

    for (int i = 0; i < 4; ++i)
    {
        // Create an EffectSlot for each effect and set up the onSwap callback
        auto* slot = new EffectSlot (i, effectNames[currentOrder[i]]);
        // Assigning a function object (lambda) to a member variable.
        // The `itemDropped` function in the `EffectSlot` class defines the callback function that is called when a slot is dropped.
        slot->onSwap = [this](int s1, int s2) { 
            swapSlots(s1, s2); 
            //This is equivalent to this->swapSlots(s1, s2);
        };
        slots.add (slot);
        addAndMakeVisible (slot);
    }

    updateAttachments();

    setSize (600, 400);
}

//==============================================================================
void AudioProcessorGraphEditorTest::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioProcessorGraphEditorTest::resized()
{
    auto area = getLocalBounds();
    auto w = area.getWidth() / 2;
    auto h = area.getHeight() / 2;

    slots[0]->setBounds (0, 0, w, h);
    slots[1]->setBounds (w, 0, w, h);
    slots[2]->setBounds (0, h, w, h);
    slots[3]->setBounds (w, h, w, h);
}

//slotIndex1 is the index of the slot to which the data was dragged, and slotIndex2 is the index of the slot to which the data was dropped.
void AudioProcessorGraphEditorTest::swapSlots (int slotIndex1, int slotIndex2)
{
    std::cout << "Swapping slots " << slotIndex1 << " and " << slotIndex2 << std::endl;
    int effectIdx1 = currentOrder[slotIndex1];
    int effectIdx2 = currentOrder[slotIndex2];

    currentOrder.set (slotIndex1, effectIdx2);
    currentOrder.set (slotIndex2, effectIdx1);

    updateSlotNames();
    updateAttachments();
    audioProcessor.updateGraphOrder (currentOrder);
}

void AudioProcessorGraphEditorTest::updateSlotNames()
{
    for (int i = 0; i < 4; ++i)
    {
        slots[i]->setEffectName (effectNames[currentOrder[i]]);
    }
}


//Implement a method to dynamically rebuild the button-parameter mappings (ButtonAttachment) whenever the effector in a slot is replaced.        
void AudioProcessorGraphEditorTest::updateAttachments()
{
   /* For example, when an FIR filter is in Slot 1, the checkbox for Slot 1 controls the FIR filter; 
   after moving the FIR filter to Slot 2, the checkbox for Slot 2 will continue to control it.*/
    bypassAttachments.clear();

    for (int i = 0; i < 4; ++i)
    {
        auto paramID = "bypass_" + juce::String(currentOrder[i]);
        bypassAttachments.add(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (valueTreeState, paramID, slots[i]->getBypassButton()));
    }
}
