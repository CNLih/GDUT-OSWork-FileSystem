//
// Created by Lih-pc on 2021-06-24.
//

#include <string.h>
#include <stdlib.h>
#include "include/shell.h"
#include "include/mysys.h"

const char *shellFunc[FUNC_NUM] = {"help", "debug_mem", "ls", "cd", "mkdir", "write", "read", "disc", "use", "login", "exit"};
const char helpMess[FUNC_NUM][128] = {
        "[use: help]",
        "[use: debug_mem]",
        "[use: ls [-l]]",
        "[use: cd [./../file name]]",
        "[use: mkdir [name]]",
        "[use: write [name] (--size) [content]]",
        "[use: read [name]",
        "[use: disc [name] [size(*1024b)]",
        "[use: use [disc name]]",
        "[use: login [name] [password]]",
        "[use: exit]"
};
enum FUNC_LIST{
    HELP,
    MEM,
    LS,
    CD,
    MKDIR,
    WRITE,
    READ,
    DISC,
    USE,
    LOGIN,
    EXIT
};

/**
 * cmd_ls
 * @param fun 1 -- 打印文件名  2 -- 打印文件详细信息
 * @return
 */
void cmd_ls(int fun){
    switch (fun) {
        case 1:
            for(int i = 0; i < workingPlace[workingDisc].childNum; i ++){
                printf("%s ", workingPlace[workingDisc].childFile[i]->name);
            }
            printf("\n");
            break;
        case 2:
            printf("name        size    block   privilege\n");
            for(int i = 0; i < workingPlace[workingDisc].childNum; i ++){
                printf("%-12s", workingPlace[workingDisc].childFile[i]->name);
                printf("%-8d", workingPlace[workingDisc].childFile[i]->fileSize);
                printf("%-8d", workingPlace[workingDisc].childFile[i]->blockSize);
                printf("%d", workingPlace[workingDisc].childFile[i]->privilege);
                printf("\n");
            }
            break;
    }
}

/**
 * cmd_cd
 * @param fun 1 -- 返回跟目录  2 -- 返回上级目录  3 -- 进入到name文件夹
 * @param name
 * @return -1 -- 已经在根目录  -2 -- 未找到  -3 -- 非目录文件
 */
int cmd_cd(int fun, char *name){
    switch (fun) {
        case 1:
            workingPlace[workingDisc].cur = workingPlace[workingDisc].root;
            strcpy(workingDir, loadedDisc[workingDisc]->DiscName);
            strcpy(workingDir + strlen(workingDir), ":");
            memcpy(workingPlace[workingDisc].curFolder, loadedDisc[workingDisc]->DiscName, MAX_NAME_SIZE);;
            break;
        case 2:
            if(workingPlace[workingDisc].cur == workingPlace[workingDisc].root){
                return -1;
            }
            workingPlace[workingDisc].cur = workingPlace[workingDisc].cur->parent;
            if(workingPlace[workingDisc].cur == workingPlace[workingDisc].root){
                cmd_cd(1, NULL);
                break;
            }
            int len = strlen(workingPlace[workingDisc].curFolder);
            workingDir[strlen(workingDir) - len - 1] = '\0';
            strcpy(workingPlace[workingDisc].curFolder, workingDir + strlen(workingDir) + 1);
            break;
        case 3:  {//大括号因为编译器不允许case第一行定义
            int i = 0;
            for(i = 0; i < workingPlace[workingDisc].childNum; i ++){
                if(strcmp(workingPlace[workingDisc].childFile[i]->name, name) == 0){
                    if(workingPlace[workingDisc].childFile[i]->fileSize != 0){
                        return -3;
                    }
                    CSTree tra = workingPlace[workingDisc].cur->firstChild;
                    strcpy(workingPlace[workingDisc].curFolder, name);
                    int len = strlen(workingDir);
                    workingDir[len] = '/';
                    workingDir[len + 1] = '\0';
                    strcpy(workingDir + strlen(workingDir), name);
                    while(tra != NULL){
                        if(*(int *)tra->data == workingPlace[workingDisc].childFile[i]->id){
                            workingPlace[workingDisc].cur = tra;
                            break;
                        }
                    }
                    break;
                }
            }
            if(i == workingPlace[workingDisc].childNum){
                return -2;
            }
            break;
        }
    }
    updateChildFile(workingDisc);
    return 1;
}

void cmd_mkdir(char *name){
    writeFile(name, workingDisc, 0, NULL, 7);
    updateChildFile(workingDisc);
}

int cmd_write(char *name, char *content, int size, int pri) {
    if(size == 0){
        size = strlen(content);
    }
    writeFile(name, workingDisc, size, content, pri);
    updateChildFile(workingDisc);
}

/**
 *
 * @param name
 * @param size
 */
void newDisc(char *name, int size){
    if(size < 10 || size > 1000){
        printf("please create disc size between [10, 1000]");
        return;
    }
    for(int i = 0; i < LoadDiscSize; i ++){
        if(strcmp(loadedDisc[i]->DiscName, name) == 0){
            printf("disc already exist!");
            return;
        }
    }
    createDisc(name, size);
}

/**
 * cmd_read
 * @param name
 * @return -1 -- is folder  -2 -- no such file
 */
int cmd_read(char *name){
    for(int i = 0; i < workingPlace[workingDisc].childNum; i ++){
        if(strcmp(workingPlace[workingDisc].childFile[i]->name, name) == 0){
            if(workingPlace[workingDisc].childFile[i]->fileSize == 0){
                return -1;
            }
            printf("%s:\n", name);
            printf("%s\n", readFromMem(loadedDisc[workingDisc]->DiscName, workingPlace[workingDisc].childFile[i]->startBlock));
            return 1;
        }
    }
    return -2;
}

