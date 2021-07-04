//
// Created by Lih-pc on 2021-06-22.
//

#ifndef FILESYSTEM_TREE_H
#define FILESYSTEM_TREE_H

#include "form.h"

typedef struct CSNode
{
    void *data;
    struct CSNode *firstChild, *rightSib, *parent;
    void (*destroy)(void *data);
}CSNode, *CSTree;

typedef struct{
    void *data;
    int parent;
}*pParFormItem, parFormItem;

CSTree treeInit(void (*destroy)(void *data));

void destroyTree(CSTree des);

void showChild(CSTree par, void (*callback)(const void *data));

void treeLTraverse(CSTree tree, void (*callback)(const void *data));

CSNode *newNode(void *data, CSNode *parent);

CSTree createTreeByForm(form *myForm, void (*destroy)(void *data));

form *createFormByTree(CSTree tree);

int getTreeRoot(CSTree tree);

int getNodeNum(CSTree tree);

void writeTreeForm(FILE *fp, form *myForm);

form *readTreeForm(FILE *fp);

int *getChildId(CSTree par, int *size);

#endif //FILESYSTEM_TREE_H
