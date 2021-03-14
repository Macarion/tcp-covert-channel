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

#define __NET_DEVICE "wlan0"

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
            /* if (!send_data(iph->daddr, &tcph->urg_ptr, sizeof(tcph->urg_ptr))) */
                /* tcpCheckSum(skb, tcph, iph); */
            send_data(iph->daddr, &tcph->urg_ptr, sizeof(tcph->urg_ptr), tcph->seq);
        }
    }
    return NF_ACCEPT;
}

static uint recvHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
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
            recv_data(iph->saddr, &tcph->urg_ptr, sizeof(unsigned short), tcph->seq);
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

struct nf_hook_ops recvNfHook =
{
    .hook = recvHook,
    .pf = NFPROTO_IPV4,
    .hooknum = NF_BR_PRE_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};

static int Hook_Init(void)
{
    /* const char message[] = "apt provides a high-level commandline interface for the package management system."; */
    /* unsigned int ip = 67217600; */
    /* int n = 0; */
    int ret = 0;
    /* unsigned short buf; */
    /* int size; */

    if (load_from_file(&send_map, LOADFILE) <= 0)
    {
        info("Failed in reading send_data file.\n");
    }

    /* save_all_datas(&send_map, SAVEFILE); */

    device_init();

    /* print_all_datas(&recv_map); */

    /* { */
        /* int i; */
        /* buf = 0x0100; */
        /* recv_data(ip, &buf, 2); */
        /* buf = size = strlen(message); */
        /* recv_data(ip, &buf, 2); */
        /* for (i = 0; i < size; i += 2) */
        /* { */
            /* buf = (message[i + 1] << 8) + message[i]; */
            /* recv_data(ip, &buf, 2); */
        /* } */
        /* buf = check_chk(find_data(&send_map, ip)); */
        /* recv_data(ip, &buf, 2); */
    /* } */

    /* print_all_datas(&recv_map); */
    
    /* print_all_datas(&send_map); */
    /* while (find_data(&send_map, ip)) */
    /* { */
        /* send_data(ip, &buf, 2); */
        /* printk(KERN_CONT "%c%c", buf & 0xff, buf >> 8); */
        /* printk(KERN_INFO "%c%c", buf & 0xff, buf >> 8); */
        /* n++; */
    /* } */
    /* printk(KERN_CONT "\n"); */
    /* printk(KERN_INFO "n = %d\n", n); */

    /* return -1; */

    printk(KERN_INFO "NET_HOOK STSRTED!\n");

    ret = nf_register_net_hook(&init_net, &sendNfHook);
    if (0 != ret)
    {
        printk(KERN_WARNING "nf_register_hook failed\n");
        return -1;
    }

    ret = nf_register_net_hook(&init_net, &recvNfHook);
    if (0 != ret)
    {
        printk(KERN_WARNING "nf_register_hook failed\n");
        return -1;
    }

    return 0;
}

static void Hook_Exit(void)
{
    device_exit();
    /* save_all_datas(&send_map, SAVEFILE); */
    print_all_datas(&send_map);
    free_map(&send_map);
    free_map(&recv_map);
    nf_unregister_net_hook(&init_net, &sendNfHook);
    nf_unregister_net_hook(&init_net, &recvNfHook);
    printk(KERN_INFO "SEND NET_HOOK STOPPED!\n");
}

module_init(Hook_Init);
module_exit(Hook_Exit);
