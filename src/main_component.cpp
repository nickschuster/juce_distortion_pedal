// This file contains the implementation of our MainComponent class. The entry point for the program
// and the window that holds our main component both live in main.cpp, so here we implement our
// MainComponent class, including the paint and resized methods to draw and lay out our content.
// This is a common C++ pattern: define a class in a header file, then implement it in a .cpp file.
// Although it's not explicitly required, it is common practice to separate the definition and
// implementation of classes in C++.
#include "main_component.h"

// Constructor, in this case setting the window size at creation, starting our timer to call the
// timerCallback() function every 100ms, and setting up the audio device manager to allow the user
// to select and configure the audio device.
MainComponent::MainComponent() : audioSetupComp(deviceManager,
                                                0,     // Minimum input channels
                                                256,   // Maximum input channels
                                                0,     // Minimum output channels
                                                256,   // Maximum output channels
                                                false, // Show MIDI input options
                                                false, // Show MIDI output device selector
                                                false, // Treat channels as stereo pairs
                                                false) // Hide advanced options behind a button (false = show them directly)
{
  deviceManager.addChangeListener(this);
  addAndMakeVisible(audioSetupComp);
  addAndMakeVisible(cpuUsageText);
  setSize(500, 300);
  startTimer(100);
}

// Destructor.
MainComponent::~MainComponent()
{
  deviceManager.removeChangeListener(this);
}

// The paint method, in this case setting the background color. The look and feel is a JUCE class
// that handles the default colors and styles for the application based on the OS and theme, so we
// can use it to get the default background color for a resizable window. Note that we're just
// borrowing the "resizable window" color here; the actual window is created in main.cpp.
void MainComponent::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

// The resized method: what happens to our content when the user resizes the window? We recalculate
// the positions and sizes of our child components. For now, only the CPU label gets positioned; the
// audio device selector doesn't get any bounds yet.
void MainComponent::resized()
{
  // Total area of our component (the window minus the title bar).
  auto rect = getLocalBounds();

  // Give it 10 pixels of padding on every side.
  rect.reduce(10, 10);

  // Remove a 20-pixel strip from the top of the area.
  auto cpuTopLine(rect.removeFromTop(20));
  auto deviceSelectorTopLine(rect.removeFromTop(40));

  // Set the label's bounds to that top strip (it starts at 10,10 because of the padding above).
  cpuUsageText.setBounds(cpuTopLine);
  audioSetupComp.setBounds(deviceSelectorTopLine);
}

// Timer function, called every 100ms (set by startTimer in the constructor).
void MainComponent::timerCallback()
{
  auto cpu = deviceManager.getCpuUsage() * 100;
  cpuUsageText.setText(juce::String(cpu, 6) + " %", juce::dontSendNotification);
}

// This callback is triggered when the audio device setup changes (device, sample rate, buffer size,
// etc). For now we log the new sample rate and buffer size.
void MainComponent::changeListenerCallback(juce::ChangeBroadcaster *)
{
  auto *device = deviceManager.getCurrentAudioDevice();
  if (device != nullptr)
  {
    auto sampleRate = device->getCurrentSampleRate();
    auto bufferSize = device->getCurrentBufferSizeSamples();
    juce::Logger::writeToLog("Sample Rate: " + juce::String(sampleRate) + ", Buffer Size: " + juce::String(bufferSize));
  }
}

// JUCE requires these methods for AudioAppComponent. They are currently defined inline (empty) in
// main_component.h because we aren't processing audio yet. These are left here, commented out, for
// when we move the real implementations into this file. 
// void MainComponent::prepareToPlay(int/*samplesPerBlockExpected*/, double /*sampleRate*/)
// {
// }

// void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo & /*bufferToFill*/)
// {
// }

// void MainComponent::releaseResources()
// {
// }