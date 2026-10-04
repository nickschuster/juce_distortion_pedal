#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include "main_component.h"

// this is all template code we don't need to change for now
// essentially handling the application lifecycle and creating
// a window to hold our main component
class MyApplication : public juce::JUCEApplication
{
public:
  const juce::String getApplicationName() override { return JUCE_APPLICATION_NAME_STRING; }
  const juce::String getApplicationVersion() override { return JUCE_APPLICATION_VERSION_STRING; }
  bool moreThanOneInstanceAllowed() override { return true; }

  void initialise(const juce::String &) override
  {
    mainWindow = std::make_unique<MainWindow>(getApplicationName());
  }

  void shutdown() override { mainWindow = nullptr; }
  void systemRequestedQuit() override { quit(); }

private:
  class MainWindow : public juce::DocumentWindow
  {
  public:
    explicit MainWindow(const juce::String &name)
        : DocumentWindow(name,
                         juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
                         DocumentWindow::allButtons)
    {
      setUsingNativeTitleBar(true);
      setContentOwned(new MainComponent(), true);
      setResizable(true, true);
      centreWithSize(getWidth(), getHeight());
      setVisible(true);
    }

    void closeButtonPressed() override
    {
      JUCEApplication::getInstance()->systemRequestedQuit();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
  };

  std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(MyApplication)