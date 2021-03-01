#ifndef __TIME_H
#define __TIME_H

#include <linux/ktime.h>
#include <linux/module.h>

#define __CST (8 * 3600)

MODULE_LICENSE("GPL");

struct tm* getDateAndTime(struct tm *dtm);
char* saveTimeToStr(char *str, const struct tm *time);

#endif
