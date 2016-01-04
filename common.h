
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <semaphore.h>

#include <string>
#include <map>
#include <list>
#include <algorithm>
using namespace std;

typedef struct USER
{
    int fd; // socket
    string username; 
    string ipaddr;
} USER;

typedef struct BUFFER
{
    int fd;   // socket
    string buf;  // 字符串部分内容
    short total_size; // 字符串部分内容应该有的尺寸
    short recv_size; // 一共收到的尺寸
} BUFFER;

extern list<int> clients;
extern map<int, BUFFER*> buffers; // 缓存的数据
extern map<string, USER*> users; // 用户信息
extern list<BUFFER*> tasks;
extern pthread_mutex_t mutex;    // 避免全局变量访问冲突
extern sem_t sem;                 // 线程2和线程3通信的信号量

void common_init();

#endif
