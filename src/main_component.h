// pragma is a special instruction to the compiler, in this case telling it to only include this header file one
// time, even if it is included multiple times in the project. The reason to only include it once is to avoid
// duplicate definitions of the same class, which would cause a compile error
#pragma once

// include is the "import" statement in C++
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>

// class representing the main content component of the application, implements the juce::Component and juce::Timer interfaces
// so we can draw stuff to the screen and access a "timer" callback to do stuff at a regular interval
class MainComponent : public juce::AudioAppComponent,
                      private juce::Timer,
                      private juce::ChangeListener
{
public:
    MainComponent();
    ~MainComponent() override;

    // JUCE requires these overrides for AudioAppComponent.
    // They are no-ops for a minimal starter app, but they are required to make the class concrete
    // and compile successfully.
    void prepareToPlay(int /*samplesPerBlockExpected*/, double /*sampleRate*/) override {}
    void getNextAudioBlock(const juce::AudioSourceChannelInfo & /*bufferToFill*/) override {}
    void releaseResources() override {}

    // inherited paint and resized methods from juce::Component, overridden here to draw and lay out our content
    void paint(juce::Graphics &) override;
    void resized() override;

    // a function exposed by the juce::Timer class, which is called at a regular interval, in this case every 100ms
    void timerCallback() override;

    // This method is part of the change-listener callback system used by JUCE.
    // It allows us to respond when the selected audio device changes.
    void changeListenerCallback(juce::ChangeBroadcaster *) override;

private:
    // a component to allow the user to select the audio device and configure it
    juce::AudioDeviceSelectorComponent audioSetupComp;

    // a label to display the CPU usage
    juce::Label cpuUsageText;

    // JUCE macro to add a memory leak detector to this class, and to prevent copy construction and assignment
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};