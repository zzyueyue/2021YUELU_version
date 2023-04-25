/**
 * @file Serialcodec.h
 * @author modify by NeoZng (neozng1@hnu.edu.cn)
 * @brief 
 * @version 0.2
 * @date 2021-08-19
 * 
 * @copyright Copyright HNUYueLuRM-Vision (c) 2021
 * @todo 1.test recv API,data_len
 *       2.optimize the logic of whole recv function
 */


/* kiko@idispace.com 2021.01 */

//#define SERIAL_CODEC_DEBUG

/* SerialCodec API
 * 1. SerialCodec(int usb_id);
 * @Brief: open serial_port "/dev/ttyUSB(usb_id)"
 *
 * 2. SerialCode(std::string device_name);
 * @Brief: open serial_port "device_name"
 *
 * 3. bool try_get_recv_data_for(Protocol::VisionRecvData& recv_data, int milli_secs = 10);
 * @Brief: try to retrieve data from serial port within mill_secs milliseconds
 * @return: if data has been fetched and decoded properly, then return true, otherwise return false
 * @NOTICE: this method will block the process for at most mill_secs milliseconds
 *
 * 4. bool send_data(int cam_id, float pitch, float yaw, float distance);
 * @Brief: encode and send data to the serial port
 * @return: true if data sent succeed
*/


#include "Protocol/protocol.h"
#include "serial.h"
#include "Data/DataType.hpp"

#include <chrono>
#include <exception>

namespace hnurm
{

// Read & Write from or to the serial port
// Ensure integrity of receieved data pack thourgh this
// wrappered class combined with Serial and Protocol
class SerialCodec: public Serial, public Protocol
{
public:
    SerialCodec():enemy_color(-1){}

public:
    // may get broke msg or don't get enough msg to decode, then return false
    // if both retrieve data and decode work fine, then return true
    // by defualt wait for up to 10 milli-secs (tests shows that 3ms is enough to work)
    bool try_get_recv_data_for(VisionRecvData& recv_data,IMUInfo &angle_data, int milli_secs = 10);

    // encode and send data to the serial port
    bool send_data(bool shoot, float pitch, float yaw);

    bool SetColor(void);   //set color when initialize

    int GetColor(void);
     
    double GetSpeed(VisionRecvData recv_data);



private:
    int enemy_color = -1;  //used for init color

};

} // namespace hnurm


