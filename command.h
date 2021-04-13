#ifndef _COMMAND_H
#define _COMMAND_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include "functions.h"
#include "config.h"

int call_user_program(char *cmd, unsigned int ip); // 调用用户程序

int call_user_file(char *fpath, unsigned int ip);  // 调用用户脚本文件

#endif
