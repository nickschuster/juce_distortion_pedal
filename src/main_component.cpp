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
  addAndMakeVisible(latencyText);
  addAndMakeVisible(cpuUsageText);
  addAndMakeVisible(callbackTimeText);

  modeBox.addItemList({"Clean", "Hard clip", "Soft clip", "Asymmetric", "Oversampled"}, 1);
  modeBox.setSelectedId(mode.load());
  modeBox.onChange = [this] { mode = modeBox.getSelectedId(); };
  addAndMakeVisible(modeBox);

  gainSlider.setRange(1.0, 500.0);
  gainSlider.setSkewFactorFromMidPoint(50.0);
  gainSlider.setTextValueSuffix(" gain");
  gainSlider.setValue(gain.load());
  gainSlider.onValueChange = [this] { gain = (float)gainSlider.getValue(); };
  addAndMakeVisible(gainSlider);

  thresholdSlider.setRange(0.05, 1.0);
  thresholdSlider.setTextValueSuffix(" threshold");
  thresholdSlider.setValue(threshold.load());
  thresholdSlider.onValueChange = [this] { threshold = (float)thresholdSlider.getValue(); };
  addAndMakeVisible(thresholdSlider);

  setSize(500, 600);
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
  auto latencyTopLine(rect.removeFromTop(20));
  auto cpuTopLine(rect.removeFromTop(20));
  auto callbackTimeTopLine(rect.removeFromTop(20));

  // Set the label's bounds to that top strip (it starts at 10,10 because of the padding above).
  latencyText.setBounds(latencyTopLine);
  cpuUsageText.setBounds(cpuTopLine);
  callbackTimeText.setBounds(callbackTimeTopLine);
  modeBox.setBounds(rect.removeFromTop(25));
  gainSlider.setBounds(rect.removeFromTop(25));
  thresholdSlider.setBounds(rect.removeFromTop(25));
  audioSetupComp.setBounds(rect);
}

// Timer function, called every 100ms (set by startTimer in the constructor).
void MainComponent::timerCallback()
{
  auto cpu = deviceManager.getCpuUsage() * 100;
  cpuUsageText.setText(juce::String(cpu, 6) + " %", juce::dontSendNotification);

  callbackTimeText.setText(juce::String(averageCallbackMs.load(), 3) + " ms avg / " + juce::String(peakCallbackMs.load(), 3) + " ms peak",
                           juce::dontSendNotification);
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
    auto inputLatency = device->getInputLatencyInSamples();
    auto outputLatency = device->getOutputLatencyInSamples();

    // Inline function for converting samples to milliseconds
    auto toMs = [sampleRate](int samples)
    { return (samples / sampleRate) * 1000.0; };

    auto latencyLine = device->getName() + " | Sample rate: " + juce::String(sampleRate, 0) + " | Buffer: " + juce::String(bufferSize) + " (" + juce::String(toMs(bufferSize), 2) + " ms)" + " | Reported input latency: " + juce::String(toMs(inputLatency), 2) + " ms" + " | Reported output latency: " + juce::String(toMs(outputLatency), 2) + " ms" + " | Reported round trip: " + juce::String(toMs(inputLatency + outputLatency), 2) + " ms";
    juce::Logger::writeToLog(latencyLine);
    latencyText.setText(latencyLine, juce::dontSendNotification);
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
  oversampling.initProcessing((size_t)bufferSize);
}

// This is the main audio function. As we mentioned, this is the function that will be called
// repeatedly to process audio. It will be called with a buffer of audio data to process and then
// replace with the processed audio data.
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
  const auto startTicks = juce::Time::getHighResolutionTicks();

  juce::dsp::AudioBlock<float> block(*bufferToFill.buffer);
  const auto m = mode.load();
  const auto g = gain.load();
  const auto t = threshold.load();

  auto distort = [m, g, t](juce::dsp::AudioBlock<float> b)
  {
    for (size_t ch = 0; ch < b.getNumChannels(); ++ch)
    {
      auto *data = b.getChannelPointer(ch);
      for (size_t i = 0; i < b.getNumSamples(); ++i)
      {
        auto x = data[i] * g;
        if (m == 2)
          data[i] = juce::jlimit(-t, t, x);
        else if (m == 4)
          data[i] = t * (x >= 0.0f ? std::tanh(x) : std::tanh(x * 0.7f));
        else if (m != 1)
          data[i] = t * std::tanh(x);
      }
    }
  };

  if (m == 5)
  {
    // Upsample. This hands back a block 4x as long.
    distort(oversampling.processSamplesUp(block));

    // Filter and downsample back into the original buffer.
    oversampling.processSamplesDown(block);
  }
  else
    distort(block);

  const auto elapsedMs = (float)(juce::Time::highResolutionTicksToSeconds(
                                     juce::Time::getHighResolutionTicks() - startTicks) *
                                 1000.0);

  averageCallbackMs.store(averageCallbackMs.load() * 0.95f + elapsedMs * 0.05f);

  if (elapsedMs > peakCallbackMs.load())
    peakCallbackMs.store(elapsedMs);
}

// The reverse of prepareToPlay, this function is called when the audio device is stopped or
// changed. We can use it to free up any resources we allocated in prepareToPlay.
void MainComponent::releaseResources()
{
  juce::Logger::writeToLog("releaseResources called");
}