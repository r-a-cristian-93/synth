cmake_minimum_required(VERSION 3.16.3)
project(Synth)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")


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
)

ADD_DEFINITIONS(-D__LINUX_ALSA__)

add_executable(Synth ${SOURCE_SYNTH} ${SOURCE_RTMIDI} ${SOURCE_NANOMIDI})
target_link_libraries(Synth dl pthread m curses asound)
target_include_directories(Synth PRIVATE ${PROJECT_SOURCE_DIR}/include)

