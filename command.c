#include "command.h"

int call_user_program(char *cmd, unsigned int ip)
{
    char ipstr[20] = {0};
    ipnAddrToStr(ipstr, ip);
    char *argv[] = {USERPROGRAM, "-R", "-i", ipstr, "-c", cmd, NULL};
    static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL 
    };

    return call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
    /* return call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT); */
}

int call_user_file(char *fpath, unsigned int ip)
{
    char ipstr[20] = {0};
    ipnAddrToStr(ipstr, ip);
    char *argv[] = {USERPROGRAM, "-R", "-i", ipstr, "-s", fpath, NULL};
    static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL 
    };

    return call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
    /* return call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT); */
}
