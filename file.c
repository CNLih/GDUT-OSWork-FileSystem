//
// Created by Lih-pc on 2021-06-21.
//

/**
 * 编写inode表，实现树状文件目录
 * 结构：inode表，树
 * 测试：为每个盘符建立inode表，存取，接口-readInode(disc)
 */

#include <stdio.h>
#include "include/file.h"
#include "include/form.h"
#include "include/mysys.h"

typedef struct inodeItem{
    int id;
    int linkedN;
    int fileSize;
    int blockSize;
}inodeItem;

form *loadedInode[MAX_LOAD_DISC];
int LoadSize = 0;

void writeInode(FILE *fp){

}

void readInode(FILE *fp){

}