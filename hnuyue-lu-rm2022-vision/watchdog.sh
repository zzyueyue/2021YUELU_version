#!/bin/bash
# execute using bash
# 特别注意特别注意特别注意特别注意特别注意特别注意特别注意特别注意特别注意特别注意特别注意特别注意：请将/bin/sh链接到/bin/bash，ubuntu18.04后的默认shell为dash，dash无法source动态加载环境变量！

# 检查间隔时间，每隔sec秒查看一次当前线程是否正常运行
sec=1
# 异常统计数，超过5次重新make，超过20次reboot
remakecnt=0
rebootcnt=0
#进程名字，取决于CMake的项目名称，即可执行文件的名称，也可以在启动程序后在终端输入top/ps来查找
PROC_NAME=HNU

# init openvino
source /opt/intel/openvino_2021/bin/setupvars.sh
# remember to add these if your programme depends on other dependencies
source /etc/profile
source ~/.bashrc
# be sure that vars in profile will be loaded
# source /etc/environment
# source ...

# 进入文件里面
cd /home/rm/hnuyue-lu-rm2022-vision/build

# 清除并重新make一下，防止文件损坏（-j提高效率,根据核心数选择，一般2倍） 
make clean && make -j12 
    
# 赋予读写权限，通过管道将密码传递给sudo -S（-S参数接收密码）
echo " " | sudo -S chmod 777 ./HNU

# 赋予串口权限,若有多个一次性赋予(*)，建议直接将当前用户加入串口所有者的用户组中，就不需要这一步了
echo " " | sudo -S chmod 777 /dev/ttyUSB* 
# 新建一个terminal在里面运行程序，方便监视程序的情况,若要ssh链接则直接运行，替换为./YueLuRMVision2022
gnome-terminal -- ./HNU
echo "[daemon]: ${PROC_NAME} has started!"

while [ 1 ] # 中括号的判断条件两边要空格
do
# 判定进程运行否，是则继续，否则重新启动
# ps列出进程，-ef给出进程的详细信息，grep捕获我们的进程，
# grep -v把grep进程去掉，否则grep也会把自己统计进来，wc(word count) -l(line)统计进程数
# 当程序正常运行的时候，tcount>=1(多线程）
tcount=`ps -ef | grep ${PROC_NAME} | grep -v "grep" | wc -l` 
echo "[daemon]:Thread count: ${tcount}"
if [ ${tcount} -ge 1 ];then  # -ge greater equal，大于等于1情况下 进程没被杀害
	echo "[daemon]: ${PROC_NAME} is running..."
	sleep ${sec} 
else  # 出现异常，进程未运行
	# 异常统计数累加
	rebootcnt=`expr ${rebootcnt} + 1`
	remakecnt=`expr ${remakecnt} + 1`
	if [ ${remakecnt} -gt 4 ];then # greater,5 times failed: try to remake the project
		# echo -e "\033[31m你要输入的字符串\033[0m"，可以换颜色，xxm是颜色代码，还可以换背景
		echo -e "\033[31m[daemon]:trying to remake project...\033[0m"
		cd /home/rm/hnuyue-lu-rm2022-vision/build  #移动到你要编译运行的文件夹里面
		make clean && make -j12  #清除并重新make一下
		echo -e "\033[31m[daemon]:Remake successfully! \033[0m"
		remakecnt=$[ 0 ]
	fi
	
	if [ ${rebootcnt} -gt 20 ];then # 大于20，直接重开！
		echo " " | sudo -S reboot
	fi
	
	# 小于五次，尝试重新运行程序
	echo -e "\033[31m[daemon]:${PROC_NAME} error! \033[0m" 
	echo -e "\033[31m[daemon]:Trying to restart ${PROC_NAME}...\033[0m" 
	echo -e "\033[32m[daemon]:retry counts: ${rebootcnt} \033[0m"
	echo " " | sudo -S chmod 777 /home/rm/hnuyue-lu-rm2022-vision/build/HNU
	cd /home/rm/hnuyue-lu-rm2022-vision/build
	gnome-terminal -- ./HNU
	echo -e "\033[34m [daemon]:${PROC_NAME} restart successfully! \033[0m"
	sleep ${sec}
	
fi  
done

