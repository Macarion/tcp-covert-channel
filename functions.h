#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#define __DEBUG 1
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/tty.h>
#include <linux/sched.h>

char* ipnAddrToStr(char* ipAddrStr, unsigned int ipAddr);       // ip转字符串
unsigned int ipnStrToAddr(const char* ipAddrStr);               // 字符串转ip
void printHashValue(const unsigned char *fileSha, int size);
void tcpCheckSum(struct sk_buff *skb, struct tcphdr *tcph, struct iphdr *iph); // 校验tcp数据包，已废弃
void info(const char *str);         // 打印字符串到log文件
void infonum(int num);
int strcpyn(char *tar, const char *str, int size); // 字符串复制，更安全
int countInfoLen(const char *str);
int countDataLen(const char *str);

#endif
