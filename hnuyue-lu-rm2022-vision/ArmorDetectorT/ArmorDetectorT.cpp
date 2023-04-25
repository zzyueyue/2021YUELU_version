#include "ArmorDetectorT.h"
using namespace std;
namespace hnurm
{
    ArmorDetector::ArmorDetector()
    {
        // _kernel=cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(8,8));
    }

    int ArmorDetector::Detect(const ImgInfo &raw_frame, TargetInfo &target)
    {
        _contours.clear();
        _light_bars.clear();
        _armors.clear();
        _carrier = raw_frame.img;
        _raw_img = _carrier;
        target.shoot = 0;
        Preprocess();
        CreateBars();
        if (_light_bars.size() >= 2)
        {
            if(_light_bars.size()==2)
                CreateTarget(_light_bars[0], _light_bars[1], target);
            else
            {
                PairBars(target);
            }
        }


        ArmorDetector::Reset();
        // int nums=PairBars();
        // ChooseTarget();
        return 1;
    }

    int ArmorDetector::PairBars(TargetInfo& target)
    {
        //vars for judgement
        double height_ratio,width_ratio,area_ratio,angle_diff,center_xdiff_ratio,center_ydiff_ratio;
        //these two vars are designed for improving efficiency
        auto n=_light_bars.size();
        for(auto i=0;i<n-1;i++)
        {
            for(auto j=i+1;j<n;j++)
            {
                
                if((_light_bars[i].angle>2 && _light_bars[j].angle<178) ||
                    _light_bars[i].angle<178 && _light_bars[j].angle>2)
                {
                    continue;
                }
                else if(abs(_light_bars[i].angle-_light_bars[j].angle)>12)
                {
                    continue;
                }

                height_ratio=_light_bars[i].size.height/_light_bars[j].size.height;
                //make a experiment to tell which is faster
                //maybe the later one if better because of the prediction of streamline process
                //height_ratio=height_ratio>1?height_ratio:1/height_ratio;
                if(height_ratio>1.8 || height_ratio<0.55)
                {
                    continue;
                }

                width_ratio=_light_bars[i].size.width/_light_bars[j].size.width;
                //make a experiment to tell which is faster
                //width_ratio=width_ratio>1?width_ratio:1/width_ratio;
                if(width_ratio>5 || width_ratio<0.2)
                {
                    continue;
                }
                
                // should this be a criticize condition? think over
                // area_ratio=_light_bars[i].size.area()/_light_bars[j].size.area();
                // //area_ratio=area_ratio>1?area_ratio:1/area_ratio;
                // if(area_ratio>3)

                //modify this judgement,how to decompose them correctly?
                //center_diff_ratio=abs(_light_bars[i].center.x-_light_bars[j].center.x)/

                center_ydiff_ratio=abs(_light_bars[i].center.y-_light_bars[j].center.y)/_light_bars[i].size.height;
                
                if(center_ydiff_ratio>4.2 || center_ydiff_ratio<0.24)
                {
                    continue;
                }
  
                center_xdiff_ratio=abs(_light_bars[i].center.x-_light_bars[j].center.x)/_light_bars[i].size.height;
                
                if(center_xdiff_ratio>6 || center_xdiff_ratio<0.5)
                {
                    continue;
                }
                CreateTarget(_light_bars[i],_light_bars[j],target);
                return 1;
            } // for 2
        } // for 1
        return 0;
        
    }

