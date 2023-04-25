#ifndef TOGETHER_ANGLESOLVER_H
#define TOGETHER_ANGLESOLVER_H
#ifndef ANGLESOLVER_H
#define ANGLESOLVER_H

#include<opencv2/calib3d.hpp>
#include<opencv2/opencv.hpp>
#include<vector>
#include"Data/DataType.hpp"
#include<math.h>
#include <exception>

//armor size
#define BIG_ARMOR_LEN 217.5
#define BIG_ARMOR_WID 49.5
#define SMALL_ARMOR_LEN 137.2
#define SMALL_ARMOR_WID 54.1

//definitions of constants
#define PI 3.1415926535       //circle ratio
#define E 2.7182818284        //nature exponent
#define K 0.01                 //air attrition coefficiency
#define G 9.7883              //g of ShenZhen,g of ChangSha is:9.7915,calculated using g=9.7803(1+0.0053024sin虏蠄-0.000005sin虏2蠄) m/s虏
#define CAMERA_BIAS_Y 0.01 //0.0248  //unit:m the vertical distance between optical center of camera and pitch pivot
#define CAMERA_BIAS_X  0.01//0.19342 //unit:m the horizontal distance between optical center of camera and pitch pivot
#define GUN_BIAS_Y 0.02621    //unit:m the vertical distance between axis of gun center and pitch pivot
#define GUN_BIAS_X 0.19342    //unit:m the horizontal distance between axis of gun center and pitch pivot
                              //---!!!temporally,GUN_BIAS_X = CAMERA_BIAS_X!!!---
#define FIX 10
namespace hnurm
{

    class AngleSolver
    {
    public:

        AngleSolver()=default;
        ~AngleSolver()=default;

        /**
         * @brief
         *
         * @return true
         * @return false
         */
        bool Init();


        /**
         * @brief
         *
         * @param target
         * @param distance
         * @param pitch
         * @param yaw
         */
        void Solve(TargetInfo& target,float& distance,float& pitch,float& yaw);


        /**
         * @brief get the world coordinates of target,in order to predict movement
         *        no paremeter is required,using _translation_vect
         */
        void CoordinatesTrans();


        //overload b of realsense bifocal camera
        //just return the depth,that's nice!
        //bool SolveDistance();


        /**
         * @brief Get pitch angle after recompensation,using iteration to
         *        approximate the best pitch angle for gimbal to aim
         * @param  camera_pitch     unit:掳    pitch angle of camera
         * @param  camera_distance  unit:mm   distance between camera and the armor to attack
         * @param  gimbal_pitch     unit:掳    temporal pitch angle of gimbal
         * @param  bullet_speed     unit:m/s  bullet speed,15 30 12 etc.
         * @return double           unit:掳    pitch angle of gimbal after recompensation
         *
         * @todo  test more for accurate constant and some fixed bias
         * @todo  add air attrition to axis-y too,though the differential equation may be complex...
         */

        double GetRecompensation(double camera_pitch, double camera_distance, double gimbal_pitch, double bullet_speed);

    private:
        cv::Mat _camera_matrix;       //Camera Matrix from Camera calibration
        cv::Mat _distortion_matrix;   //Distortion Matrix from Camera calibration

        std::vector<cv::Point3f> _big_armor_real;
        std::vector<cv::Point3f> _small_armor_real;

        std::vector<cv::Point3d> _target_world_coordinate;
        cv::Mat _rotation_vect;
        cv::Mat _translation_vect;

    };//anglesolver

} // namespace hnurm

#endif // ANGLESOLVER_H

#endif //TOGETHER_ANGLESOLVER_H
