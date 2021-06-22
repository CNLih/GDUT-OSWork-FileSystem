//
// Created by Lih-pc on 2021-06-21.
//

#ifndef FILESYSTEM_MYSYS_H
#define FILESYSTEM_MYSYS_H

#include "memory.h"

//最大加载盘的数目
#define MAX_LOAD_DISC      10

//磁盘文件配置
#define SYS_DISC     ("SYS_DISC")

//文件名不长于24
#define MAX_NAME_SIZE       24

//已经加载的磁盘数
int LoadDiscSize;

//磁盘挂载处
discType *loadedDisc[MAX_LOAD_DISC];

void sysParaInit();

int createDisc(const char *name, int size);

int loadDisc();

void writeFile(int index, int size, const char* content);

#endif //FILESYSTEM_MYSYS_H
