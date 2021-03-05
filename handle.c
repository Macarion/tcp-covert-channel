#include "handle.h"

int send_data(Map *map, unsigned int ip, void *buf, int size)
{
    int state;
    Data *pd = find_data(map, ip);
    if (!pd) return -1;

    state = get_sstate(pd);
    switch (state)
    {
        case _NULL: break;
        case _FINI:
            del_data(map, ip);
            break;

        //Send content
        case _SEND: 
            if (get_content(pd, buf, size))
            {
                break;
            }
            set_sstate(pd, _CHEK);
            break;
        case _WAIT:
            set_sstate(pd, _SEND);
            break;
        case _CHEK:
            {
                unsigned short chksum = check_chk(pd);
                memcpy(buf, &chksum, sizeof(chksum));
                set_sstate(pd, _FINI);
            }
    }
    return 0;
}

void recv_data(Map *map, unsigned int ip, const void *buf, int size)
{
    int state;
    Data *pd = find_data(map, ip);
    if (!pd) return;

    state = get_rstate(pd);
    switch (state)
    {
        case _NULL: break;
        case _WAIT:
            set_rstate(pd, _RECV);
            break;
        case _RECV:
            if (!add_content(pd, buf, size))
            {
                set_rstate(pd, _CHEK);
            }
            break;
        case _FINI:
            save_to_file(SAVEFILE, pd);
            del_data(map, ip);
            break;
        case _CHEK:
            {
                unsigned short chksum = check_chk(pd);
                if (!memcmp(&chksum, buf, sizeof(chksum)))
                {
                    set_rstate(pd, _FINI);
                    break;
                }
                set_rstate(pd, _WAIT);
                break;
            }
    }
    /* add_content(pd, buf, size); */
}


