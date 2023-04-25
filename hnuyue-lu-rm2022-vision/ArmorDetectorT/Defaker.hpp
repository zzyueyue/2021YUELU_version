#ifndef DEFAKER_HPP
#define DEFAKER_HPP

#include<opencv2/core.hpp>
#include<opencv2/ml.hpp>
#include<opencv2/calib3d.hpp>
#include<opencv2/imgproc.hpp>
#include<memory>
#include"Data/DataType.hpp"

#define SVM_CLASSIFIER // LENET5


namespace hnurm
{
    class Defaker 
    {
    public:

        /**
         * @brief Construct a new Defaker object,load svm;
         * 
         */
        Defaker():svm(cv::ml::SVM::create())
        {
            _vector_size=cv::Size(40,40);
            //svm->load("/path to trained model");
        }

        /**
         * @brief Destroy the Defaker object
         */
        ~Defaker()=default;

        /**
         * @brief public API of Defaker,cut ROI of possible armor then use SVM 
         *        to tell whether it's an real armor
         * @param origin_img raw frame get from camera
         * @param box possible armor
         * @return int the number pattern in the armor
         */
        int Defake(cv::Mat& origin_img,TargetInfo& target)
        {
            // Crop();
            // GetTransMat();
            // Transform();
            // PreTreat();
            // GetTrait();
            return Match();
        }
    
    private:

        /**
         * @brief 
         * 
         */
        void Crop();

        /**
         * @brief Get tranform mat via cv::getPerspectiveTransform()
         * @param raw_box an Armor object
         */
        void GetTransMat()
        {

        }

        /**
         * @brief use transform_mat to multiply the origin img
         *        to get a vector_size img
         * @param private member
         */
        void Transform(void)
        {
            
        }
        
        /**
         * @brief use cvtColor to get bi-value ima
         *        for svm->predicat()
         */
        void PreTreat(void)
        {

        }

        /**
         * @brief Get the Trait vector via resize
         * @param private_member 
         */
        void GetTrait(void)
        {

        }

        /**
         * @brief use trained svm to match trait vector
         * 
         * @return int  number in the armor
         */
        int Match(void)
        {
            return 1;
        }

    private:

#ifdef SVM_CLASSIFIER
        std::unique_ptr<cv::ml::SVM> svm;
#endif // SVM_CLASSIFIER

#ifdef LENET5

#endif // LENET5

    private:

        cv::Size _vector_size;  //size of trait vetor
        cv::Mat _origin_img;    //origin img
        cv::Mat _transform_mat; //Mat for transformation
        cv::Mat _remaped_mat;   //Mat remaped
        cv::Mat _binary_img;     //img after threshold
        cv::Mat _trait_vector;  //trait vetor
    };

}//hnurm
#endif // !DEFAKER_HPP