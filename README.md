# tcp-covert-channel
大创课题，在TCP协议头中嵌入数据。  
主要涉及Linux内核模块及netfilter编程。  
放在这里主要为了备份和分享方便。  
### 各文件说明
send.c      主程序代码文件  
covert.c    用户空间程序，需要单独编译  
config.h    配置文件  
data        数据结构文件  
handle      数据收发处理  
device      创建字符设备实现与用户空间通信  
file        文件的读写交互  
time        获取当前时间  
functions   一些方便使用的函数  
command     调用用户空间的程序  
list        链表数据结构  
queue       队列数据结构  

Makefile 文件可能需要酌情修改。  
### 注意：程序处于开发阶段！！！
程序处于开发阶段，由于可能存在重大BUG，仅供学习交流，请不要运行程序。  
