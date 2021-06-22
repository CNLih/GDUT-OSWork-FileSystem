//
// Created by Lih-pc on 2021-06-22.
//

#include <stdio.h>
#include "include/mysys.h"
#include "include/form.h"
#include "include/file.h"
#include "include/memory.h"

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
        fscanf(fp, "%s ", buf);
        FILE *disc = fopen(buf, "r");
        if(disc == NULL){
            return -2;  //无磁盘文件
        }
        //读取物理块使用情况

    }
}