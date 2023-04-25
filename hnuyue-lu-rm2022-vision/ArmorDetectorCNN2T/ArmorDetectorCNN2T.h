//
// Created by rm on 2022/5/15.
//

#ifndef TOGETHER_TEST_H
#define TOGETHER_TEST_H

#endif //TOGETHER_TEST_H
#include "Nanodet/nanodet_openvino.h"
#include "ArmorDetectorT/ArmorDetectorT.h"
#include "Data/DataType.hpp"
#include "IndustrialCamera/Camera.h"
#include "AngleSolver/AngleSolver.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#define TARGET_RED 0
#define TARGET_BLUE 1

using namespace cv;
struct object_rect{
    int x;
    int y;
    int width;
    int height;
};
const int color_list[80][3] =
        {
                //{255 ,255 ,255}, //bg
                {216 , 82 , 24},
                {236 ,176 , 31},
        };
/**
 *
 * @param src
 * @param dst
 * @param dst_size
 * @param effect_area
 * @return
 */
int resize_uniform(cv::Mat& src, cv::Mat& dst, cv::Size dst_size, object_rect& effect_area);

/**
 *
 * @param bgr
 * @param bboxes
 * @param effect_roi
 */
void draw_bboxes(const cv::Mat& bgr, const std::vector<BoxInfo>& bboxes, object_rect effect_roi);

/**
 * @brief 选择距离中心最近的装甲板
 * @param all_armors
 * @param image
 * @param color_label
 * @return
 */
int ChooseArmor(std::vector<BoxInfo> &all_armors,cv::Mat image,int color_label);

/**
 *
 * @param raw_image
 * @param effect_area
 * @param box
 * @param cropped_image
 * @return
 */
bool CropImage(const cv::Mat raw_image,const object_rect effect_area,BoxInfo box,cv::Mat &cropped_image, cv::Point2f &LeftTop);


/**
 *
 * @param detector
 * @param color_label
 * @param cam_id
 * @return
 */
int webcam_demo(NanoDet& detector,hnurm::ImgInfo frame,hnurm::ArmorDetector &armor_detector, int color_label, hnurm::TargetInfo &target);

/**
 *
 * @param detector
 * @param color_label
 * @param path
 * @return
 */
int video_demo(NanoDet& detector,int color_label, const char* path, hnurm::TargetInfo &target);

