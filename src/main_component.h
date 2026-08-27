#pragma once
#include <juce_gui_extra/juce_gui_extra.h>

class MainComponent : public juce::Component
{
public:
    MainComponent();
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::TextButton button { "Click me" };
    juce::Label label;

    int clicks = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};