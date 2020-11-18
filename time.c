#include "time.h"

struct tm* getDateAndTime(struct tm *dtm)
{
    ktime_t ktm;

    ktm = ktime_get_coarse_real();
    time64_to_tm(ktm / 1000000000, __CST, dtm);
    if (ktm)
    {
        dtm->tm_year += 1900;
        dtm->tm_mon += 1;
    }

    return dtm;
}

char* saveTimeToStr(char *str, const struct tm *time)
{
    sprintf(str, "[%.2ld-%.2d-%.2d %.2d:%.2d:%.2d]",
            time->tm_year / 100,
            time->tm_mon,
            time->tm_mday,
            time->tm_hour,
            time->tm_min,
            time->tm_sec);
    return str;
}
