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
        case _NULL: break;
        case _FINI:
            break;

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
            set_sstate(pd, _WAIT2);
            if (pd->type == TP_ACKN)
            {
                del_data(&send_map, ip);
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
            }
    }
    pd->lastseq = seq;
    pd->lastsnd = *buf;
    return 0;
}

void recv_data(unsigned int ip, const unsigned short *buf, int size, unsigned int seq)
{
    int state;
    Data *pd;

    if (*buf >> 8 == 0x1)
    {
        if ((*buf & 0xff) != TP_ACKN)
        {
            pd = find_data(&recv_map, ip);
            if (pd)
            {
                del_data(&recv_map, ip);
            }
            pd = append_data(&recv_map, ip, 0);
            pd->type = *buf & 0xff;
        }
        else
        {
            del_data(&send_map, ip);
        }
        return;
    }

    pd = find_data(&recv_map, ip);
    if (!pd) return;

    if (seq == pd->lastseq)
    {
        return;
    }
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
                        case TP_COMD: call_user_program(pd->content, pd->ip); break;
                        case TP_SHFL: break;
                        case TP_DATA: save_to_file(SAVEFILE, pd); break;
                    }
                    
                    del_data(&recv_map, ip);

                    pd = append_data(&send_map, ip, 0);
                    set_type(pd, TP_ACKN);
                    
                    break;
                }
                set_rstate(pd, _WAIT);
                info("Checksum incorrect!\n");
                break;
            }
    }
    pd->lastseq = seq;
    /* add_content(pd, buf, size); */
}


