cmake_minimum_required (VERSION 3.11...3.16)
project(netero_audio_samples
    VERSION 1.0
    DESCRIPTION "Netero audio samples programmes."
    LANGUAGES CXX)

add_executable(audio_recorder audio_record.cpp)
add_dependencies(audio_recorder Netero::Netero Netero::Audio)
target_include_directories(audio_recorder PUBLIC ${Netero_INCLUDE_DIRS})
target_link_libraries(audio_recorder Netero::Netero Netero::Audio)

add_executable(audio_play_sinus audio_play_sinus.cpp)
add_dependencies(audio_play_sinus Netero::Netero Netero::Audio)
target_include_directories(audio_play_sinus PUBLIC ${Netero_INCLUDE_DIRS})
target_link_libraries(audio_play_sinus Netero::Netero Netero::Audio)

