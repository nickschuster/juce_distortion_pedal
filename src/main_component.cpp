#include "main_component.h"

MainComponent::MainComponent()
{
  addAndMakeVisible(button);
  addAndMakeVisible(label);

  label.setText("No clicks yet", juce::dontSendNotification);
  label.setJustificationType(juce::Justification::centred);

  button.onClick = [this]
  {
    ++clicks;
    label.setText("Clicks: " + juce::String(clicks), juce::dontSendNotification);
  };

  setSize(500, 300);
}

void MainComponent::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
  auto area = getLocalBounds().reduced(20);
  label.setBounds(area.removeFromTop(40));
  button.setBounds(area.removeFromTop(40).withSizeKeepingCentre(140, 32));
}