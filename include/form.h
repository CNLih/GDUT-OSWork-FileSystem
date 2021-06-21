//
// Created by Lih-pc on 2021-06-17.
//

#ifndef FILESYSTEM_FORM_H
#define FILESYSTEM_FORM_H

typedef struct formNode{
    void *data;
}formNode;

typedef struct form{
    int maxSize;
    int size;
    void **head;
    void (*match)(const void *key1, const void *key2);
    void (*destroy)(void *data);
}form;

form *formInit(void (*destroy)(void *data), int maxSize);

void formTraverse(form *myForm, void (*callback)(const void *data, int num));

#endif //FILESYSTEM_FORM_H
