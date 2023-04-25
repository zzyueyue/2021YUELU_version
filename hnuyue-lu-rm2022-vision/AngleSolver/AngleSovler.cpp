#include"AngleSolver.h"

namespace hnurm
{
    bool AngleSolver::Init()
    {
        _big_armor_real={cv::Point3f(-BIG_ARMOR_LEN / 2.0, BIG_ARMOR_WID / 2.0, 0.0),
                         cv::Point3f(BIG_ARMOR_LEN / 2.0, BIG_ARMOR_WID / 2.0, 0.0),
                         cv::Point3f(BIG_ARMOR_LEN / 2.0, -BIG_ARMOR_WID / 2.0, 0.0),
                         cv::Point3f(-BIG_ARMOR_LEN / 2.0, -BIG_ARMOR_WID / 2.0, 0.0)};

        _small_armor_real={cv::Point3f(-SMALL_ARMOR_LEN / 2.0, SMALL_ARMOR_WID / 2.0, 0.0),
                           cv::Point3f(SMALL_ARMOR_LEN / 2.0, SMALL_ARMOR_WID / 2.0, 0.0),
                           cv::Point3f(SMALL_ARMOR_LEN / 2.0, -SMALL_ARMOR_WID / 2.0, 0.0),
                           cv::Point3f(-SMALL_ARMOR_LEN / 2.0, -SMALL_ARMOR_WID / 2.0, 0.0)};

        _rotation_vect = cv::Mat::zeros(3, 1, CV_64FC1);
        _translation_vect = cv::Mat::zeros(3, 1, CV_64FC1);

        cv::FileStorage fs_read("/home/rm/hnuyue-lu-rm2022-vision/AngleSolver/out_camera_data.xml", cv::FileStorage::READ);
        if (!fs_read.isOpened())
        {
            std::cerr << "failed to open CameraParams.xml" << std::endl;
            return false;
        }
        fs_read["camera_matrix"] >> _camera_matrix;
        fs_read["distortion_coefficients"] >> _distortion_matrix;
        fs_read.release();
        return true;
    }


    void AngleSolver::Solve(TargetInfo& target,float& distance,float& pitch,float& yaw)
    {
        //TODO: try some other method for solvePnP
        try
        {
            if(target._big_flag)
            {
                cv::solvePnP(_big_armor_real, target.vertexes, _camera_matrix, _distortion_matrix, _rotation_vect, _translation_vect, false, cv::SOLVEPNP_ITERATIVE);
            }

            if (!target._big_flag)
            {
                //std::cout<<target.vertexes.size()<<std::endl;
                cv::solvePnP(_small_armor_real, target.vertexes, _camera_matrix, _distortion_matrix, _rotation_vect, _translation_vect, false, cv::SOLVEPNP_ITERATIVE);
            }
        }
        catch(std::exception e){}

        distance = sqrt(pow(_translation_vect.at<double>(0, 0),2)
                      + pow(_translation_vect.at<double>(1, 0),2)
                      + pow(_translation_vect.at<double>(2, 0),2)
                       );

        yaw = atan(_translation_vect.at<double>(0, 0) / _translation_vect.at<double>(2, 0)) / CV_PI * 180;
        pitch = atan(_translation_vect.at<double>(1, 0) /  _translation_vect.at<double>(2, 0)) / CV_PI * 180;
    }


    void AngleSolver::CoordinatesTrans()
    {

    }

    double AngleSolver::GetRecompensation(double camera_pitch, double camera_distance, double gimbal_pitch, double bullet_speed)
    {
        double pg_angle=atan(GUN_BIAS_Y/GUN_BIAS_X);
        double pg_distance=sqrt(GUN_BIAS_X*GUN_BIAS_X+GUN_BIAS_Y*GUN_BIAS_Y);
        //transform these to standard unit first
        camera_pitch=camera_pitch/180*PI;
        gimbal_pitch=gimbal_pitch/180*PI;
        camera_distance/=1000;

        //calculate pitch angle of pivot axis
        double pivot_horizontal_distance = camera_distance * cos(camera_pitch) - CAMERA_BIAS_Y * sin(gimbal_pitch) + CAMERA_BIAS_X * cos(gimbal_pitch);
        double pivot_vertical_distance = camera_distance * sin(camera_pitch) + CAMERA_BIAS_Y * cos(gimbal_pitch) + CAMERA_BIAS_X * sin(gimbal_pitch);
        double pivot_pitch_angle = atan(pivot_vertical_distance / pivot_horizontal_distance);

        double pivot_distance = pivot_horizontal_distance / cos(pivot_pitch_angle);

        double gun_horizontal_distance = pivot_horizontal_distance - pg_distance * cos(pivot_pitch_angle+pg_angle);
        double gun_vertical_distance = pivot_vertical_distance - pg_distance * sin(pivot_pitch_angle+pg_angle);
        double target_angle = atan(gun_vertical_distance/gun_horizontal_distance);
        //double gun_distance = pivot_distance - pg_distance * cos(pg_angle);
        //initialization and declaration of iteration
        double temp_angle = target_angle;
        double target_height = gun_vertical_distance;
        //cout<<temp_angle*180/PI<<endl;
        double temp_height;
        double vy0;
        double vx0;
        double t;
        double real_height;
        double delta_height;

//fixed iteration times
        for (int i = 0; i < FIX; i++)
        {
            //updata parameters
            gun_horizontal_distance = pivot_horizontal_distance - pg_distance * cos(temp_angle+pg_angle);
            temp_height = gun_horizontal_distance * tan(temp_angle);
            vy0 = sin(temp_angle) * bullet_speed;
            vx0 = cos(temp_angle) * bullet_speed;

            t = (exp(K * gun_horizontal_distance) - 1) / (K * vx0);
            real_height = vy0 * t - 0.5 * G * t * t;
            delta_height = target_height - real_height;

            //angle should be updated as soon as we worked out delta
            temp_angle = atan((temp_height + delta_height) / gun_horizontal_distance);
//            printf("height:%f  ", real_height);
//            printf("delta:%f  ", delta_height);

        }
        return temp_angle*180/PI;
    }



}//namespace hnurm
