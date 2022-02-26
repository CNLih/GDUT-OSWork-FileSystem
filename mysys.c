//
// Created by Lih-pc on 2021-06-22.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/mysys.h"
#include "include/file.h"
#include "include/shell.h"
#include "include/user.h"

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

void saveDisc(){
    FILE *fp;
    fp = fopen(SYS_DISC, "w");

    //考虑到可能卸载磁盘：
    int i = 0, saveN = LoadDiscSize;
    fprintf(fp, "%d ", LoadDiscSize);
    while(saveN){
        if(loadedDisc[i] != NULL){
            fprintf(fp, "%24s ", loadedDisc[i]->DiscName);
            FILE *disc = fopen(loadedDisc[i]->DiscName, "rb+");
            //保存物理块使用情况
            discLoadSave(disc, i);
            //保存inode表
            saveInode(disc, i);
            //保存目录结构
            savePath(disc, i);
            fclose(disc);
            saveN --;
        }
        if(i ++ == LoadDiscSize){
            break;
        }
    }
    saveUser(fp);
    fclose(fp);
}

/**
 * 单独SYS_DISC
 */
void updateConfig(){
    FILE *fp;
    fp = fopen(SYS_DISC, "w");

    //考虑到可能卸载磁盘：
    int i = 0, saveN = LoadDiscSize;
    fprintf(fp, "%d ", LoadDiscSize);
    while(saveN){
        if(loadedDisc[i] != NULL){
            fprintf(fp, "%24s ", loadedDisc[i]->DiscName);
            saveN --;
        }
        if(i ++ == LoadDiscSize){
            break;
        }
    }
    saveUser(fp);
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
    //重新加载磁盘
    saveDisc();
    loadDisc();
    return 1;
}

/**
 * 加载磁盘
 * @return -1配置文件 1正常
 */
int loadDisc(){
    FILE *fp;
    fp = fopen(SYS_DISC, "r");
    int n = 0;

    sysParaInit();
    if(fp == NULL){
        return -1;  //无配置文件
    }
    if(fscanf(fp, "%d ", &n) == EOF || n == 0){
        return -1;  //无配置文件
    }

    for(int i = 0; i < n; i ++){
        char buf[MAX_NAME_SIZE];
        fscanf(fp, "%24s ", buf);

        FILE *disc = fopen(buf, "r");
        if(disc == NULL){
            continue;  //无磁盘文件
        }
        newInode(i);
        LoadDiscSize ++;
        //读取物理块使用情况
        discLoadRead(disc, i);
        //读取inode表
        readInode(disc, i);
        //读取目录信息
        readPath(disc, i);
        fclose(disc);
    }
    if(LoadDiscSize == 0){
        return -1;
    }

    readUser(fp);
    fclose(fp);

    if(workingDisc == -1){
        for(int i = 0; i < n; i ++){
            if(loadedDisc[i] != NULL){
                workingDisc = i;
            }
        }
        useDisc(workingDisc);
    }
    return 1;
}

/**
 * 写入新文件
 * @param index 盘号
 * @param size 期望文件的大小
 * @param content
 */
void writeFile(const char *name, int index, int size, const char *content, int pri) {
    //向磁盘中写入文件
    int begN = writeToMem(index, size, content);
    //文件写入inode表
    int id = workingPlace[index].maxId + 1;
    newFile(index, id, size, begN, name, pri);
    workingPlace[index].maxId ++;
    //写入目录中
    addFileNode(id, index);
    saveDisc();
}

/**
 * 登录或者切换用户
 * @param name
 * @param password
 * @return -1 -- Error pass    -2 -- No found
 */
int login(const char *name, const char *password){
    for(int i = 0; i < usersN; i ++){
        if(strcmp(users[i], name) == 0){
            if(strcmp(pass[i], password) == 0){
                CurUserId = users_id[i];
                strcpy(CurUserName, users[i]);
                return 1;
            }
            return -1;
        }
    }
    return -2;
}

void sysParaInit(){
    LoadDiscSize = 0;
    for(int i = 0; i < MAX_LOAD_DISC; i ++){
        if(loadedDisc[i] != NULL){
            loadedDisc[i]->BlockForm->destroy(loadedDisc[i]->BlockForm->head);
            free(loadedDisc[i]->BlockForm);
        }

        if(loadedInode[i] != NULL){
            if(loadedInode[i]->head != NULL){
                loadedInode[i]->destroy(loadedInode[i]);
                free(loadedInode[i]);
            }
        }

        workingPlace[i].childFile = NULL;
        workingPlace[i].childNum = 0;
        workingPlace[i].maxId = 0;
    }
    memset(loadedDisc, 0, sizeof(loadedDisc));
    workingDisc = -1;
}

/**
 * 更新全局变量workingPlace中的childFile（存储子文件id）
 * @param index
 */
void updateChildFile(int index){
    for(int i = 0; i < workingPlace->childNum; i ++){
        free(workingPlace->childFile[i]);
    }
    int *childIds = getChildId(workingPlace[index].cur, &workingPlace[index].childNum);
    workingPlace[index].childFile = (inodeItem **) malloc(sizeof(inodeItem *) * workingPlace[index].childNum);
    for(int i = 0; i < workingPlace[index].childNum; i ++){
        workingPlace[index].childFile[i] = getFileById(index, childIds[i]);
    }
}

void useDisc(int index){
    workingDisc = index;
    updateChildFile(workingDisc);
    memcpy(workingPlace[workingDisc].curFolder, loadedDisc[workingDisc]->DiscName, MAX_NAME_SIZE);
    strcpy(workingDir, loadedDisc[workingDisc]->DiscName);
    strcpy(workingDir + strlen(workingDir), ":");
}

int main(){
    printf("Welcome To My File System\n");
    sysParaInit();
    userPreInit();
    loadDisc();
    shellRun();
}
