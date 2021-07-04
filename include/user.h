//
// Created by Lih-pc on 2021-06-26.
//

#ifndef FILESYSTEM_USER_H
#define FILESYSTEM_USER_H

#define MAX_USER_SIZE   24
#define MAX_PASS_SIZE   24
#define MAX_USER_N      99

typedef struct {
    int ownerId;
    int pri;    //7 - w/r/e
}pri_info;

char **users;
char **pass;
int *users_id;

int usersN;
int maxUserId;

void readUser(FILE *fp);

void saveUser(FILE *fp);

int newUser(const char *name, const char *password);

void userPreInit();

#endif //FILESYSTEM_USER_H
