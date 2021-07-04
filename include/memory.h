//
// Created by Lih-pc on 2021-06-17.
//

#ifndef FILESYSTEM_MEMORY_H
#define FILESYSTEM_MEMORY_H

//1block = EACH_BLOCK_SIZE byte
#define EACH_BLOCK_SIZE       1024

#define BASE_OFF              5120          //磁盘空间inode表的偏移量

#include "form.h"
#include <stdio.h>

typedef struct discType{
    char *DiscName;
    form *BlockForm;
    int MaxBlockSize;
}discType;

void initDisc(const char *name, int blockSize, int index);

void discLoadRead(FILE *fp, int index);

void discLoadSave(FILE *fp, int index);

char *readFromMem(const char* name, int startBlock);

int writeToMem(int index, int size, const char* content);

void debugForm(int discID);

#endif //FILESYSTEM_MEMORY_H
