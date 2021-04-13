#ifndef __TIME_H
#define __TIME_H

#include <linux/ktime.h>
#include <linux/module.h>

#define __CST (8 * 3600) // UTC+8 时区

MODULE_LICENSE("Dual BSD/GPL");

struct tm* getDateAndTime(struct tm *dtm);
char* saveTimeToStr(char *str, const struct tm *time);

#endif
