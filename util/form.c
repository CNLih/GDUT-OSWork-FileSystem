//
// Created by Lih-pc on 2021-06-17.
//

#include "../include/form.h"
#include <stdlib.h>

/**
 * @param destroy
 * @param maxSize   定义表格最多行数
 * @return
 */
form *formInit(void (*destroy)(void *data), int maxSize){
    form *myForm;

    if((myForm = (form * )malloc(sizeof(form))) == NULL){
        return NULL;
    }
    myForm->maxSize = maxSize;
    if((myForm->head = (void **)malloc(sizeof(void *) * maxSize)) == NULL){
        return NULL;
    }
    myForm->size = 0;
    myForm->destroy = destroy;

    return myForm;
}

void formTraverse(form *myForm, void (*callback)(const void *data, int num)){
    for(int i = 0; i < myForm->maxSize; i ++){
        callback(myForm->head[i], i);
    }
}
