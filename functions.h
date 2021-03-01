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

char* ipnAddrToStr(char* ipAddrStr, unsigned int ipAddr);
unsigned int ipnStrToAddr(const char* ipAddrStr);
void printHashValue(const unsigned char *fileSha, int size);
void tcpCheckSum(struct sk_buff *skb, struct tcphdr *tcph, struct iphdr *iph);
void info(const char *str);
void infonum(int num);
void print(const char *str);
int strcpyn(char *tar, const char *str, int size);
int countInfoLen(const char *str);
int countDataLen(const char *str);

#endif
