//
// Created by Lih-pc on 2021-06-17.
//
/**
 * 用一个文件模拟磁盘空间，分为多个物理块，并通过显示链接分配的方式创建，保存文件
 * 结构：表，如图（物理块号、下一块）
 * 测试：打印物理存储空间，写文件，读文件
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/form.h"
#include "include/memory.h"
#include "include/mysys.h"

typedef struct discType{
    char *DiscName;
    form *BlockForm;
    int MaxBlockSize;
    fpos_t off;       //磁盘空间inode表的偏移量
}discType;

discType *loadedDisc[MAX_LOAD_DISC];

void myDestroy(void *data){
    free(data);
}

void initBlocks(int maxSize, const char *name){
    loadedDisc[LoadDiscSize] = (discType *)malloc(sizeof(discType));
    loadedDisc[LoadDiscSize]->MaxBlockSize = maxSize;
    loadedDisc[LoadDiscSize]->DiscName = (char *) malloc(sizeof(char) * MAX_NAME_SIZE);
    memcpy(loadedDisc[LoadDiscSize]->DiscName, name, MAX_NAME_SIZE);
    loadedDisc[LoadDiscSize]->BlockForm = formInit(myDestroy, maxSize);

    for(int i = 0; i < maxSize; i ++){
        int* data = (int *) malloc(sizeof(int));
        *data = -2;
        loadedDisc[LoadDiscSize]->BlockForm->head[i] = data;
    }
    LoadDiscSize ++;
}

#define addItem(i, data, index)         do{                    \
            *(int *)loadedDisc[i]->BlockForm->head[index] = data;          \
            loadedDisc[i]->BlockForm->size ++;                             \
        }while(0)

#define removeItem(i, index)         do{                       \
            *(int *)loadedDisc[i]->BlockForm->head[index] = -2;            \
            loadedDisc[i]->BlockForm->size --;                             \
        }while(0)

#define getItem(i, index)         (*(int *)loadedDisc[i]->BlockForm->head[index])

#define changeItem(i, data, index)   *(int *)loadedDisc[i]->BlockForm->head[index] = data

int findBlockIdByName(const char *name){
    int blockNum;
    for(blockNum = 0; blockNum < LoadDiscSize; blockNum ++){
        if(!strcmp(loadedDisc[blockNum]->DiscName, name)){
            return blockNum;
        }
    }
    return -1;
}

void releaseFile(const char* name, int index){
    int blockNum = findBlockIdByName(name);
    int next = getItem(blockNum, index);
    while(getItem(blockNum, index) != 0){
        next = getItem(blockNum, index);
        removeItem(blockNum, index);
        index = next;
    }
    removeItem(blockNum, next);
}

void traverseBlocks(const void *data, int num){
    if(*(int *)data != -2){
        printf("ID:%d Next:%d\n", num, *(int *)data);
    }
}

void discLoadStore(){
    FILE *fp;
    fp = fopen(SYS_DISC, "w+");

    fprintf(fp, "%d  ", LoadDiscSize);      //登记已经加载到挂载的磁盘
    for(int i = 0; i < LoadDiscSize; i ++){
        fprintf(fp, "%24s  ", loadedDisc[i]->DiscName);
        fprintf(fp, "%d  ", loadedDisc[i]->BlockForm->maxSize);

        for(int j = 0; j < loadedDisc[i]->BlockForm->maxSize; j ++){
            fprintf(fp, "%d  ", *(int *)loadedDisc[i]->BlockForm->head[j]);
        }
    }

    fclose(fp);
}

/**
 * int discLoadRead() 应该在系统加载初始就调用
 * @return -1 没有装载的盘 1 正常
 */
int discLoadRead(){
    FILE *fp;
    fp = fopen(SYS_DISC, "r");

    if(fp == NULL){
        return -1;
    }
    rewind(fp);
    int loadingSize;
    fscanf(fp, "%d  ", &loadingSize);    //读取已经加载到挂载的磁盘
    for(int i = 0; i < loadingSize; i ++){
        char name[MAX_NAME_SIZE];
        int myBlockSize = 0;
        fscanf(fp, "%24s  ", name);
        fscanf(fp, "%d  ", &myBlockSize);

        initBlocks(myBlockSize, name);
        int j = 0;
        for(int z = 0; z < myBlockSize; z ++){
            int num;
            fscanf(fp, "%d  ", &num);
            addItem(i, num, z);
            if(num != -2){
                j ++;
            }
        }
        loadedDisc[i]->BlockForm->size = j;
    }

    fclose(fp);
    return 1;
}

int initDisc(const char *name, int blockSize){
    char buf[EACH_BLOCK_SIZE] = {0};
    FILE *fp = fopen(name, "wb");

    fseek(fp, 0, SEEK_SET);
    int i;
    for(i = 0; i < blockSize; i ++){
        fwrite(buf, EACH_BLOCK_SIZE, 1, fp);
    }
    if(i == MAX_LOAD_DISC){
        return -1;
    }
    initBlocks(blockSize, name);

    fclose(fp);

    discLoadStore();

    return 1;
}