int checkFileExit(const char *name){
    inodeItem **item = workingPlace[workingDisc].childFile;
    for(int i = 0; i < workingPlace[workingDisc].childNum; i ++){
        if(strcmp(item[i]->name, name) == 0){
            return 1;
        }
    }
    return 0;
}

void printErr(int index){
    printf("Error: Unknown command\n");
    printf("%s\n", helpMess[index]);
}

void printHelp(int index){
    if(index == 0){  //help
        for(int i = 0; i < FUNC_NUM; i ++){
            printf("%s\n", helpMess[i]);
        }
    }else{
        printf("%s\n", helpMess[index]);
    }
}

/**
 * 运行shell
 */
void shellRun(){
    char inputBuf[512];
    char argv[4][128];

    while(1){
        printf("%s  ", workingDir);
        memset(inputBuf, 0, sizeof(inputBuf));
        for(int i = 0; i < 4; i ++){
            memset(argv[i], 0, sizeof(argv[i]));
        }
        gets(inputBuf);
        int len = 0;
        int argc = 0;
        while(sscanf(inputBuf + len, "%128s ", argv[argc]) != EOF){
            len += strlen(argv[argc]) + 1;
            argc ++;
        }
        int choice;
        for(choice = 0; choice < FUNC_NUM; choice ++){
            if(strcmp(shellFunc[choice], argv[0]) == 0){
                break;
            }
        }
        if(workingDisc == -1){
            if(choice != DISC){
                printf("You have no disc loaded\n");
                printf("use disc to create a new disc\n");
                printHelp(DISC);
                continue;
            }
        }else if(CurUserId == -1){
            if(choice != LOGIN){
                printf("You should login first\n");
                printf("use login to login\n");
                printHelp(LOGIN);
                continue;
            }
        }
        //write 特殊处理
        if(strcmp(argv[0], "write") == 0){
            char name[30], inputSize[10];
            int lenN = 0;
            sscanf(inputBuf + 6, "%30s", name);
            lenN = strlen(name);
            if(checkFileExit(name)){
                printf("Error: file already exit!\n");
                continue;
            }
            if(sscanf(inputBuf + 6 + lenN + 1, "%10s", inputSize) != EOF){
                if(lenN > 24){
                    printf("Error: file name should under 24 chars\n");
                    continue;
                }
                if((argv[2][0] == argv[2][1]) && (argv[2][0] == '-')){
                    lenN += strlen(name) + 1;
                }
                cmd_write(name, inputBuf + 6 + lenN + 1, atoi(argv[2] + 2), 7);
                continue;
            }
            printErr(choice);
            continue;
        }
        //一般处理命令:
        switch(choice){
            case HELP:
                printHelp(choice);
                break;
            case MEM:
                debugForm(workingDisc);
                break;
            case LS:
                if(argc > 2){
                    printErr(choice);
                }else if(argc == 1){
                    cmd_ls(1);
                }else if(argc == 2 && (strcmp(argv[1], "-l") == 0)){
                    cmd_ls(2);
                }
                break;
            case CD:{
                int ret = 0;
                if (argc != 2) {
                    printErr(choice);
                } else if (strcmp(argv[1], "..") == 0) {
                    ret = cmd_cd(2, NULL);
                } else if (strcmp(argv[1], ".") == 0) {
                    ret = cmd_cd(1, NULL);
                } else {
                    ret = cmd_cd(3, argv[1]);
                }
                switch (ret) {
                    case -1:
                        printf("Already in root\n");
                        break;
                    case -2:
                        printf("Folder no found\n");
                        break;
                    case -3:
                        printf("File can't open\n");
                        break;
                }
                break;
            }
            case MKDIR:
                if(argc != 2) {
                    printErr(choice);
                }else{
                    if(checkFileExit(argv[1])){
                        printf("Error: file already exit!\n");
                        break;
                    }
                    cmd_mkdir(argv[1]);
                }
                break;
            case WRITE://cmd_write 特殊处理在上面
                break;
            case READ:
                if(argc != 2){
                    printErr(choice);
                }else{
                    switch (cmd_read(argv[1])) {
                        case -1:
                            printf("file can't open\n");
                            break;
                        case -2:
                            printf("no found\n");
                            break;
                    }
                }
                break;
            case DISC:
                if(argc == 3 && atoi(argv[2])){
                    createDisc(argv[1], atoi(argv[2]));
                }else{
                    printErr(choice);
                }
                break;
            case USE:
                if(argc == 2){
                    int tra;
                    for(tra = 0; tra < LoadDiscSize; tra ++){
                        if(strcmp(loadedDisc[tra]->DiscName, argv[1]) == 0){
                            break;
                        }
                    }
                    if(tra == workingDisc){
                        printf("You already in the target disc\n");
                    }else if(tra == LoadDiscSize){
                        printf("Cant find target disc\n");
                    }else{
                        useDisc(tra);
                    }
                    break;
                }else{
                    printErr(choice);
                }
                break;
            case LOGIN:{
                int ret;
                if(argc != 3){
                    printErr(choice);
                }else{
                    ret = login(argv[1], argv[2]);
                }
                switch (ret) {
                    case -1:
                        printf("Error: password error\n");
                        break;
                    case -2:
                        printf("Error: user not found\n");
                        break;
                }
                break;
            }
            case EXIT:
                printf("bye");
                return ;
            default:
                printf("Unknown command\n");
        }
    }
}