    void ArmorDetector::Preprocess()
    {
        // get grey
        cv::cvtColor(_carrier, _carrier, cv::COLOR_BGR2GRAY);
        // binarization processing
        double aa = cv::threshold(_carrier, _carrier, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        //        std::cout<<aa<<std::endl;
        //        cv::threshold(_carrier,_carrier,200,255,cv::THRESH_BINARY);

         cv::imshow("2",_carrier);
    }

    void ArmorDetector::CreateBars()
    {
        cv::Mat _canvas = cv::Mat::zeros(_carrier.size(), CV_8UC1);
        cv::findContours(_carrier, _contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        for (auto i = 0; i < _contours.size(); i++)
        {
            for (auto j = 0; j < _contours[i].size(); j++)
            {
                circle(_canvas, _contours[i][j], 0.1, cv::Scalar(255));
            }
        }
        for (int i = 0; i < _contours.size(); i++)
        {
            double area = cv::contourArea(_contours[i]);
            if (area < 10 || area > 20000)
                continue;
            if (_contours[i].size() < 5)
                continue;
            _tmp_rect = cv::fitEllipse(_contours[i]);
            if (_tmp_rect.size.height / _tmp_rect.size.width > 10 || _tmp_rect.size.area() < 50 || _tmp_rect.size.area() > 3000 || _tmp_rect.size.height / _tmp_rect.size.width < 1.5 ||
                (35 < _tmp_rect.angle && _tmp_rect.angle < 145))
                continue;
            _light_bars.push_back(_tmp_rect);
        }
    }

    void ArmorDetector::CreateTarget(const cv::RotatedRect &rect1, const cv::RotatedRect &rect2, TargetInfo &target)
    {
        cv::Point2f v1[4];
        cv::Point2f v2[4];
        if (rect1.angle > 100 && rect2.angle > 100)
        {
            cv::Point2f v3[4];
            rect1.points(v3);
            cv::Point2f v4[4];
            rect2.points(v4);
            if (v3[1].x > v4[1].x)
            {
                rect1.points(v1);
                rect2.points(v2);
            }
            else
            {
                rect1.points(v2);
                rect2.points(v1);
            }
            target.vertexes.push_back(v2[0]);
            target.vertexes.push_back(v1[3]);
            target.vertexes.push_back(v1[2]);
            target.vertexes.push_back(v2[1]);
            //            std::cout<<v2[0]<<" "<<v1[3]<<" "<<v1[2]<<" "<<v2[1]<<std::endl;
            target.DrawTarget(_raw_img);
        }
        else if (rect1.angle < 80 && rect2.angle < 80)
        {
            cv::Point2f v3[4];
            rect1.points(v3);
            cv::Point2f v4[4];
            rect2.points(v4);
            if (v3[3].x > v4[3].x)
            {
                rect1.points(v1);
                rect2.points(v2);
            }
            else
            {
                rect1.points(v2);
                rect2.points(v1);
            }
            target.vertexes.push_back(v2[2]);
            target.vertexes.push_back(v1[1]);
            target.vertexes.push_back(v1[0]);
            target.vertexes.push_back(v2[3]);
            //            std::cout<<v2[2]<<" "<<v1[1]<<" "<<v1[0]<<" "<<v2[3]<<std::endl;
            target.DrawTarget(_raw_img);
        }
        else if (rect1.angle > 100 && rect2.angle < 80)
        {

            cv::Point2f v3[4];
            rect1.points(v3);
            cv::Point2f v4[4];
            rect2.points(v4);
            if (v3[1].x > v4[3].x)
            {
                rect1.points(v1);
                rect2.points(v2);
                target.vertexes.push_back(v2[2]);
                target.vertexes.push_back(v1[3]);
                target.vertexes.push_back(v1[2]);
                target.vertexes.push_back(v2[3]);
                //                std::cout<<v2[2]<<" "<<v1[3]<<" "<<v1[2]<<" "<<v2[3]<<std::endl;
                target.DrawTarget(_raw_img);
            }
            else if (v3[1].x < v4[3].x)
            {
                rect1.points(v2);
                rect2.points(v1);
                target.vertexes.push_back(v2[0]);
                target.vertexes.push_back(v1[1]);
                target.vertexes.push_back(v1[0]);
                target.vertexes.push_back(v2[1]);
                //                std::cout<<v2[0]<<" "<<v1[1]<<" "<<v1[0]<<" "<<v2[1]<<std::endl;
                target.DrawTarget(_raw_img);
            }
        }
        else if (rect1.angle < 80 && rect2.angle > 100)
        {
            cv::Point2f v3[4];
            rect1.points(v3);
            cv::Point2f v4[4];
            rect2.points(v4);
            if (v3[1].x > v4[3].x)
            {
                rect1.points(v1);
                rect2.points(v2);
                target.vertexes.push_back(v2[0]);
                target.vertexes.push_back(v1[1]);
                target.vertexes.push_back(v1[0]);
                target.vertexes.push_back(v2[1]);
                //                std::cout<<v2[0]<<" "<<v1[1]<<" "<<v1[0]<<" "<<v2[1]<<std::endl;
                target.DrawTarget(_raw_img);
            }
            else if (v3[1].x < v4[3].x)
            {
                rect1.points(v2);
                rect2.points(v1);
                target.vertexes.push_back(v2[2]);
                target.vertexes.push_back(v1[3]);
                target.vertexes.push_back(v1[2]);
                target.vertexes.push_back(v2[3]);
                //                std::cout<<v2[2]<<" "<<v1[3]<<" "<<v1[2]<<" "<<v2[3]<<std::endl;
                target.DrawTarget(_raw_img);
            }
        }
        // cv::imshow("draw_armor_area",_raw_img);
    }

    void ArmorDetector::Reset()
    {
        _contours.clear();
        _light_bars.clear();
        _armors.clear();
    }

} // namespace hnurm
