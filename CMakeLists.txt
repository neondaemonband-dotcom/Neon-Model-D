cmake_minimum_required(VERSION 3.22)
project(NeonModelD VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT DEFINED JUCE_DIR)
 message(FATAL_ERROR "Set -DJUCE_DIR=path/to/JUCE")
endif()
add_subdirectory("${JUCE_DIR}" JUCE)
juce_add_plugin(NeonModelD
 COMPANY_NAME "Neon Daemon"
 BUNDLE_ID "com.neondaemon.neonmodeld"
 PLUGIN_MANUFACTURER_CODE Ndmn
 PLUGIN_CODE Nmd1
 FORMATS VST3
 PRODUCT_NAME "Neon Model D"
 IS_SYNTH TRUE
 NEEDS_MIDI_INPUT TRUE
 NEEDS_MIDI_OUTPUT FALSE
 EDITOR_WANTS_KEYBOARD_FOCUS FALSE
 COPY_PLUGIN_AFTER_BUILD FALSE)
target_sources(NeonModelD PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(NeonModelD PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
juce_generate_juce_header(NeonModelD)
target_link_libraries(NeonModelD PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)
