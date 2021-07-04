//
// Created by Lih-pc on 2021-06-21.
//

#ifndef FILESYSTEM_MYSYS_H
#define FILESYSTEM_MYSYS_H

#include "memory.h"
#include "tree.h"
#include "file.h"

//最大加载盘的数目
#define MAX_LOAD_DISC      10

//磁盘文件配置
#define SYS_DISC     ("SYS_DISC")

//文件名不长于24
#define MAX_NAME_SIZE       24

//用户名最长
#define MAX_USER_SIZE       24

//文件路径
typedef struct{
    CSTree root;
    CSTree cur;
    int maxId;
    inodeItem **childFile;
    int childNum;
    char curFolder[24];
}workPlace;

//已经加载的磁盘数
int LoadDiscSize;

//加载的inode表
form *loadedInode[MAX_LOAD_DISC];

//磁盘挂载处
discType *loadedDisc[MAX_LOAD_DISC];

//磁盘基本路径
workPlace workingPlace[MAX_LOAD_DISC];

//当前登录的用户
int CurUserId;
char CurUserName[MAX_USER_SIZE];

int workingDisc;

void sysParaInit();

int createDisc(const char *name, int size);

int loadDisc();

void writeFile(const char *name, int index, int size, const char *content, int pri);

void formTra(const void *data, int num);

void updateChildFile(int index);

void useDisc(int index);

int login(const char *name, const char *password);

#endif //FILESYSTEM_MYSYS_H
