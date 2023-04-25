#include "ThreadManager.h"
using namespace std;
using namespace cv;
namespace hnurm
{

    void ThreadManager::InitAll()
    {

        if(_serial.open_port("/dev/ttyUSB0"))
        {
            std::cout<<"open port!"<<std::endl;
        }

        _serial.rm_init();
        if(!_camera->OpenCam())
        {
            std::cout<<"fail"<<std::endl;
            std::exit(-1);
        }
        _angle_solver.Init();
        _serial.SetColor();
    }
    

    void ThreadManager::Exit()
    {
        _camera->CloseCam();
    }


    void ThreadManager::GenerateThread()
    {
        ImgInfo temp_frame;
        while(true)
        {

            if(_camera->GetFlame(temp_frame))
            {
                _img_buffer.Update(temp_frame);
                //std::cout<<temp_frame.img.cols<<std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(6));
        }

    }


    void ThreadManager::ProcessThread()
    {
        ImgInfo temp_frame;
        VisionRecvData temp_recv;
        TargetInfo temp_armor;
        VisionSendData temp_send;
        double speed;

        
        int state=0;

        while(1)
        {
            _receive_buffer.Get(temp_recv);
            if(_img_buffer.Get(temp_frame))
            {
                if(temp_frame.img.empty())
                {
                    continue;
                }    
                cv::imshow("t",temp_frame.img);
                state=webcam_demo(detector, temp_frame, _armor_detector, _serial.GetColor(), temp_armor);
                cv::waitKey(1);
                if(temp_armor.shoot == 1 && state==1)
                {
                    temp_send.shoot=true;
                    _angle_solver.Solve(temp_armor, temp_send.distance, temp_send.pitch, temp_send.yaw);
                    IMUInfo temp_imu;
                    _imu_buffer.Get(temp_imu);
                    speed=_serial.GetSpeed(temp_recv);
                    temp_send.pitch = _angle_solver.GetRecompensation(temp_imu.pitch+temp_send.pitch,temp_send.distance,temp_imu.pitch,speed)-temp_imu.pitch;
                    _send_buffer.Update(temp_send);
                }
                else
                {
                 temp_send.shoot=false;
                 temp_send.distance=0;
                 temp_send.pitch=0;
                 temp_send.yaw=0;
                 _send_buffer.Update(temp_send);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(6));
        }
    }


    void ThreadManager::SendThread()
    {
        VisionSendData temp_send;
        while(1)
        {
            _send_buffer.Get(temp_send, nullptr);

            std::cout << "distance= " << temp_send.distance
                      << " pitch  = " << temp_send.pitch
                      << "  yaw   = " << temp_send.yaw << std::endl;

            _serial.send_data(temp_send.shoot,temp_send.pitch,temp_send.yaw);

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }


    void ThreadManager::ReceiveThread()
    {
        VisionRecvData temp_recv;
        IMUInfo imu_angle;

        while(1)
        {
            _serial.try_get_recv_data_for(temp_recv,imu_angle);
            _receive_buffer.Update(temp_recv);
            _imu_buffer.Update(imu_angle);
            // std::cout<<(int)temp_recv.speed<<std::endl;
            // std::cout<<(int)temp_recv.self_color<<std::endl;
            // std::cout<<imu_angle.pitch<<"   "<<imu_angle.yaw<<std::endl; 
            

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

    }

} //hnurm