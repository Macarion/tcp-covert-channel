#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/fs.h>
#include <linux/tcp.h>
#include <linux/crypto.h>
#include <linux/netlink.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_bridge.h>
#include <linux/netfilter/x_tables.h>

#include "file_hash.h"
#include "data.h"
#include "file_io.h"
#include "functions.h"

//MODULE_LICENSE("GPL");

#define __NET_DEVICE "eth0"

static struct Map data_map;

static uint hookCallBack(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *iph = NULL;
    struct tcphdr *tcph = NULL;

    if (unlikely(skb == NULL))
    {
        return NF_ACCEPT;
    }
    iph = ip_hdr(skb);
    if (unlikely(iph == NULL))
    {
        return NF_ACCEPT;
    }

    if (iph->protocol == IPPROTO_TCP)
    {
        tcph = tcp_hdr(skb);
        if (!tcph->urg && !tcph->urg_ptr)
        {
//            printk(KERN_CONT "%u", iph->daddr);
            map_send(&data_map, iph->daddr, &tcph->urg_ptr);
            tcpCheckSum(skb, tcph, iph);
        }
    }
    return NF_ACCEPT;
}

struct nf_hook_ops NfHook =
{
    .hook = hookCallBack,
    .pf = NFPROTO_IPV4,
    .hooknum = NF_BR_FORWARD,
    .priority = NF_IP_PRI_FIRST,
};

static int Hook_Init(void)
{
    int ret = 0;
//    char ipStr[20];
//    struct MapList *list_ptr;

    if (!map_loadFromFile(&data_map, __FILE_NAME))
    {
        info("Failed in reading file.\n");
        return -1;
    }
    printMapChildren(&data_map);
//    list_ptr = data_map.list;
//    while (list_ptr)
//    {
//        printk(KERN_INFO "%s, %d, %s\n", ipnAddrToStr(ipStr, list_ptr->raw_data->ip),
//            list_ptr->raw_data->size, list_ptr->raw_data->content);
//        printHashValue(list_ptr->raw_data->hash, __HASH_SIZE);
//        list_ptr = list_ptr->next;
//    }
//    info("\n");
//    return -1;

    printk(KERN_INFO "SEND NET_HOOK STSRTED!\n");

    ret = nf_register_net_hook(&init_net, &NfHook);
    if (0 != ret)
    {
        printk(KERN_WARNING "nf_register_hook failed\n");
        return -1;
    }

    return 0;
}

static void Hook_Exit(void)
{
    nf_unregister_net_hook(&init_net, &NfHook);
    printk(KERN_INFO "SEND NET_HOOK STOPPED!\n");
}

module_init(Hook_Init);
module_exit(Hook_Exit);
