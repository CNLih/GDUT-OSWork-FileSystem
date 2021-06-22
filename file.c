//
// Created by Lih-pc on 2021-06-21.
//

/**
 * 编写inode表，实现树状文件目录
 * 结构：inode表，树
 * 测试：为每个盘符建立inode表，存取，接口-readInode(disc)
 */

#include <stdio.h>
#include <stdlib.h>
#include "include/file.h"

typedef struct{
    int id;
    int linkedN;
    int fileSize;
    int blockSize;
    int startBlock;
    int *user; //暂
}inodeItem;

void myFileDestroy(void *data){
    free(((inodeItem *)data)->user);
    free(data);
}

/**
 * 新建inode表（在新建磁盘的时候调用）
 * @param usingDisc
 */
void newInode(int usingDisc){
    loadedInode[usingDisc] = formInit(&myFileDestroy, EACH_DISC_MAX_FILES);
}

/**
 * 文件载入到inode表
 * @param usingDisc
 * @param id
 * @param fileSize
 * @param startBlock
 * @return
 */
int newFile(int usingDisc, int id, int fileSize, int startBlock) {
    int newIndex = loadedInode[usingDisc]->size ++;
    if(newIndex == EACH_DISC_MAX_FILES){
        return -1;
    }
    inodeItem *newItem = (inodeItem *) malloc(sizeof(inodeItem));
    newItem->blockSize = (fileSize + EACH_BLOCK_SIZE - 1) / EACH_BLOCK_SIZE;
    newItem->fileSize = fileSize;
    newItem->id = id;
    newItem->linkedN = 1;
    newItem->startBlock = startBlock;

    loadedInode[usingDisc]->head[newIndex] = newItem;
    return 1;
}

/**
 * 将inode表写入fp文件中
 * @param fp 存放于该文件中
 */
void saveInode(FILE *fp){
    fprintf(fp, "%d ", LoadDiscSize);
    for(int i = 0; i < LoadDiscSize; i ++){
        fprintf(fp, "%d ", loadedInode[i]->size);
        for(int j = 0; j < loadedInode[i]->size; j ++){
            fprintf(fp, "%d %d %d %d %d  ",
                    ((inodeItem *)loadedInode[i]->head[j])->id,
                    ((inodeItem *)loadedInode[i]->head[j])->linkedN,
                    ((inodeItem *)loadedInode[i]->head[j])->blockSize,
                    ((inodeItem *)loadedInode[i]->head[j])->startBlock,
                    ((inodeItem *)loadedInode[i]->head[j])->fileSize);
        }
    }
}

/**
 * 从fp中读取inode表
 * @param fp 存放于该文件中
 */
void readInode(FILE *fp){
    for(int i = 0; i < LoadDiscSize; i ++){
        loadedInode[i] = formInit(&myFileDestroy, EACH_DISC_MAX_FILES);
        fscanf(fp, "%d ", &loadedInode[i]->size);
        for(int j = 0; j < loadedInode[i]->size; j ++){
            inodeItem *newItem = (inodeItem *) malloc(sizeof(inodeItem));
            loadedInode[i]->head[j] = newItem;
            fscanf(fp, "%d %d %d %d %d  ",
                   &((inodeItem *)loadedInode[i]->head[j])->id,
                   &((inodeItem *)loadedInode[i]->head[j])->linkedN,
                   &((inodeItem *)loadedInode[i]->head[j])->blockSize,
                   &((inodeItem *)loadedInode[i]->head[j])->startBlock,
                   &((inodeItem *)loadedInode[i]->head[j])->fileSize);
        }
    }
//    fpos_t off;
//    fgetpos(fp, &off);
//    printf("offset = %d", off);
}

/**
 * 打印inode表
 */
void debugInodeInfo(){
    for(int i = 0; i < LoadDiscSize; i ++){
        for(int j = 0; j < loadedInode[i]->size; j ++){
            printf("id:%d", ((inodeItem *)loadedInode[i]->head[j])->id);
            printf(" linkedN:%d", ((inodeItem *)loadedInode[i]->head[j])->linkedN);
            printf(" fileSize:%d", ((inodeItem *)loadedInode[i]->head[j])->fileSize);
            printf(" blockSize:%d", ((inodeItem *)loadedInode[i]->head[j])->blockSize);
            printf("\n");
        }
    }
}
