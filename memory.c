//
// Created by Lih-pc on 2021-06-17.
//
/**
 * 用一个文件模拟磁盘空间，分为多个物理块，并通过显示链接分配的方式创建，保存文件
 * 结构：表，如图（物理块号、下一块）
 * 测试：打印物理存储空间，写文件，读文件
 */
#include <stdlib.h>
#include <string.h>
#include "include/form.h"
#include "include/memory.h"
#include "include/mysys.h"

extern discType *loadedDisc[MAX_LOAD_DISC];

void myDestroy(void *data){
    free(data);
}

void initBlocks(int maxSize, const char *name, int freeIndex) {
    loadedDisc[freeIndex] = (discType *)malloc(sizeof(discType));
    loadedDisc[freeIndex]->MaxBlockSize = maxSize;
    loadedDisc[freeIndex]->DiscName = (char *) malloc(sizeof(char) * MAX_NAME_SIZE);
    memcpy(loadedDisc[freeIndex]->DiscName, name, MAX_NAME_SIZE);
    loadedDisc[freeIndex]->BlockForm = formInit(myDestroy, maxSize);

    for(int i = 0; i < maxSize; i ++){
        int* data = (int *) malloc(sizeof(int));
        *data = -2;
        loadedDisc[freeIndex]->BlockForm->head[i] = data;
    }
}

#define addItem(i, data, index)         do{                                \
            *(int *)loadedDisc[i]->BlockForm->head[index] = data;          \
            if(data != -2) loadedDisc[i]->BlockForm->size ++;              \
        }while(0)

#define removeItem(i, index)         do{                                   \
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
    if(num != 0 && num % 6 == 0){
        printf("\n");
    }
    printf("ID:%-3d Next:%-3d  ", num, *(int *)data);
}

/**
 * 保存位视图
 * @param fp  要保存的磁盘文件
 * @param index 保存到哪个盘号
 */
void discLoadSave(FILE *fp, int index) {
//    fp = fopen(SYS_DISC, "w+");
//
//    fprintf(fp, "%d  ", LoadDiscSize);      //登记已经加载到挂载的磁盘
//    for(int i = 0; i < LoadDiscSize; i ++){
        fprintf(fp, "%24s  ", loadedDisc[index]->DiscName);
        fprintf(fp, "%d  ", loadedDisc[index]->BlockForm->maxSize);

        for(int j = 0; j < loadedDisc[index]->BlockForm->maxSize; j ++){
            fprintf(fp, "%d  ", *(int *)loadedDisc[index]->BlockForm->head[j]);
        }
//    }
//
//    fclose(fp);
}


/**
 * 加载位视图
 * @param fp  要加载的磁盘文件
 * @param index 加载到哪个盘号
 */
void discLoadRead(FILE *fp, int index) {
//    int loadingSize;
//    fscanf(fp, "%d  ", &loadingSize);    //读取已经加载到挂载的磁盘
//    for(int i = 0; i < loadingSize; i ++){
        char name[MAX_NAME_SIZE];
        int myBlockSize = 0;
        fscanf(fp, "%24s  ", name);
        fscanf(fp, "%d  ", &myBlockSize);

        initBlocks(myBlockSize, name, index);
        int j = 0;
        for(int z = 0; z < myBlockSize; z ++){
            int num;
            fscanf(fp, "%d  ", &num);
            addItem(index, num, z);
            if(num != -2){
                j ++;
            }
        }
//        loadedDisc[i]->BlockForm->size = j;
//    }
}

/**
 * 初始化磁盘文件（大小上）
 * @param name
 * @param blockSize
 * @param index
 */
void initDisc(const char *name, int blockSize, int index) {
    char buf[EACH_BLOCK_SIZE] = {0};
    FILE *fp = fopen(name, "w");

    fseek(fp, 0, SEEK_SET);
    int i;
    for(i = 0; i < blockSize; i ++) {
        fwrite(buf, EACH_BLOCK_SIZE, 1, fp);
    }

    initBlocks(blockSize, name, index);

    fclose(fp);
}

