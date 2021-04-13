#include "functions.h"

char* ipnAddrToStr(char* ipAddrStr, unsigned int ipAddr)
{
    sprintf(ipAddrStr, "%u.%u.%u.%u",
            ipAddr & 0xff,
            ipAddr >> 8 & 0xff,
            ipAddr >> 16 & 0xff,
            ipAddr >> 24);
    return ipAddrStr;
}

unsigned int ipnStrToAddr(const char* ipAddrStr)
{
    unsigned int ipp[4];
    sscanf(ipAddrStr, "%u.%u.%u.%u", ipp + 0, ipp + 1, ipp + 2, ipp + 3);
    return (ipp[3] << 24) + (ipp[2] << 16) + (ipp[1] << 8) + ipp[0];
}

void printHashValue(const unsigned char *hash, int size)
{
    int i = 0;
    do
    {
        printk(KERN_CONT "%02hhx", hash[i]);
    }
    while (++i < size);
    printk(KERN_CONT "\n");
}

void tcpCheckSum(struct sk_buff *skb, struct tcphdr *tcph, struct iphdr *iph)
{
    if (tcph->urg_ptr != 0 && tcph->urg == 0)
    {
        tcph->check = 0;
        skb->csum = csum_partial((unsigned char *)tcph, ntohs(iph->tot_len) - iph->ihl * 4, 0);
        tcph->check = csum_tcpudp_magic(iph->saddr,
                                iph->daddr,
                                ntohs(iph->tot_len) - iph->ihl * 4,
                                iph->protocol,
                                skb->csum);
    }
}

void info(const char *str)
{
    if (__DEBUG)
    {
        printk(KERN_INFO "%s", str);
    }
}

void infonum(int num)
{
    if (__DEBUG)
    {
        printk(KERN_CONT "%d", num);
    }
}

int strcpyn(char *tar, const char *str, int size)
{
    int pos;
    for (pos = 0; str[pos] != '\n' && str[pos] != '\0' && pos < size; ++pos)
    {
        tar[pos] = str[pos];
    }
    tar[pos] = '\0';
    return pos;
}

int countInfoLen(const char *str)
{
    int pos;
    for (pos = 0; str[pos] != '\0'; ++pos)
    {
        if (str[pos] == ']')
        {
            while (str[++pos] == ' ');
            if (str[pos] != '[')
            {
                return pos;
            }
        }
    }
    return pos;
}

int countDataLen(const char *str)
{
    int pos;
    for (pos = 0; str[pos] != '\n' && str[pos] != '\0'; ++pos);
    return pos;
}

