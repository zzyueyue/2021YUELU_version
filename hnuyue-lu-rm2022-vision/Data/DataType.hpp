#ifndef DATATYPE_H
#define DATATYPE_H

#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<chrono>
#include<vector>

namespace hnurm
{
    //Enum type for state infomation 

    /**
     * @brief opeartor will choose this,read from slave computer
     * 
     */
    enum class WorkMode : int 
    {
        none=0,
        assist_aim=1,   //track target,operator will fire
        auto_fire=2,    //for armor,choose the best timing automatically
        assist_rune=3,  //track rune,operator will fire
        auto_rune=4,    //for rune,choose the best timing to fire
        slang_shoot=5,  //for hero
        auto_save=6,    //for engine
        auto_grab=7,      //for engine
        auto_shoot=8,
        auto_windmill=9
    };

    //@TODO: reduce bit expenditure
    enum class SelfColor:int
    {
        none = 0,
        red=5,
        blue=10
    };
    
    /**
     * @brief bullet speed for pitch compensation
     * 
     */
    enum class BulletSpeed : int
    {
        none=0,
        s15=12,
        s18=14,
        s30=15,
        b10=7,
        b16=8
    };


    /**
     * @brief read from referee color
     * 
     */
    enum class EnemyColor : int
    {
        none=-1,
        red=0,
        blue=1
    };


    /**
     * @brief this can help operator to find the best fire timing
     * 
     */
    enum class AimState : int
    {
        none=0,
        target_lost=1,
        target_converging=2,
        target_lock=3
    };


    /**
     * @brief for sentry and auto_infantry
     * 
     */
    enum class HealthLevel : int
    {
        none=0,
        healthy=1,
        good=2,
        modest=3,
        less=4,
        lower=5,
        danger=6
    };


    /**
     * @brief temporary for sentry moving strategy
     * 
     */
    enum class MovingSpeed : int
    {
        none=0,
        stay=1,
        slow=2,
        fast=3,
        erratic=4
    };


    /**
     * @brief for the operator to choose aiming target when there is lost of armors
     * @todo for balance infantry, they got 3/4/5 big armor
     *       add them later(if possible)
     */
    enum class TargetType : int
    {
        none=0,
        hero_1=1,
        engine_2=2,
        infantry3=3,    
        infantry4=4,
        infantry5=5,
        outpost=6,
        sentry=7,
        base_s=8,
        base_b=9
    };


    // data pack struct and class

    struct VisionSendData
    {
        //strut initiate function
        VisionSendData(){}
        VisionSendData(bool shoot_, float pitch_, float yaw_):
                shoot(shoot_), pitch(pitch_), yaw(yaw_){}

        bool shoot;
        int cam_id;     // formatted in flag_register, low 8 bits  ||camera id(for engine car,there are two cameras)
        float pitch;
        float yaw;      // formatted in float data
        float distance; // formatted in float data

        short time_stamp;
    };


    /* memory figure
    *
    * self_color | act_id
    *       work_mode
    *
    */
    struct VisionRecvData
    {
        VisionRecvData(){}
        VisionRecvData(SelfColor self_color_, int actuator_id_, WorkMode mode_,  BulletSpeed speed_):
                self_color(self_color_), actuator_id(actuator_id_), mode(mode_), speed(speed_){}

        SelfColor self_color; // formatted in flag_register, lower 4 bits of low 8 bits
        int actuator_id;       // formatted in flag_register, higher 4 bits of low 8 bits
        WorkMode mode;        // formatted in flag_register, lower 4 bits of high 8 bits
        BulletSpeed speed; // formatted in flag_register, higher 4 bits of high 8 bits

        short time_stamp;
    };

    
    /**
     * @brief 
     * @param img a mat store pixel info
     * @param create_time when this frame is being created
     * @param count counts from first frame
     * @param cam_id if we have mul cameras,indicating the order
     */
    struct ImgInfo
    {
        ImgInfo(){}

        //ImgInfo(){}
        //std::time_t time_stamp;To do: this is a big bug
        cv::Mat img;
        std::time_t create_time;
        long count;
        int cam_id;
    };
    

    /**
     * @brief target for further process
     * 
     */
    struct TargetInfo
    {
        TargetInfo(){}

        //TargetInfo(){}

        /**
         * @brief draw target for visualization
         * 
         * @param canvas where to draw 
         */
        void DrawTarget(cv::Mat& canvas)
        {
            for(int i=0,j=vertexes.size();i<j;i++)
            {
                cv::line(canvas,vertexes[i%j],vertexes[(i+1)%j],
                         cv::Scalar(0,255,0),1);
            }
        }
        std::vector<cv::Point2f> vertexes;
        TargetType target_type;
        bool _big_flag = 1;
        bool shoot = 0;
    };


    /**
     * @brief 
     * 
     */
    struct IMUInfo
    {
        IMUInfo(){}

        //IMUInfo(){}

        float pitch;
        float yaw;
        // float row;

        // float vx;
        // float vy;
        // float vz;

        // float wx;
        // float wy;
        // float wz;
    };

} // namespace hnurm

#endif // DATATYPE_H