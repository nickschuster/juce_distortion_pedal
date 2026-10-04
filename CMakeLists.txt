cmake_minimum_required(VERSION 3.22)
project(SuperDistortion VERSION 0.1.0 LANGUAGES CXX C)

add_subdirectory(JUCE)

juce_add_gui_app(SuperDistortion
    PRODUCT_NAME "SuperDistortion"
    COMPANY_NAME "nik")

target_sources(SuperDistortion PRIVATE
    src/main.cpp
    src/main_component.cpp)

target_compile_features(SuperDistortion PRIVATE cxx_std_17)

target_compile_definitions(SuperDistortion PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:SuperDistortion,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:SuperDistortion,JUCE_VERSION>")

target_link_libraries(SuperDistortion
    PRIVATE juce::juce_audio_devices
            juce::juce_audio_utils
            juce::juce_gui_extra
    PUBLIC  juce::juce_recommended_config_flags
            juce::juce_recommended_warning_flags)