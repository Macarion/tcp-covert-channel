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

#include "config.h"
#include "handle.h"
#include "device.h"

/* MODULE_LICENSE("Dual BSD/GPL"); */

#define __NET_DEVICE "eth0"

Map send_map;
Map recv_map;

static uint sendHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
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
        if (unlikely(tcph == NULL))
        {
            return NF_ACCEPT;
        }
        if (!tcph->urg && !tcph->urg_ptr)
        {
            /* if (map_send(&data_map, iph->daddr, &tcph->urg_ptr)) */
                /* tcpCheckSum(skb, tcph, iph); */
            send_data(&send_map, iph->daddr, &tcph->urg_ptr, sizeof(tcph->urg_ptr));
        }
    }
    return NF_ACCEPT;
}

static uint recrpnHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
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
        if (!tcph->urg && tcph->urg_ptr)
        {
            /* map_recrpn(&data_map, iph->saddr, tcph->urg_ptr); */
        }
    }
    return NF_ACCEPT;
}

struct nf_hook_ops sendNfHook =
{
    .hook = sendHook,
    .pf = NFPROTO_IPV4,
    /* .hooknum = NF_BR_FORWARD, */
    .hooknum = NF_BR_POST_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};

static int Hook_Init(void)
{
    int ret = 0;

    if (load_from_file(&send_map, LOADFILE) <= 0)
    {
        info("Failed in reading file.\n");
        return -1;
    }

    device_init();

    /* return -1; */

    /* printk(KERN_INFO "SEND NET_HOOK STSRTED!\n"); */

    /* ret = nf_register_net_hook(&init_net, &sendNfHook); */
    /* if (0 != ret) */
    /* { */
        /* printk(KERN_WARNING "nf_register_hook failed\n"); */
        /* return -1; */
    /* } */

    return 0;
}

static void Hook_Exit(void)
{
    device_exit();
    print_all_datas(&send_map);
    save_all_datas(&send_map, SAVEFILE);
    free_map(&send_map);
    /* nf_unregister_net_hook(&init_net, &sendNfHook); */
    /* printk(KERN_INFO "SEND NET_HOOK STOPPED!\n"); */
}

module_init(Hook_Init);
module_exit(Hook_Exit);