/**
 * 从磁盘中读出文件信息
 * @param blockNum 磁盘id
 * @param startBlock FAT 位视图的首个块
 * @return 整个文件的内容
 */
char *readFromMem(int blockId, int startBlock){
    char *buf;
    int i = 0;
    if(startBlock < 0){
        return NULL;
    }

    if(getItem(blockId, startBlock) == -2){
        return "Empty";
    }

    FILE *fp = fopen(loadedDisc[blockId]->DiscName, "r");

    buf = (char *)malloc(sizeof(char) * EACH_BLOCK_SIZE);
    while(1){
        //传递给realloc的指针必须是先前通过malloc(), calloc(), 或realloc()分配的
        buf = (char *) realloc(buf, (i + 1) * EACH_BLOCK_SIZE);
        memset(buf + i * EACH_BLOCK_SIZE, 0, EACH_BLOCK_SIZE);
        fseek(fp, startBlock * EACH_BLOCK_SIZE + BASE_OFF, SEEK_SET);
        fread(buf + i * EACH_BLOCK_SIZE, sizeof(char), EACH_BLOCK_SIZE, fp);
        i ++;
        if(getItem(blockId, startBlock) == 0){
            break;
        }
        startBlock = getItem(blockId, startBlock);
    }

    fclose(fp);
    return buf;
}

/**
 * 向磁盘中写入信息
 * @param size
 * @param content
 * @return 文件开始的物理块位置  -1分配失败
 */
int writeToMem(int index, int size, const char* content){
    int blockSize;
    blockSize = (size + (EACH_BLOCK_SIZE - 1)) / EACH_BLOCK_SIZE;
    if(blockSize == 0){
        return 0;    //大小为0的默认挂载到0块
    }

    FILE *fp;
    fp = fopen(loadedDisc[index]->DiscName, "rb+");

    if(loadedDisc[index]->BlockForm->size + blockSize >= loadedDisc[index]->MaxBlockSize){
        return -1;
    }
    int ret = -1, i = 0, pre = 0;
    for(int j = 0; j < blockSize; ){
        if((*(int *)loadedDisc[index]->BlockForm->head[i ++]) == -2){
            if(ret == -1){
                ret = i - 1;
                pre = i - 1;
            }
            fseek(fp, (i - 1) * EACH_BLOCK_SIZE + BASE_OFF, SEEK_SET);
            int len = strlen(content + j * EACH_BLOCK_SIZE);
            if(len < EACH_BLOCK_SIZE){
                fwrite(content + j * EACH_BLOCK_SIZE, len, 1, fp);
            }else{
                fwrite(content + j * EACH_BLOCK_SIZE, EACH_BLOCK_SIZE, 1, fp);
            }
            addItem(index, i - 1, pre);
            pre = i - 1;

            j ++;
        }
    }
    changeItem(index, 0, pre);

    fclose(fp);
    return ret;
}

/**
 * void debugForm(int discID)
 * @param discID 打印的磁盘id
 */
void debugForm(int discID){
    printf("disc name: %s\n", loadedDisc[discID]->DiscName);
    printf("----------------------------------------------------\n");
    formTraverse(loadedDisc[discID]->BlockForm, &traverseBlocks);
    printf("----------------------------------------------------\n");
}
//
//void testWriteRead(){
//    writeToMem(0, 100, "liheng");
//    writeToMem(0, 100, "lihengsdfsdfs");
//    writeToMem(0, 100, "lihengsdfsdfsd");
//
//    printf("cmd_read: %s\n", readFromMem("DISC", 0));
//    printf("cmd_read: %s\n", readFromMem("DISC", 2));
//    printf("cmd_read: %s\n", readFromMem("DISC", 4));
//
//    writeToMem(1, 100, "lihengsdfsdfsdfsd");
//    writeToMem(1, 100, "lihengsdfsdfs");
//    writeToMem(1, 100, "lihengsdfsdfsd");
//
//    printf("cmd_read: %s\n", readFromMem("DISC2", 0));
//    printf("cmd_read: %s\n", readFromMem("DISC2", 2));
//    printf("cmd_read: %s\n", readFromMem("DISC2", 4));
//}
