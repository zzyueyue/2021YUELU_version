#ifndef ARMOR_DETECTOR_T
#define ARMOR_DETECTOR_T


#define PREPROCESS_DEBUG
#define LIGHTBAR_DEBUG
#define ARMOR_DEBUG
#define TARGET_DEBUG

#include"Detector.hpp"
#include"Defaker.hpp"

namespace hnurm
{
    class ArmorDetector : Detector
    {
    public:

        ArmorDetector();

        ~ArmorDetector()=default;

        /**
         * @brief
         *
         * @param raw_frame
         * @param target
         * @return int
         */
        int

        Detect(const ImgInfo& raw_frame,TargetInfo& target);


    private:

        /**
         * @brief
         *
         */
        void Preprocess();

        /**
         * @brief Create a Bars object
         *
         */
        void CreateBars();

        /**
         * @brief Create a Target object
         *
         */
        void CreateTarget(const cv::RotatedRect& rect1,const cv::RotatedRect& rect2,TargetInfo& target);

        /**
         * @brief
         *
         * @return int
         */
        int PairBars(TargetInfo& target);

        /**
         * @brief
         *
         */
        void Reset();

    private:

        Defaker _defaker;

        cv::Mat _raw_img;
        cv::Mat _carrier;
        cv::Mat _kernel;

        std::vector<std::vector<cv::Point> > _contours;
        cv::RotatedRect _tmp_rect;
        std::vector<cv::RotatedRect> _light_bars;
        std::vector<TargetInfo> _armors;



#ifdef ARMOR_DETECTOR_DEBUG
        cv::Mat 
        cv::Mat _canvas;
#endif // ARMOR_DETECTOR_DEBUG

    }; // ArmorDetectorT

} // namespace hnurm

#endif // !ARMOR_DETECTOR_T