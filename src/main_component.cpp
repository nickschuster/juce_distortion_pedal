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
  setAudioChannels(2, 2);
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
  shutdownAudio();
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

// The three functions below are the true "audio" functions called by JUCE to do things to our
// audio. We setup a device selector in the constructor which will handle the input and output
// selection for our program. Now we can choose to take some input and pass it to the output.

// This is the start function. Whenever anything about our current audio setup changes (device
// selection, sample rate, buffer size, etc.) JUCE will call this function first. We can use it to
// setup any state we need to process audio e.g. a dedicated array for delay from previous buffers
// (called a "delay line"). In this case we are not going to setup anything.
void MainComponent::prepareToPlay(int bufferSize, double sampleRate)
{
  juce::Logger::writeToLog("prepareToPlay called");
}

// This is the main audio function. As we mentioned, this is the function that will be called
// repeatedly to process audio. It will be called with a buffer of audio data to process and then
// replace with the processed audio data.
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
  // Now we can play around a bit with our audio signal.
  auto *buffer = bufferToFill.buffer;

  // For each channel (e.g. stereo has 2 channels, left and right)
  auto numChannels = buffer->getNumChannels();
  for (int ch = 0; ch < numChannels; ++ch)
  {
    // get the samples in that channel
    auto *samples = buffer->getWritePointer(ch, bufferToFill.startSample);

    // loop through them and
    for (int i = 0; i < bufferToFill.numSamples; ++i)
    {
      // process each sample (example: apply a random gain between 0.5 and 1.0 to each sample for a simple distortion effect).
      samples[i] *= juce::Random::getSystemRandom().nextFloat() * 0.5f + 0.5f;
    }
  }
}

// The reverse of prepareToPlay, this function is called when the audio device is stopped or
// changed. We can use it to free up any resources we allocated in prepareToPlay.
void MainComponent::releaseResources()
{
  juce::Logger::writeToLog("releaseResources called");
}