/**
 * createDisc(const char *name, int size)
 * @param name 磁盘名
 * @param size 磁盘大小（byte）
 * @return -1 无法分配（用尽）  1 正常
 */
int createDisc(const char *name, int size){
    return initDisc(name, size / EACH_BLOCK_SIZE);
}

/**
 * char *readFile(const char* name, int startBlock)
 * @param name 磁盘名
 * @param startBlock FAT 位视图的首个块
 * @return 整个文件的内容
 */
char *readFile(const char* name, int startBlock){
    char *buf;
    int i = 0;
    if(startBlock < 0){
        return NULL;
    }

    int blockNum = findBlockIdByName(name);
    if(getItem(blockNum, startBlock) == -2){
        return NULL;
    }

    FILE *fp = fopen(name, "r");

    buf = (char *)malloc(sizeof(char) * EACH_BLOCK_SIZE);
    while(1){
        //传递给realloc的指针必须是先前通过malloc(), calloc(), 或realloc()分配的
        buf = (char *) realloc(buf, (i + 1) * EACH_BLOCK_SIZE);
        memset(buf + i * EACH_BLOCK_SIZE, 0, EACH_BLOCK_SIZE);
        fseek(fp, startBlock * EACH_BLOCK_SIZE, SEEK_SET);
        fread(buf + i * EACH_BLOCK_SIZE, sizeof(char), EACH_BLOCK_SIZE, fp);
        i ++;
        if(getItem(blockNum, startBlock) == 0){
            break;
        }
        startBlock = getItem(blockNum, startBlock);
    }

    fclose(fp);

    discLoadStore();   //更新FAT

    return buf;
}

/**
 * @param size
 * @param content
 * @return 文件开始的物理块位置  -1分配失败
 */
int writeFile(const char *name, int size, const char* content){
    int blockSize;
    FILE *fp = fopen(name, "rb+");
    blockSize = (size + (EACH_BLOCK_SIZE - 1)) / EACH_BLOCK_SIZE;

    int blockNum = findBlockIdByName(name);

    if(loadedDisc[blockNum]->BlockForm->size + blockSize >= loadedDisc[blockNum]->MaxBlockSize){
        return -1;
    }
    int ret = -1, i = 0, pre = 0;
    for(int j = 0; j < blockSize; ){
        if((*(int *)loadedDisc[blockNum]->BlockForm->head[i ++]) == -2){
            if(ret == -1){
                ret = i - 1;
                pre = i - 1;
            }
            fseek(fp, (i - 1) * EACH_BLOCK_SIZE, SEEK_SET);
            int len = strlen(content + j * EACH_BLOCK_SIZE);
            if(len < EACH_BLOCK_SIZE){
                fwrite(content + j * EACH_BLOCK_SIZE, len, 1, fp);
            }else{
                fwrite(content + j * EACH_BLOCK_SIZE, EACH_BLOCK_SIZE, 1, fp);
            }
            addItem(blockNum, i - 1, pre);
            pre = i - 1;

            j ++;
        }
    }
    changeItem(blockNum, 0, pre);

    fclose(fp);

    discLoadStore();

    return ret;
}

/**
 * void debugForm(const char *name)
 * @param name 磁盘名
 */
void debugForm(const char *name){
    printf("disc name: %s\n", name);
    formTraverse(loadedDisc[findBlockIdByName(name)]->BlockForm, &traverseBlocks);
}

void testCreateDisc(){
    createDisc("DISC", 6400);
    createDisc("DISC2", 6400);
}

void testWriteRead(){
    writeFile("DISC", 100, "liheng");
    writeFile("DISC", 100, "lihengsdfsdfs");
    writeFile("DISC", 100, "lihengsdfsdfsd");

    printf("read: %s\n", readFile("DISC", 0));
    printf("read: %s\n", readFile("DISC", 2));
    printf("read: %s\n", readFile("DISC", 4));

    writeFile("DISC2", 100, "lihengsdfsdfsdfsd");
    writeFile("DISC2", 100, "lihengsdfsdfs");
    writeFile("DISC2", 100, "lihengsdfsdfsd");

    printf("read: %s\n", readFile("DISC2", 0));
    printf("read: %s\n", readFile("DISC2", 2));
    printf("read: %s\n", readFile("DISC2", 4));
}

void testReleaseFile(){
    writeFile("DISC", 100, "release ed");
    printf("before release read: %s\n", readFile("DISC", 0));
    releaseFile("DISC", 0);
    printf("after release read: %s\n", readFile("DISC", 0));
    debugForm("DISC");
}

//int main(){
//    discLoadRead();
//    return 0;
//}