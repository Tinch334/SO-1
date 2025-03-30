#ifndef __MINI_SHELL_H__
#define __MINI_SHELL_H__
#include "slist.h"

void pipeExecutor(SList *commands);

void executeCommand(char *command, const int in, const int out);

#endif /* __MINI_SHELL_H__ */