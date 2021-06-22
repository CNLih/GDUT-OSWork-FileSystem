//
// Created by Lih-pc on 2021-06-21.
//

#ifndef FILESYSTEM_FILE_H
#define FILESYSTEM_FILE_H

#include "form.h"
#include "mysys.h"
#define SYS_FILE     ("SYS_FILE")

#define EACH_DISC_MAX_FILES       100

form *loadedInode[MAX_LOAD_DISC];
int LoadInodeSize = 0;

void newInode(int usingDisc);

int newFile(int usingDisc, int id, int fileSize, int blockSize, int startBlock);

void debugInodeInfo();

void writeInode(FILE *fp);

void readInode(FILE *fp);

void debugInodeInfo();

#endif //FILESYSTEM_FILE_H
