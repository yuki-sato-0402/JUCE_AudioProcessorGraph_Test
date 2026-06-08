#pragma once

#include <JuceHeader.h>

class EffectSlot : public juce::Component,
                   public juce::DragAndDropTarget
{
public:
    //A variable that can hold a function that takes two integers and returns nothing
    std::function<void(int, int)> onSwap;
    
    EffectSlot(int index, const juce::String& name)
        : slotIndex(index), effectName(name)
    {
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey.withAlpha(0.5f));
        g.setColour(juce::Colours::white);
        g.drawRect(getLocalBounds(), 2);
        g.setFont(24.0f);
        g.drawText(effectName, getLocalBounds(), juce::Justification::centred, true);
        
        g.setFont(12.0f);
        g.drawText("Slot: " + juce::String(slotIndex + 1), getLocalBounds().removeFromTop(20).reduced(5), juce::Justification::left, true);
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        if (auto* container = findParentComponentOfClass<juce::DragAndDropContainer>())
        {
            container->startDragging("EffectSlot", this);
        }
    }

    // DragAndDropTarget implementation
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override
    {
        return dragSourceDetails.description == "EffectSlot";
    }

    void itemDropped(const SourceDetails& dragSourceDetails) override
    {
        auto* sourceSlot = dynamic_cast<EffectSlot*>(dragSourceDetails.sourceComponent.get());
        if (sourceSlot != nullptr && sourceSlot != this)
        {
            if (onSwap)
                onSwap(sourceSlot->slotIndex, this->slotIndex);
        }
    }

    void setEffectName(const juce::String& newName) { effectName = newName; repaint(); }
    int getSlotIndex() const { return slotIndex; }

private:
    int slotIndex;
    juce::String effectName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectSlot)
};
