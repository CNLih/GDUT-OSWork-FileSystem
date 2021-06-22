//
// Created by Lih-pc on 2021-06-17.
//

#ifndef FILESYSTEM_MEMORY_H
#define FILESYSTEM_MEMORY_H

//1block = EACH_BLOCK_SIZE byte
#define EACH_BLOCK_SIZE       64

//已经加载的磁盘数
int LoadDiscSize = 0;

int createDisc(const char *name, int size);

int discLoadRead();

char *readFile(const char* name, int startBlock);

int writeFile(const char *name, int size, const char* content);

void debugForm(const char *name);

#endif //FILESYSTEM_MEMORY_H
