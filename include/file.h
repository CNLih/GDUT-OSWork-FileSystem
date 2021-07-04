//
// Created by Lih-pc on 2021-06-21.
//

#ifndef FILESYSTEM_FILE_H
#define FILESYSTEM_FILE_H

#include "form.h"

#define EACH_DISC_MAX_FILES       100

//inode项
typedef struct{
    int id;
    char *name;
    int fileSize;
    int blockSize;
    int startBlock;
    int ownerId;
    int privilege;
}inodeItem;

void newInode(int usingDisc);

int newFile(int usingDisc, int id, int fileSize, int startBlock, const char *name, int pri);

void saveInode(FILE *fp, int index);

void readInode(FILE *fp, int index);

void debugInodeInfo();

void addFileNode(int id, int index);

void readPath(FILE *fp, int index);

void savePath(FILE *fp, int index);

void showChildDir(int index);

inodeItem *getFileById(int index, int id);

#endif //FILESYSTEM_FILE_H
