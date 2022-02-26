//
// Created by Lih-pc on 2021-06-26.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/user.h"
#include "include/mysys.h"

char **users;
char **pass;
int *users_id;

int usersN;
int maxUserId;

/**
 * 从配置文件读取用户信息
 * @param fp
 */
void readUser(FILE *fp){
    fscanf(fp, "%d ", &usersN);
    users_id = (int *) malloc(sizeof(int) * MAX_USER_N);
    users = (char **)malloc(sizeof(char *) * usersN);
    pass = (char **)malloc(sizeof(char *) * usersN);
    maxUserId = 0;
    for(int i = 0; i < usersN; i ++){
        users[i] = (char *) malloc(sizeof(char) * MAX_USER_SIZE);
        pass[i] = (char *) malloc(sizeof(char) * MAX_PASS_SIZE);
        fscanf(fp, "%d ", &users_id[i]);
        fscanf(fp, "%24s ", users[i]);
        fscanf(fp, "%24s ", pass[i]);
        if(maxUserId < users_id[i]){
            maxUserId = users_id[i];
        }
    }
}

/**
 * 将用户信息保存到配置文件
 * @param fp
 */
void saveUser(FILE *fp){
    if(usersN == 0){
        newUser("root", "root");
    }
    fprintf(fp, "%d ", usersN);
    for(int i = 0; i < usersN; i ++){
        fprintf(fp, "%d ", users_id[i]);
        fprintf(fp, "%24s ", users[i]);
        fprintf(fp, "%24s ", pass[i]);
    }
}

/**
 * 新建用户
 * @param name
 * @param password
 * @return 1 -- 正常  -1 -- 用户数达到上限  -2 -- 已存在
 */
int newUser(const char *name, const char *password){
    if(usersN + 1 >= MAX_USER_N){
        return -1;
    }
    for(int i = 0; i < usersN; i ++){
        if(strcmp(users[i], name) == 0){
            return -2;
        }
    }
    usersN ++;
    users = realloc(users, sizeof(char *) * usersN);
    pass = realloc(pass, sizeof(char *) * usersN);
    users_id = realloc(users_id, sizeof(int) * usersN);
    users[usersN - 1] = (char *) malloc(sizeof(char) * MAX_USER_SIZE);
    pass[usersN - 1] = (char *) malloc(sizeof(char) * MAX_PASS_SIZE);
    strcpy(users[usersN - 1], name);
    strcpy(pass[usersN - 1], password);
    maxUserId ++;
    users_id[usersN - 1] = maxUserId;
    return 1;
}

void userPreInit(){
    CurUserId = -1;
}