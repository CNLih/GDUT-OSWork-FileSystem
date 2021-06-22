//
// Created by Lih-pc on 2021-06-21.
//

#ifndef FILESYSTEM_FILE_H
#define FILESYSTEM_FILE_H

#include "form.h"
#include "mysys.h"

#define EACH_DISC_MAX_FILES       100

form *loadedInode[MAX_LOAD_DISC];

void newInode(int usingDisc);

int newFile(int usingDisc, int id, int fileSize, int startBlock);

void saveInode(FILE *fp);

void readInode(FILE *fp);

void debugInodeInfo();

#endif //FILESYSTEM_FILE_H
