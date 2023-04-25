#ifndef YUELURMVISION2022_THREADMANAGER_H
#define YUELURMVISION2022_THREADMANAGER_H

#define INDUSTRIAL// USB INDUSTRIAL 

#pragma once

#include<memory>
#include<iostream>
#include<thread>
#include<chrono>


#include "Data/Databuffer.hpp"
#include "Data/DataType.hpp"
#include "Nanodet/nanodet_openvino.h"
#include "ArmorDetectorT/ArmorDetectorT.h"
#include "Serial/Serialcodec.h"
#include "AngleSolver/AngleSolver.h"
#include "Nanodet/nanodet_openvino.h"
#include "ArmorDetectorCNN2T/ArmorDetectorCNN2T.h"

#ifdef USB
#include"../Camera/CameraUSB.h"
#endif
#ifdef INDUSTRIAL
#include "IndustrialCamera//Camera.h"
#endif

namespace hnurm
{

    class ThreadManager
    {
    public:
        ThreadManager():_camera(std::make_unique<HKcam>())
        {};
        ~ThreadManager()=default;

        void InitAll();

        void GenerateThread();

        // void IMUThread();

        void ProcessThread();

        void SendThread();

        void ReceiveThread();

        void Exit();
        
        std::thread::native_handle_type GenerateId;

        std::thread::native_handle_type ProcessId;

        std::thread::native_handle_type SendId;

        std::thread::native_handle_type ReceiveId;


    private:

        //RuneDetector _rune_detector;

        ArmorDetector _armor_detector;

        SerialCodec _serial;

        AngleSolver _angle_solver;

        DataBuffer<VisionRecvData> _receive_buffer;

        DataBuffer<VisionSendData> _send_buffer;

        DataBuffer<ImgInfo> _img_buffer;

        DataBuffer<IMUInfo> _imu_buffer;

        NanoDet detector = NanoDet("/home/rm/hnuyue-lu-rm2022-vision/Nanodet/nanodet.xml");

#ifdef USB
        Camera _camera;
#endif

#ifdef INDUSTRIAL
        std::unique_ptr<HKcam> _camera;
#endif

        //DataBuf<Armor> _target_buffer;
        //DataBuf<IMU> _imu_buffer;

    }; //class ThreadManager

}
#endif //YUELURMVISION2022_THREADMANAGER_H