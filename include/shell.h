//
// Created by Lih-pc on 2021-06-24.
//

#ifndef FILESYSTEM_SHELL_H
#define FILESYSTEM_SHELL_H

#define FUNC_NUM  13

extern char workingDir[128];

void cmd_ls(int fun);

int cmd_cd(int fun, char *name);

void cmd_mkdir(char *name);

int cmd_write(char *name, char *content, int size, int pri);

int cmd_read(char *name);

void shellRun();

#endif //FILESYSTEM_SHELL_H
