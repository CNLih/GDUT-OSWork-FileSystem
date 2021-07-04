//
// Created by Lih-pc on 2021-06-22.
//

/**
 * 通过孩子兄弟表示法读取到内存，通过双亲表示法将树结构存储到物理空间
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/tree.h"

CSTree treeInit(void (*destroy)(void *data)){
    CSTree tree = (CSTree) malloc(sizeof(CSNode));
    tree->parent = NULL;
    tree->rightSib = NULL;
    tree->firstChild = NULL;
    tree->destroy = destroy;
    return tree;
}

void destroyTree(CSTree des){
    des->destroy(des->data);
}

void showChild(CSTree par, void (*callback)(const void *data)){
    if(par == NULL){
        return;
    }
    CSTree tra = par->firstChild;
    while(tra != NULL){
        callback(tra->data);
        tra = tra->rightSib;
    }
    printf("\n");
}

int *getChildId(CSTree par, int *size){
    int *buf = (int *)malloc(sizeof(int) * 10);
    int use = 0;
    if(par == NULL){
        *size = use;
        return NULL;
    }
    CSTree tra = par->firstChild;
    while(tra != NULL){
        buf[use] = *(int *)tra->data;
        use ++;
        if(!(use % 10)){
            buf = realloc(buf, sizeof(int) * (use + 10));
        }
        tra = tra->rightSib;
    }
    *size = use;
    return buf;
}

void treeLTraverse(CSTree tree, void (*callback)(const void *data)){
    CSTree tra = tree->firstChild;

    while(tra != NULL){
        callback(tra->data);
        tra = tra->rightSib;
    }
    tra = tree->firstChild;
    while(tra != NULL){
        printf("\n");
        treeLTraverse(tra, callback);
        tra = tra->rightSib;
    }
}

CSNode *newNode(void *data, CSNode *parent){
    CSNode *node = (CSNode *) malloc(sizeof(CSNode));
    node->data = data;
    node->parent = parent;
    node->rightSib = NULL;
    node->firstChild = NULL;
//    printf("here2\n");
//    if(parent == NULL){
//        printf("here3\n");
//    }
    CSNode *tra = parent->firstChild;
    if(tra == NULL){
        parent->firstChild = node;
        return node;
    }
    while(tra->rightSib != NULL){
        tra = tra->rightSib;
    }
    tra->rightSib = node;
    return node;
}

int getTreeRoot(CSTree tree){
    if(tree == NULL){
        return 1;
    }
    if(tree->firstChild == NULL){
        return 0;
    }
    else{
        CSTree tra = tree->firstChild;
        int ret = 1;
        while(tra != NULL){
            ret += getTreeRoot(tra);
            tra = tra->rightSib;
        }
        return ret;
    }
}

int getNodeNum(CSTree tree){
    if(tree == NULL){
        return 0;
    }else{
        CSTree tra = tree->firstChild;
        int ret = 1;
        while(tra != NULL){
            ret += getNodeNum(tra);
            tra = tra->rightSib;
        }
        return ret;
    }
}

void *copyData(void *orData){
    void *dat = (void *)malloc(sizeof(orData));
    memcpy(dat, orData, sizeof(orData));
    return dat;
}

void treeTestDestroy(void *data){
    free(data);
}

void treeTestPrint(const void *data){
    int dat = *(int *)data;
    printf("%d ", dat);
}

CSTree createTreeByForm(form *myForm, void (*destroy)(void *data)){
    //size代表这棵树最大的parent在表格中的角标
    //malloc会在run的时候产生空指针错误（实际运行可以）不知道为什么？
//    CSTree *trees = (CSTree *) malloc(sizeof(CSTree) * (myForm->size));
    CSTree trees[myForm->size + 1];
    CSTree root;
    trees[0] = treeInit(treeTestDestroy);

    for(int i = 1; i < myForm->maxSize; i ++){
        int parN = ((pParFormItem)(myForm->head[i]))->parent;
        //当i是别人的双亲，加入到trees中，方便孩子找家  自己的双亲一定在trees中，不然就是bug数据
        CSNode *node = newNode(copyData(((pParFormItem)(myForm->head[i]))->data), trees[parN]);
//        printf("debug : %d \n", *(int *)node->data);
        if(i < myForm->size && trees[i] != NULL){
            trees[i] = node;
        }
    }
    root = trees[0];
    return root;
}

void bfs(CSTree tree, form *myForm, int par, int left, int *right){
    if(tree == NULL){
        return ;
    }
    CSNode *node = tree->firstChild;
    while(node != NULL){
        pParFormItem item = (pParFormItem) malloc(sizeof(parFormItem));
        item->data = copyData(node->data);
        int dat = *(int *)item->data;
//        printf("debug : %d \n", dat);
        item->parent = par;
        if(node->firstChild == NULL){
            myForm->head[*right] = item;
            *right = *right + 1;
        }else{
            myForm->head[left ++] = item;
            bfs(node, myForm, left - 1, left, right);
        }
        node = node->rightSib;
    }
}

void destroyParForm(void *data){
    free(((pParFormItem)data)->data);
    free(data);
}

void formTra(const void *data, int num){
    if(num == 0){
        return ;
    }
    printf("%d, %d  ", num, *(int *)((pParFormItem)data)->data);
}

void writeTreeForm(FILE *fp, form *myForm){
    fprintf(fp, "%d ", myForm->maxSize);
    for(int i = 1; i < myForm->maxSize; i ++){
        fprintf(fp, "%d %d ", *(int *)((pParFormItem)myForm->head[i])->data, ((pParFormItem)myForm->head[i])->parent);
    }
}

form *readTreeForm(FILE *fp){
    int size = 0, maxSize;
    fscanf(fp, "%d ", &maxSize);
    form *myForm = formInit(destroyParForm, maxSize);
    for(int i = 1; i < maxSize; i ++){
        pParFormItem item = (pParFormItem) malloc(sizeof(parFormItem));
        item->data = (void *) malloc(sizeof(int));
        fscanf(fp, "%d %d ", (int *)item->data, &item->parent);
        if(size < (int)item->parent){
            size = (int)item->parent;
        }
        myForm->head[i] = item;
    }
    myForm->size = size + 1;
    return myForm;
}

form *createFormByTree(CSTree tree){
    int nodeN = getNodeNum(tree);
    int rootN = getTreeRoot(tree);
    form *treeForm = formInit(destroyParForm, nodeN);
    treeForm->size = rootN;
    bfs(tree, treeForm, 0, 1, &rootN);
    return treeForm;
}

CSTree createTreeTest(){
    CSTree tree = treeInit(treeTestDestroy);

    int data[10] = {0,1,2,3,4,5,6,7,8,9};
    for(int i = 1; i < 4; i ++){
        newNode(&data[i], tree);
    }
    for(int i = 5; i < 8; i ++){
        newNode(&data[i], tree->firstChild);
    }
    showChild(tree, treeTestPrint);
    treeLTraverse(tree, treeTestPrint);
    printf("root : %d", getTreeRoot(tree));
    printf("all : %d", getNodeNum(tree));
    return tree;
}

void treeTest(){
    //tree Test
    FILE *fp = fopen("testTree.txt", "r");
    form *myForm = readTreeForm(fp);
    fclose(fp);

//    CSTree myTree = createTreeTest();
//    form *myForm = createFormByTree(myTree);
//    formTraverse(myForm, formTra);
    CSTree tree1 = createTreeByForm(myForm, treeTestDestroy);
    showChild(tree1, treeTestPrint);
    treeLTraverse(tree1, treeTestPrint);
    fp = fopen("testTree.txt", "w+");
    writeTreeForm(fp, myForm);
    fclose(fp);
    printf("root : %d", getTreeRoot(tree1));
    printf("all : %d", getNodeNum(tree1));
}
