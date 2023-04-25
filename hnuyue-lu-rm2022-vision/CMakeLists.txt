cmake_minimum_required(VERSION 3.16.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "debug")

project(HNU)

find_package(OpenCV REQUIRED)
find_package(InferenceEngine REQUIRED)
find_package(ngraph REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

file(GLOB_RECURSE source
        ArmorDetectorT/*
        ArmorDetectorCNN2T/*
        Nanodet/*
        ThreadManager/*
        IndustrialCamera/*
        Data/*
        Serial/*
        AngleSolver/*
        )

add_executable(HNU main.cpp ${source})

target_link_libraries( HNU
    ${InferenceEngine_LIBRARIES}
    ${NGRAPH_LIBRARIES}
    ${OpenCV_LIBS}
    /opt/MvCamCtrlSDK/lib/64/libFormatConversion.so
    /opt/MvCamCtrlSDK/lib/64/libMediaProcess.so
    /opt/MvCamCtrlSDK/lib/64/libMvCameraControl.so
    /opt/MvCamCtrlSDK/lib/64/libMVRender.so
    /opt/MvCamCtrlSDK/lib/64/libMvUsb3vTL.so
    -lpthread -lm
)

