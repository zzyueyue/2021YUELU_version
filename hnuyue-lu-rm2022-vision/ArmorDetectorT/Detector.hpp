
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>

#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<math.h>

#include"Data/DataType.hpp"

namespace hnurm
{   
    /**
     * @brief this is a abstract base class for building all kinds of detecotr
     * 
     */
    class Detector
    {

    public:
        

        Detector()=default;

        ~Detector()=default;

        virtual int Detect(const ImgInfo& raw_frame,TargetInfo& target)=0;

        void SetColor(EnemyColor color)
        {
            _target_color=color;
        }

    private:



    private:
        
        EnemyColor _target_color;


    };// Detctor base

} // namespace hnurm