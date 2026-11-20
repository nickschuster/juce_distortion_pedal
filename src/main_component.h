// This is our header file, not really the meat of our application. Skip to main_component.cpp for
// the implementations.

// #pragma is a special instruction to the compiler, in this case telling it to only include this
// header file once per .cpp file, even if it gets included multiple times along the way (e.g.
// through other headers). The reason to only include it once is to avoid defining the same class
// twice in one file, which would cause a compile error.
#pragma once

// #include is roughly the "import" statement in C++ (it literally pastes the header's contents in
// here).
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>

// Class representing the main content component of the application. It inherits from
// juce::AudioAppComponent, juce::Timer and juce::ChangeListener so we can draw stuff to the screen,
// handle audio, use a "timer" callback to do stuff at a regular interval, and get notified when the
// audio device setup changes.
class MainComponent : public juce::AudioAppComponent,
                      private juce::Timer,
                      private juce::ChangeListener
{
public:
    MainComponent();
    ~MainComponent() override;

    // JUCE requires these overrides for AudioAppComponent. They are no-ops for a minimal starter
    // app, but they're required to make the class concrete and compile successfully.
    void prepareToPlay(int bufferSize, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

    // The paint and resized methods inherited from juce::Component, overridden here to draw and lay
    // out our content.
    void paint(juce::Graphics &) override;
    void resized() override;

    // A function exposed by the juce::Timer class, which is called at a regular interval, in this
    // case every 100ms.
    void timerCallback() override;

    // This method is part of the change-listener callback system used by JUCE. It allows us to
    // respond when the audio device setup changes (device, sample rate, buffer size, etc).
    void changeListenerCallback(juce::ChangeBroadcaster *) override;

private:
    // A component to allow the user to select and configure the audio device.
    juce::AudioDeviceSelectorComponent audioSetupComp;

    // labels
    juce::Label cpuUsageText;
    juce::Label latencyText;
    juce::Label callbackTimeText;

    // measurement variables
    std::atomic<float> averageCallbackMs { 0.0f };
    std::atomic<float> peakCallbackMs    { 0.0f };

    // JUCE macro to add a memory leak detector to this class and to prevent copy construction and
    // assignment.
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};