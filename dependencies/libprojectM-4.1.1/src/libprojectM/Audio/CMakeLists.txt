
add_library(Audio OBJECT
        AudioConstants.hpp
        MilkdropFFT.cpp
        MilkdropFFT.hpp
        FrameAudioData.hpp
        PCM.cpp
        PCM.hpp
        Loudness.cpp
        Loudness.hpp
        WaveformAligner.cpp
        WaveformAligner.hpp
        )

target_include_directories(Audio
        PUBLIC
        .
        )

target_link_libraries(Audio
        PUBLIC
        libprojectM::API
        )