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

MODULE_LICENSE("Dual BSD/GPL");

/* #define __NET_DEVICE "wlan0" */

LinkList send_map;
LinkList recv_map;

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
            send_data(iph->daddr, &tcph->urg_ptr, sizeof(tcph->urg_ptr), tcph->seq);
            /* tcpCheckSum(skb, tcph, iph); */
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
    int ret = 0;
    struct file *fp;
    struct net_device *dev;
    struct net *net;
    char *fcont;
    char inter[32];

    InitList(&send_map);
    InitList(&recv_map);

    if (!(fcont = get_file_content_ptr(CONFIGFILE)))
        return -1;
    sscanf(fcont, "%s", inter);
    kfree(fcont);
    /* printk(KERN_INFO "Interface: %s\n", inter); */
    dev = dev_get_by_name(&init_net, inter);
    if (IS_ERR(dev) || !dev)
        return -1;
    printk(KERN_INFO "Interface: %s\n", dev->name);

    sendNfHook.dev = dev;
    recvNfHook.dev = dev;
    net = dev_net(dev);
    dev_put(dev);
    if (IS_ERR(net) || !net)
        return -1;
    /* printk(KERN_INFO "Net address: %p\n", net); */

    fp = filp_open(SHFILEPATH, O_DIRECTORY, S_IRUSR);
    if (IS_ERR(fp))
    {
        char *argv[] = {"/bin/mkdir", SHFILEPATH, NULL};
        static char *envp[] = {
            "HOME=/",
            "TERM=linux",
            "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL 
        };
        call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    }
    else
    {
        filp_close(fp, NULL);
    }

    if (load_from_file(send_map, LOADFILE) < 0)
    {
        info("Failed in reading send_data file.\n");
    }

    device_init();

    /* return -1; */

    printk(KERN_INFO "NET_HOOK STARTED!\n");

    ret = nf_register_net_hook(net, &sendNfHook);
    if (0 != ret)
    {
        printk(KERN_WARNING "nf_register_hook failed\n");
        return -1;
    }

    ret = nf_register_net_hook(net, &recvNfHook);
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
    print_all_datas(send_map);
    destory_all(&send_map);
    destory_all(&recv_map);
    nf_unregister_net_hook(&init_net, &sendNfHook);
    nf_unregister_net_hook(&init_net, &recvNfHook);
    printk(KERN_INFO "SEND NET_HOOK STOPPED!\n");
}

module_init(Hook_Init);
module_exit(Hook_Exit);
