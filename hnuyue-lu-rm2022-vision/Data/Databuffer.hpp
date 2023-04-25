/* kiko@idiospace.com 2020.01.20 */

#ifndef DataBuffer_HPP
#define DataBuffer_HPP

#include <mutex>
#include <chrono>
#include <vector>
#include <memory>
#include <exception>
#include <iostream>

#define DATABUFFER_DEBUG


namespace hnurm
{

    template <typename DataType>
    class DataBuffer
    {
    protected:
        using Ms = std::chrono::milliseconds;
        using validator = bool (*) (const DataType&);  // validate data func

    public:
        DataBuffer(int size=1);
        ~DataBuffer() = default;
        bool Get(DataType& data, validator v=nullptr);
        bool Update(const DataType& data, validator v=nullptr);

    private:
        std::vector<DataType> data_buf;
        std::vector<std::timed_mutex> mtx_buf;
        int head_idx, tail_idx;
    };

    template <class DataType>
    DataBuffer<DataType>::DataBuffer(int size):
            head_idx(0),
            tail_idx(0),
            data_buf(size),
            mtx_buf(size){}

    template <class DataType>
    bool DataBuffer<DataType>::Get(DataType& data, validator v)
    {
        try
        {
            const int cur_head_idx = head_idx;

            if (mtx_buf[cur_head_idx].try_lock_for(Ms(2)))  // lock succeed
            {
                if ((v == nullptr ? true: v(data_buf[cur_head_idx]))) // check data integrity if validator is given
                {
                    data = data_buf[cur_head_idx];
                    mtx_buf[cur_head_idx].unlock();
                    return true;
                }
            }
            else
            {
#ifdef DATABUFFER_DEBUG
                std::cout << "[get] failed to lock" << std::endl;
#endif // DATABUFFER_DEBUG
            }

            mtx_buf[cur_head_idx].unlock();
            return false;
        }
        catch (std::exception& e)
        {
#ifdef DATABUFFER_DEBUG
            std::cerr << e.what() << std::endl;
#endif //DATABUFFER_DEBUG
        }
    }



    template <class DataType>
    bool DataBuffer<DataType>::Update(const DataType& data, validator v)
    {
        try{
            if (v!=nullptr && !v(data))
                return false;

            const int new_head_idx = (head_idx + 1) % data_buf.size();

            if (mtx_buf[new_head_idx].try_lock_for(Ms(2)))  // lock succeed
            {
                data_buf[new_head_idx] = data;
                if (new_head_idx == tail_idx)
                {
                    tail_idx = (tail_idx + 1) % data_buf.size();
                }
                head_idx = new_head_idx;
                mtx_buf[new_head_idx].unlock();
                return true;
            }
            else
            {
#ifdef DATABUFFER_DEBUG
                std::cout << "[update] failed to lock" << std::endl;
#endif // DATABUFFER_DEBUG
            }
            return false;
        }
        catch (std::exception& e)
        {
#ifdef DATABUFFER_DEBUG
            std::cerr << e.what() << std::endl;
#endif //DATABUFFER_DEBUG
        }
    }

}// class DataBufferfer 
#endif // DataBuffer_HPP
