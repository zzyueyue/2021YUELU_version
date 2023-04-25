#include <iostream>
#include <thread>
#include <pthread.h>
#include <csignal>  //
#include <unistd.h> //
#include "ThreadManager/ThreadManager.h"

using namespace std;
using namespace cv;
using namespace hnurm;

bool thread_flag=false;

hnurm::ThreadManager thread_manager;

void signalHandler( int signum )
{
    
    // 清理并关闭
    // 终止程序  
    pthread_cancel(thread_manager.GenerateId);
    pthread_cancel(thread_manager.ProcessId);
    pthread_cancel(thread_manager.ReceiveId);
    pthread_cancel(thread_manager.SendId);
    thread_manager.Exit();
    
   exit(signum);  
 
}

int main()
{
    // hnurm::ThreadManager thread_manager;
    signal(SIGINT, signalHandler);  
    thread_manager.InitAll();
    std::thread generate_thread(&hnurm::ThreadManager::GenerateThread,std::ref(thread_manager));
    thread_manager.GenerateId = generate_thread.native_handle();
   std::thread process_thread(&hnurm::ThreadManager::ProcessThread,std::ref(thread_manager));
   thread_manager.ProcessId = process_thread.native_handle();
    std::thread receive_thread(&hnurm::ThreadManager::ReceiveThread,std::ref(thread_manager));
    thread_manager.ReceiveId = receive_thread.native_handle();
    std::thread send_thread(&hnurm::ThreadManager::SendThread,std::ref(thread_manager));
    thread_manager.SendId = send_thread.native_handle();

   generate_thread.join();
    process_thread.join();
    receive_thread.join();
    send_thread.join();

    return 0;//aha this will never happen
}
