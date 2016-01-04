#include "common.h"

list<int> clients;
map<int, BUFFER*> buffers;
map<string, USER*> users;
list<BUFFER*> tasks;
pthread_mutex_t mutex;
sem_t sem;

void common_init()
{
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem, 0, 0);
}
