//
// Created by Lih-pc on 2021-06-17.
//

#ifndef FILESYSTEM_MEMORY_H
#define FILESYSTEM_MEMORY_H

//1block = EACH_BLOCK_SIZE byte
#define EACH_BLOCK_SIZE       64
//文件名不长于24
#define MAX_NAME_SIZE       24

#define SYS_DISC     ("SYS_DISC")

int createDisc(const char *name, int size);

int discLoadRead();

char *readFile(const char* name, int startBlock);

int writeFile(const char *name, int size, const char* content);

void debugForm(const char *name);

#endif //FILESYSTEM_MEMORY_H
