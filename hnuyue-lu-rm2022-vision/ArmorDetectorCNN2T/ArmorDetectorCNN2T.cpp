//
// Created by rm on 2022/5/15.
//
//#pragma once
#include "IndustrialCamera/Camera.h"
#include "ArmorDetectorCNN2T/ArmorDetectorCNN2T.h"
#include "Data/DataType.hpp"

int resize_uniform(cv::Mat& src, cv::Mat& dst, cv::Size dst_size, object_rect& effect_area)
{
    int w = src.cols;
    int h = src.rows;
    int dst_w = dst_size.width;
    int dst_h = dst_size.height;
    //std::cout << "src: (" << h << ", " << w << ")" << std::endl;
    dst = cv::Mat(cv::Size(dst_w, dst_h), CV_8UC3, cv::Scalar(0));

    float ratio_src = w * 1.0 / h;
    float ratio_dst = dst_w * 1.0 / dst_h;

    int tmp_w = 0;
    int tmp_h = 0;
    if (ratio_src > ratio_dst)
    {
        tmp_w = dst_w;
        tmp_h = floor((dst_w * 1.0 / w) * h);
    }
    else if (ratio_src < ratio_dst)
    {
        tmp_h = dst_h;
        tmp_w = floor((dst_h * 1.0 / h) * w);
    }
    else
    {
        cv::resize(src, dst, dst_size);
        effect_area.x = 0;
        effect_area.y = 0;
        effect_area.width = dst_w;
        effect_area.height = dst_h;
        return 0;
    }

    //std::cout << "tmp: (" << tmp_h << ", " << tmp_w << ")" << std::endl;
    cv::Mat tmp;
    cv::resize(src, tmp, cv::Size(tmp_w, tmp_h));

    if (tmp_w != dst_w)
    {
        int index_w = floor((dst_w - tmp_w) / 2.0);
        //std::cout << "index_w: " << index_w << std::endl;
        for (int i = 0; i < dst_h; i++)
        {
            memcpy(dst.data + i * dst_w * 3 + index_w * 3, tmp.data + i * tmp_w * 3, tmp_w * 3);
        }
        effect_area.x = index_w;
        effect_area.y = 0;
        effect_area.width = tmp_w;
        effect_area.height = tmp_h;
    }
    else if (tmp_h != dst_h) {
        int index_h = floor((dst_h - tmp_h) / 2.0);
        //std::cout << "index_h: " << index_h << std::endl;
        memcpy(dst.data + index_h * dst_w * 3, tmp.data, tmp_w * tmp_h * 3);
        effect_area.x = 0;
        effect_area.y = index_h;
        effect_area.width = tmp_w;
        effect_area.height = tmp_h;
    }
    else
    {
        printf("error\n");
    }
    //cv::imshow("dst", dst);
    //cv::waitKey(0);
    return 0;
}

void draw_bboxes(const cv::Mat& bgr, const std::vector<BoxInfo>& bboxes, object_rect effect_roi)
{
    static const char* class_names[] = { "red","blue"};

    cv::Mat image = bgr.clone();
    int src_w = image.cols;
    int src_h = image.rows;
    int dst_w = effect_roi.width;
    int dst_h = effect_roi.height;
    float width_ratio = (float)src_w / (float)dst_w;
    float height_ratio = (float)src_h / (float)dst_h;


    for (size_t i = 0; i < bboxes.size(); i++)
    {
        const BoxInfo& bbox = bboxes[i];
        cv::Scalar color = cv::Scalar(color_list[bbox.label][0], color_list[bbox.label][1], color_list[bbox.label][2]);
        //fprintf(stderr, "%d = %.5f at %.2f %.2f %.2f %.2f\n", bbox.label, bbox.score,
        //    bbox.x1, bbox.y1, bbox.x2, bbox.y2);

        cv::rectangle(image, cv::Rect(cv::Point((bbox.x1 - effect_roi.x) * width_ratio, (bbox.y1 - effect_roi.y) * height_ratio),
                                      cv::Point((bbox.x2 - effect_roi.x) * width_ratio, (bbox.y2 - effect_roi.y) * height_ratio)), color);

        char text[256];
        sprintf(text, "%s %.1f%%", class_names[bbox.label], bbox.score * 100);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseLine);

        int x = (bbox.x1 - effect_roi.x) * width_ratio;
        int y = (bbox.y1 - effect_roi.y) * height_ratio - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > image.cols)
            x = image.cols - label_size.width;

        cv::rectangle(image, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                      color, -1);

        cv::putText(image, text, cv::Point(x, y + label_size.height),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255));
    }

    //cv::imshow("image", image);
}

