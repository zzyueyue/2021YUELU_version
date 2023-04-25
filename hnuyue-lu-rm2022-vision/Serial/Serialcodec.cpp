#define MAX_DATA_LENGTH 100
#define MIN_DATA_LENGTH 0

#include "Serialcodec.h"
using namespace std;

namespace hnurm
{
/**
 * @todo change the params of this function,use a struct to store params instead of pass then one by one
 */
// encode and send data to the serial port
// return true if send succeedd, otherwise return false
bool SerialCodec::send_data(bool shoot, float pitch, float yaw)
{
    //visionSendData should be modified
    auto data = VisionSendData(shoot, pitch, yaw);
    auto data_str = Protocol::encode(data);
    return Serial::send(data_str) > 0;
}


double SerialCodec::GetSpeed(VisionRecvData recv_data)
{
    if((int)recv_data.speed==(int)BulletSpeed::b10)
    return 10;
    if((int)recv_data.speed==(int)BulletSpeed::b16)
    return 16;
    if((int)recv_data.speed==(int)BulletSpeed::s15)
    return 15;
    if((int)recv_data.speed==(int)BulletSpeed::s18)
    return 18;
    if((int)recv_data.speed==(int)BulletSpeed::s30)
    return 30;
    else
    return 0;
    
}

// try receive a valid package within milli_secs
// and decode it to a Protocol::Vision_recv_data
bool SerialCodec::try_get_recv_data_for(VisionRecvData& recv_data,IMUInfo& angle_data, int milli_secs)
{
    try{
    auto start = clk::now();

    volatile static int data_len = 10;   // default 10 bits,
                                 // auto detect string stream from serialport and update
                                 // no need for handcraft modification

    static bool pack_start = false;
    static std::string recv_buf = "";  // receive buffer
    static std::string tmp_str;        // receive some data from each recv
    int find_pos = std::string::npos;  // not find by defualt

#ifdef SERIAL_CODEC_DEBUG
    static int i = 0;
#endif

    tmp_str.clear();

    while (1)
    {
        // step1: try to receive str from serial port for up to milli_secs milliseconds
        if (Serial::try_recv_for(tmp_str, milli_secs))
        {
            find_pos = tmp_str.find_first_of(static_cast<char>(PROTOCOL_CMD_ID));

            // step1: handle newly received data string

            // package has not started && find frame header : 0XA5
            if (!pack_start && find_pos != std::string::npos)
            {
                pack_start = true;
                recv_buf = tmp_str.substr(find_pos);
                // grab from the header identifier 0XA5 to the end
            }
            else if (!pack_start && find_pos == std::string::npos)
            {
                auto end = clk::now();
                auto gap = std::chrono::duration_cast<Ms>(end - start);
                if (gap.count() > milli_secs)
                {
                    break;
                }
                // package not started and didn't find identifier
                // just pass and try to get some new data
                continue;
            }
            else if(pack_start && find_pos == std::string::npos)
            {
                // pack started and received data of this pack, just append them
                recv_buf.append(tmp_str);
            }
            else if (pack_start && find_pos != std::string::npos)
            {
                // pack started and find another frame
                // just grab the front data before next frame
                recv_buf.append(tmp_str.substr(0, find_pos));
            }

            // step2: update data_len
            if (recv_buf.size() > 3)
            {
                // update data_len if valid
                auto new_data_len = static_cast<int>((recv_buf[2]<<8) | recv_buf[1]); //从收到的整包中解出包的数据长度
                auto new_full_pack_len = new_data_len + OFFSET_BYTE;     //整包长度,offset_byte是帧头帧尾和校验位的总长度,详见protocol的说明文档
                if ((new_full_pack_len >= MIN_DATA_LENGTH) && (new_full_pack_len <= MAX_DATA_LENGTH)) //规定的包长度        
                {

                    data_len = new_full_pack_len;
    #ifdef SERIAL_CODEC_DEBUG
                    std::cout << "new_full_pack_len" << new_full_pack_len << std::endl;
                    std::cout << "data_len = " << data_len << std::endl;
    #endif
                }
    #ifdef SERIAL_CODEC_DEBUG
                // debug
                std::cout << "outer data_len = " << data_len << std::endl;
    #endif
            }

            // step3: check data integrity
            if (recv_buf.size() >= data_len)
            {
                if (decode(recv_buf,recv_data,angle_data))
                {
                    pack_start = false;
                    recv_buf.clear();
                    return true;
                }
                else
                {
                    pack_start = false;
                    recv_buf.clear();
                    auto end = clk::now();
                    auto gap = std::chrono::duration_cast<Ms>(end - start);
                    if (gap.count() > milli_secs)
                    {
                        break;
                    }
                }
            }

        }
        auto end = clk::now();
        auto gap = std::chrono::duration_cast<Ms>(end - start);
        if (gap.count() > milli_secs)
        {
#ifdef SERIAL_CODEC_DEBUG
            std::cout << "times:" << i++ << " gap.count(): " << gap.count() << std::endl;
#endif
            break;
        }
    }

    return false;

    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}



bool SerialCodec::SetColor(void)
{
    VisionRecvData a_pack_for_color(SelfColor::none,0,WorkMode::none,BulletSpeed::none);
    IMUInfo abs_angle;
    int over_times=0;
    
    try_get_recv_data_for(a_pack_for_color,abs_angle);
    while(a_pack_for_color.self_color!=SelfColor::red && a_pack_for_color.self_color!=SelfColor::blue)
    {
        over_times++;
        //cout<<over_times<<endl;
        //std::cout<<"color" << (int)a_pack_for_color.self_color<<std::endl;
        try_get_recv_data_for(a_pack_for_color,abs_angle);

    }
    std::cout<<"[Serial]::get color successed!"<<std::endl;


    if((int)a_pack_for_color.self_color==(int)SelfColor::red)
    {
        enemy_color=1;
    }
    else if ((int)a_pack_for_color.self_color==(int)SelfColor::blue)
    {
        enemy_color=0;
    }
    return true;
}

int SerialCodec::GetColor(void)
{
    return enemy_color;
}


}//namespace hnurm
