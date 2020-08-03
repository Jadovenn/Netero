cmake_minimum_required(VERSION 3.11...3.16)
project(netero_audio_samples
        VERSION 1.0
        DESCRIPTION "Netero audio samples programmes."
        LANGUAGES CXX)

add_executable(scan_audio_devices scan_audio_devices.cpp)
add_dependencies(scan_audio_devices Netero::Netero Netero::Audio)
target_include_directories(scan_audio_devices PUBLIC ${Netero_INCLUDE_DIRS})
target_link_libraries(scan_audio_devices Netero::Netero Netero::Audio)

add_executable(record_audio_to_wave record_audio_to_wave.cpp)
add_dependencies(record_audio_to_wave Netero::Netero Netero::Audio)
target_include_directories(record_audio_to_wave PUBLIC ${Netero_INCLUDE_DIRS})
target_link_libraries(record_audio_to_wave Netero::Netero Netero::Audio)

add_executable(play_audio_sinus play_audio_sinus.cpp)
add_dependencies(play_audio_sinus Netero::Netero Netero::Audio)
target_include_directories(play_audio_sinus PUBLIC ${Netero_INCLUDE_DIRS})
target_link_libraries(play_audio_sinus Netero::Netero Netero::Audio)