// 选择距离中心最近的装甲板
int ChooseArmor(std::vector<BoxInfo> &all_armors,cv::Mat image,int color_label)
{
    if(all_armors.size() == 0)
        return -1;
    for(auto  it = all_armors.begin();it !=all_armors.end();)
    {
        if(it->label != color_label)
        {
            it=all_armors.erase(it);
        }
        else
        {
            ++it;
        }
    }
    int _src_w = image.cols;
    int _src_h = image.rows;
    Point2f _center;
    _center.x=_src_w/2;
    _center.y=_src_h/2;
    int distance=10000000;
    int tmp_distance=9999999;
    int index = -1;
    for(int i=0;i< all_armors.size();i++)
    {
        tmp_distance=pow((all_armors[i].x1+all_armors[i].x2)/2-_center.x,2)+pow((all_armors[i].y1+all_armors[i].y2)/2-_center.y,2);
        if(tmp_distance<distance)
        {
            distance=tmp_distance;
            index=i;
        }
    }
    return index;
}

bool CropImage(const cv::Mat raw_image,const object_rect effect_area,BoxInfo box,cv::Mat &cropped_image,cv::Point2f &LeftTop)
{
    int _src_w = raw_image.cols;
    int _src_h = raw_image.rows;
    int _dst_w = effect_area.width;
    int _dst_h = effect_area.height;
    float _width_ratio = (float)_src_w / (float)_dst_w;
    float _height_ratio = (float)_src_h / (float)_dst_h;
    box.x1-=(box.x2-box.x1)/6;
    box.y1-=(box.y2-box.y1)/8;
    box.x2+=(box.x2-box.x1)/6;
    box.y2+=(box.y2-box.y1)/8;
    LeftTop = cv::Point((box.x1 - effect_area.x)>0?(box.x1 - effect_area.x) * _width_ratio:0*_width_ratio,
                        (box.y1 - effect_area.y)>0?(box.y1 - effect_area.y)* _height_ratio:0 * _height_ratio);
    cv::Rect rect(cv::Point((box.x1 - effect_area.x)>0?(box.x1 - effect_area.x) * _width_ratio:0*_width_ratio,
                            (box.y1 - effect_area.y)>0?(box.y1 - effect_area.y)* _height_ratio:0 * _height_ratio),
                  cv::Point((box.x2 - effect_area.x) * _width_ratio  > _src_w ? _src_w :(box.x2 - effect_area.x) * _width_ratio,
                            (box.y2 - effect_area.y) * _height_ratio > _src_h ? _src_h :(box.y2 - effect_area.y) * _height_ratio));
    if(rect.area()<=0)
        return false;
    cv::Mat image1 = raw_image.clone();
    cropped_image = image1(rect);
    return true;
}


