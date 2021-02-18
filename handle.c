#include "handle.h"

int send_data(unsigned int ip, void *buf, int size)
{
    int state = get_sstate(ip);
    switch (state)
    {
        case _NULL: break;
        case _FINI:
            del_data(ip);

        //Send content
        case _SEND: 
            if (get_content(ip, buf, size))
            {
                break;
            }
            set_sstate(ip, _CHEK);
            break;
        case _WAIT:
            set_sstate(ip, _SEND);
        case _CHEK:
            {
                unsigned short chksum = check_chk(ip);
                memcpy(buf, &chksum, sizeof(chksum));
            }
    }
    return 0;
}

void recv_data(unsigned int ip, const void *buf, int size)
{
    int state = get_rstate(ip);
    switch (state)
    {
        case _NULL: break;
        case _WAIT:
            set_rstate(ip, _RECV);
            break;
        case _RECV:
            if (add_content(ip, buf, size))
            {
                set_rstate(ip, _CHEK);
            }
            break;
        case _FINI:
            del_data(ip);
            break;
        case _CHEK:
            {
                unsigned short chksum = check_chk(ip);
                if (!memcmp(&chksum, buf, sizeof(chksum)))
                {
                    set_rstate(ip, _FINI);
                    break;
                }
                set_rstate(ip, _WAIT);
                break;
            }
    }
    /* add_content(ip, buf, size); */
}


