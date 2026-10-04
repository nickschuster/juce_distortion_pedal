// This file contains the actual application code, which is the entry point for the program.
// It creates a window and adds our main component to it. So we initialize our defined MainComponent
// class here, and then we implement the paint and resized methods to draw and lay out our content.
// This is a C++ pattern, define a class in a header file, and then implement it in a cpp file although
// not explicitly required, it is a common practice to separate the definition and implementation of classes in C++.
// You may see definitions directly in the header file for example.
#include "main_component.h"

// constructor, in this case setting the window size at creation, starting
// our timer to call the timerCallback() function every 100ms, and setting up
// the audio device manager to allow the user to select and configure the audio device
MainComponent::MainComponent() : audioSetupComp(deviceManager,
                                                0,     // minimum input channels
                                                256,   // maximum input channels
                                                0,     // minimum output channels
                                                256,   // maximum output channels
                                                false, // ability to select midi inputs
                                                false, // ability to select midi output device
                                                false, // treat channels as stereo pairs
                                                false) // hide advanced options
{
  deviceManager.addChangeListener(this);
  addAndMakeVisible(audioSetupComp);
  setSize(500, 300);
  startTimer(100);
}

// destructor
MainComponent::~MainComponent()
{
  deviceManager.removeChangeListener(this);
}

// The paint method, in this case setting the background color.
// The look and feel is a JUCE class that handles the default colors and styles for the application
// based on the OS and theme, so we can use it to get the default background color for a resizable window
// Note that we are creating a "resizable window", not all "windows" or applications are resizable
void MainComponent::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

// the resized method, what happens to our content when the user resizes the window?
// We recalculate their relative positions and sizes.
void MainComponent::resized()
{
  // total area of the window
  auto rect = getLocalBounds();

  // give it 10 pixels of padding
  rect.reduce(10, 10);

  // remove 20 pixels from the top of the area
  auto topLine(rect.removeFromTop(20));

  // set the bounds to the top of that area (defaults to all the way left as in 0,0)
  cpuUsageText.setBounds(topLine);
}

// timer function, called every Xms
void MainComponent::timerCallback()
{
  auto cpu = deviceManager.getCpuUsage() * 100;
  cpuUsageText.setText(juce::String(cpu, 6) + " %", juce::dontSendNotification);
}

// This callback is triggered when the selected audio device changes.
// For now we log the new sample rate and buffer size.
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

// JUCE requires these methods for AudioAppComponent.
// They are empty in a minimal starter app because we aren't processing audio yet.
// void MainComponent::prepareToPlay(int /*samplesPerBlockExpected*/, double /*sampleRate*/)
// {
// }

// void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo & /*bufferToFill*/)
// {
// }

// void MainComponent::releaseResources()
// {
// }