int webcam_demo(NanoDet& detector,hnurm::ImgInfo frame,hnurm::ArmorDetector &armor_detector, int color_label, hnurm::TargetInfo &target)
{
    //hnurm::TargetInfo temp;
    float distance,yaw,pitch;
    cv::Mat image;
    //cv::imshow("nice",frame.img);
//    cv::VideoCapture cap(cam_id);

    int height = detector.input_size[0];
    int width = detector.input_size[1];

        image = frame.img;
        double t = (double)cv::getTickCount();//开始计时
        target.vertexes.clear();
        object_rect effect_roi;
        cv::Mat resized_img;
        resize_uniform(image, resized_img, cv::Size(width, height), effect_roi);
        auto results = detector.detect(resized_img, 0.4, 0.5);
        //std::cout<<"results.size()"<<results.size()<<std::endl;
        //std::cout<<color_label<<std::endl;
        int index=ChooseArmor(results,image,color_label);
        if(index != -1)
        {
            cv::Mat new_image ;
            cv::Point2f LeftTop;
            bool _flag = CropImage(image,effect_roi,results[index],new_image, LeftTop);
            if (_flag)
            {
                cv::namedWindow("croped_image", cv::WINDOW_NORMAL);
                cv::imshow("croped_image",new_image);
                //找四个点
                if(!new_image.empty())
                {
                    frame.img = new_image;
                    //std::cout<<"dian="<<results[index].x1<<" "<<results[index].y1<<std::endl;
                    armor_detector.Detect(frame, target);
                    if (target.vertexes.size() == 4)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            target.vertexes[i].x += LeftTop.x;
                            target.vertexes[i].y += LeftTop.y;
                            //std::cout << "[" << target.vertexes[i].x << "," << target.vertexes[i].y << "]  ";
                        }
                        if((target.vertexes[1].x-target.vertexes[0].x)/(target.vertexes[3].y-target.vertexes[0].y)<2.4)
                            target._big_flag = 0;
                        else
                            target._big_flag = 1;
                        //std::cout << std::endl;
                        target.shoot=1;
                        //target.DrawTarget(frame.img);
                        cv::imshow("target",frame.img);
                        //waitKey(1);
                    }
                }
            }
            else{
                return 0;
                std::cout << "have no armor area" << std::endl;
            }
        }
        else{
                return 0;
                std::cout << "have no armor area" << std::endl;
            }

        draw_bboxes(image, results, effect_roi);

        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();//结束计时
        double fps = int(1.0 / t);
        /*char str[20];
        std::string FPSstring("FPS:");
        sprintf(str, "%.2f", fps);
        FPSstring += str;
        putText(image, FPSstring,Point(5, 20),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));//将帧率显示在图片上

        cv::namedWindow("showFPS", cv::WINDOW_NORMAL);
        cv::imshow("showFPS",image);
        cout<<"FPS"<<fps<<endl;

        if(cv::waitKey(1)=='q')
            exit(-1);
        */

    return 1;
}

int video_demo(NanoDet& detector,int color_label, const char* path, hnurm::TargetInfo &target)
{

    hnurm::ArmorDetector armor_detector;
    hnurm::ImgInfo frame;
    //hnurm::TargetInfo temp;

    cv::Mat image;
    cv::VideoCapture cap(path);

    int height = detector.input_size[0];
    int width = detector.input_size[1];

    while (true)
    {
        double t = (double)cv::getTickCount();//开始计时
        //target.vertexes.clear();
        cap >> image;
        if (image.empty())
        {
            printf("测试视频结束\n");
            break;
        }

        object_rect effect_roi;
        cv::Mat resized_img;
        resize_uniform(image, resized_img, cv::Size(width, height), effect_roi);
        auto results = detector.detect(resized_img, 0.45, 0.55);
        //std::cout<<results.size()<<std::endl;
        int index=ChooseArmor(results,image,color_label);

        if(results.size()!=0)
        {
            cv::Mat new_image ;
            cv::Point2f LeftTop;
            if (CropImage(image,effect_roi,results[index],new_image, LeftTop))
            {
                //cv::namedWindow("croped_image", cv::WINDOW_NORMAL);
                //cv::imshow("croped_image",new_image);
                //找四个点
                if(!new_image.empty()) {
                    frame.img = new_image;
                    armor_detector.Detect(frame, target);
//                    std::cout<<"size="<<target.vertexes.size()<<std::endl;
                    if(target.vertexes.size() == 4)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            target.vertexes[i].x += LeftTop.x;
                            target.vertexes[i].y += LeftTop.y;
                            //std::cout << "[" << target.vertexes[i].x << "," << target.vertexes[i].y << "]  ";
                        }
                       // std::cout << std::endl;
                        //target.DrawTarget(image);
                    }
                }
            }
            else
                std::cout << "have no armor area" << std::endl;
        }
        else
            std::cout<<"have no target"<<std::endl;
        draw_bboxes(image, results, effect_roi);

        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();//结束计时
        double fps = int(1.0 / t);
        char str[20];
        std::string FPSstring("FPS:");
        sprintf(str, "%.2f", fps);
        FPSstring += str;
        putText(image, FPSstring,Point(5, 20),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));//将帧率显示在图片上
//        cv::namedWindow("showFPS", cv::WINDOW_NORMAL);
//        cv::imshow("showFPS",image);
        //cv::waitKey(1);
    }
    return 0;
}

