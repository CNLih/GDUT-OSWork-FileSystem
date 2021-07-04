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
#include <string.h>
#include "include/file.h"
#include "include/mysys.h"

void myFileDestroy(void *data){
    free(((inodeItem *)data)->name);
    free(data);
}

void treeDestroy(void *data){
    free(data);
}

void treePrint(const void *data){
    int dat = *(int *)data;
    printf("%d ", dat);
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
int newFile(int usingDisc, int id, int fileSize, int startBlock, const char *name, int pri) {
    int newIndex = loadedInode[usingDisc]->size ++;
    if(newIndex == EACH_DISC_MAX_FILES){
        return -1;
    }
    inodeItem *newItem = (inodeItem *) malloc(sizeof(inodeItem));
    newItem->blockSize = (fileSize + EACH_BLOCK_SIZE - 1) / EACH_BLOCK_SIZE;
    newItem->fileSize = fileSize;
    newItem->id = id;
    newItem->name = (char *) malloc(sizeof(char) * MAX_NAME_SIZE);
    strcpy(newItem->name, name);
    newItem->startBlock = startBlock;
    newItem->ownerId = CurUserId;
    newItem->privilege = pri;

    loadedInode[usingDisc]->head[newIndex] = newItem;
    return 1;
}

/**
 * 将inode表写入fp文件中
 * @param fp 存放于该文件中
 * @param index 要保存到那一个磁盘号中
 */
void saveInode(FILE *fp, int index) {
//    fprintf(fp, "%d ", LoadDiscSize);
//    for(int i = 0; i < LoadDiscSize; i ++){
        fprintf(fp, "%d %d ", loadedInode[index]->size, workingPlace[index].maxId);
        for(int j = 0; j < loadedInode[index]->size; j ++){
            fprintf(fp, "%d %24s %d %d %d %d %d  ",
                    ((inodeItem *)loadedInode[index]->head[j])->id,
                    ((inodeItem *)loadedInode[index]->head[j])->name,
                    ((inodeItem *)loadedInode[index]->head[j])->blockSize,
                    ((inodeItem *)loadedInode[index]->head[j])->startBlock,
                    ((inodeItem *)loadedInode[index]->head[j])->fileSize,
                    ((inodeItem *)loadedInode[index]->head[j])->ownerId,
                    ((inodeItem *)loadedInode[index]->head[j])->privilege);
        }
//    }
}

/**
 * 从fp中读取inode表
 * @param fp 存放于该文件中
 * @param index 要读取到那一个磁盘号中
 */
void readInode(FILE *fp, int index) {
//    for(int i = 0; i < LoadDiscSize; i ++){
        loadedInode[index] = formInit(&myFileDestroy, EACH_DISC_MAX_FILES);
        fscanf(fp, "%d %d ", &loadedInode[index]->size, &workingPlace[index].maxId);
        for(int j = 0; j < loadedInode[index]->size; j ++){
            inodeItem *newItem = (inodeItem *) malloc(sizeof(inodeItem));
            loadedInode[index]->head[j] = newItem;
            ((inodeItem *)loadedInode[index]->head[j])->name = (char *) malloc(sizeof(char) * MAX_NAME_SIZE);
            fscanf(fp, "%d %24s %d %d %d %d %d  ",
                   &((inodeItem *)loadedInode[index]->head[j])->id,
                   ((inodeItem *)loadedInode[index]->head[j])->name,
                   &((inodeItem *)loadedInode[index]->head[j])->blockSize,
                   &((inodeItem *)loadedInode[index]->head[j])->startBlock,
                   &((inodeItem *)loadedInode[index]->head[j])->fileSize,
                   &((inodeItem *)loadedInode[index]->head[j])->ownerId,
                   &((inodeItem *)loadedInode[index]->head[j])->privilege);
        }
//    }
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
            printf(" name:%s", ((inodeItem *)loadedInode[i]->head[j])->name);
            printf(" fileSize:%d", ((inodeItem *)loadedInode[i]->head[j])->fileSize);
            printf(" blockSize:%d", ((inodeItem *)loadedInode[i]->head[j])->blockSize);
            printf("\n");
        }
    }
}

inodeItem *getFileById(int index, int id){
    //可以用hash来放inode表，检索会更快
    for(int j = 0; j < loadedInode[index]->size; j ++){
        if(id == ((inodeItem *)loadedInode[index]->head[j])->id){
            inodeItem *item = (inodeItem *) malloc(sizeof(inodeItem));
            item->id = ((inodeItem *)loadedInode[index]->head[j])->id;
            item->name = ((inodeItem *)loadedInode[index]->head[j])->name;
            item->fileSize = ((inodeItem *)loadedInode[index]->head[j])->fileSize;
            item->blockSize = ((inodeItem *)loadedInode[index]->head[j])->blockSize;
            item->startBlock = ((inodeItem *)loadedInode[index]->head[j])->startBlock;
            item->ownerId = ((inodeItem *)loadedInode[index]->head[j])->ownerId;
            item->privilege = ((inodeItem *)loadedInode[index]->head[j])->privilege;
            return item;
        }
    }
    return NULL;
}

/**
 * 将文件加入到目录文件中
 * @param id
 * @param index
 */
void addFileNode(int id, int index){
    int *fileId = (int *) malloc(sizeof(int));
    *fileId = id;
    newNode(fileId, workingPlace[index].cur);
}

/**
 * 从文件中读取磁盘index的目录结构
 * @param fp
 * @param index
 */
void readPath(FILE *fp, int index){
    form *myForm = readTreeForm(fp);
    workingPlace[index].root = createTreeByForm(myForm, treeDestroy);
//    workingPlace[index].maxId = (workingPlace[index].root);
    workingPlace[index].cur = workingPlace[index].root;
}

/**
 * 保存目录结构到文件index
 * @param fp
 * @param index
 */
void savePath(FILE *fp, int index){
    form *myForm = createFormByTree(workingPlace[index].root);
    writeTreeForm(fp, myForm);
}

void showChildDir(int index){
    int size = 0;
    int* buf = getChildId(workingPlace[index].cur, &size);
    for(int i = 0; i < size; i ++){
        printf("%s ", getFileById(index, buf[i])->name);
    }
    printf("\n");
}