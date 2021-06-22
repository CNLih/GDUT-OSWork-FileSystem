//
// Created by Lih-pc on 2021-06-22.
//

#include <stdio.h>
#include <string.h>
#include "include/mysys.h"
#include "include/file.h"

void saveDisc(){
    FILE *fp;
    fp = fopen(SYS_DISC, "w");

    //考虑到可能卸载磁盘：
    int i = 0, saveN = LoadDiscSize;
    fprintf(fp, "%d ", LoadDiscSize);
    while(saveN){
        if(i == 10){
            printf("bug? LoadDiscSize与实际不符\n");
            return ;
        }
        if(loadedDisc[i] != NULL){
            fprintf(fp, "%24s ", loadedDisc[i]->DiscName);
            FILE *disc = fopen(loadedDisc[i]->DiscName, "rb+");
            //保存物理块使用情况
            discLoadSave(disc, i);
            //保存inode表
            saveInode(disc);
            fclose(disc);
            saveN --;
        }
        if(i ++ == LoadDiscSize){
            break;
        }
    }
    fclose(fp);
}

/**
 * createDisc(const char *name, int size)
 * @param name 磁盘名
 * @param size 磁盘大小（kb）
 * @return -1 挂载盘数已达上限  1 正常  -2 创建的太大了，可能用完文件数目都不能填满
 */
int createDisc(const char *name, int size){
    if(size > 1024){
        return -2;
    }
    if(LoadDiscSize == MAX_LOAD_DISC){      //挂载数目达到上限
        return -1;
    }
    int i;
    for(i = 0; i < MAX_LOAD_DISC; i ++){
        if(loadedDisc[i] == NULL){
            break;
        }
    }
    initDisc(name, size, i);
    newInode(i);
    LoadDiscSize ++;
    saveDisc();
    return 1;
}

/**
 * 加载磁盘
 * @return -1配置文件 -2无磁盘文件 1正常
 */
int loadDisc(){
    FILE *fp;
    fp = fopen(SYS_DISC, "r");
    int n;

    if(fp == NULL){
        return -1;  //无配置文件
    }
    fscanf(fp, "%d ", &n);
    for(int i = 0; i < n; i ++){
        char buf[MAX_NAME_SIZE];
        fscanf(fp, "%24s ", buf);

        newInode(i);
        LoadDiscSize ++;

        FILE *disc = fopen(buf, "r");
        if(disc == NULL){
            return -2;  //无磁盘文件
        }
        //读取物理块使用情况
        discLoadRead(disc, i);
        //读取inode表
        readInode(disc);
        fclose(disc);
    }
    fclose(fp);
    return 1;
}

/**
 * 写入新文件
 * @param index 盘号
 * @param size 期望文件的大小
 * @param content
 */
void writeFile(int index, int size, const char* content){
    //向磁盘中写入文件
    int begN = writeToMem(index, size, content);
    //文件写入inode表
    newInode(0);
    newFile(index, 0, sizeof(content), begN);
    saveDisc();
}

void sysParaInit(){
    LoadDiscSize = 0;
    memset(loadedDisc, 0, sizeof(loadedDisc));
}

int main(){
    sysParaInit();
//    createDisc("DISC_A", 100);
//    saveDisc();
    loadDisc();
    writeFile(0, 100, "liheng");
    debugForm("DISC_A");
    printf("read: %s\n", readFromMem("DISC_A", 0));
    printf("read: %s\n", readFromMem("DISC_A", 2));
    printf("read: %s\n", readFromMem("DISC_A", 4));
}
