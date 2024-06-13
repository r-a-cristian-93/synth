cmake_minimum_required(VERSION 3.16.3)
project(Synth)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

message(STATUS "Configuring project for ${CMAKE_SYSTEM_NAME}")

if(WIN32)
  add_definitions(-D__WINDOWS_MM__)
  message(STATUS "Compiling for Windows: __WINDOWS_MM__ flag added")
elseif(UNIX AND NOT APPLE)
  add_definitions(-D__LINUX_ALSA__)
  message(STATUS "Compiling for Linux: __LINUX_ALSA__ flag added")
endif()

set (SOURCE_RTMIDI
  include/RtMidi/RtMidi.cpp
)

set (SOURCE_NANOMIDI
  include/nanomidi/nanomidi_decoder.c
  include/nanomidi/nanomidi_encoder.c
  include/nanomidi/nanomidi_stream.c
  include/nanomidi/msgprint.c
)

set (SOURCE_SYNTH
  src/Synth.cpp
  src/AudioDriver.cpp
  src/MidiDriver.cpp
  src/OrganSynthesizer.cpp
)

set (SOURCE_SYNTH_ENGINE
  include/OrganEngine/OrganOscillator.cpp
  include/OrganEngine/Envelope.cpp
  include/OrganEngine/NoteManager.cpp
  include/OrganEngine/MidiManager.cpp
  include/OrganEngine/RotarySpeaker.cpp
  include/OrganEngine/RingBuffer.cpp
  include/OrganEngine/WaveTables.cpp
)

if(BUILD_FOR_JACK)
  add_definitions(
    -DMA_ENABLE_ONLY_SPECIFIC_BACKENDS
    -DMA_ENABLE_JACK
  )
  message(STATUS "Building with JACK support")
endif()

add_executable(Synth ${SOURCE_SYNTH} ${SOURCE_SYNTH_ENGINE} ${SOURCE_RTMIDI} ${SOURCE_NANOMIDI})
target_include_directories(Synth PRIVATE ${PROJECT_SOURCE_DIR}/include)

if(WIN32)
  target_link_libraries(Synth winmm)
elseif(UNIX AND NOT APPLE)
  target_link_libraries(Synth dl pthread m curses asound)
endif()
