
#include "common.h"

void response(USER* user, const char* resp, short len)
{
    
    int sendSize = len;

                len = htons(len);
                send(user->fd, (char*)&len, 2, 0);
                send(user->fd, resp, sendSize, 0);
}

void* thread3(void*)
{
    while(1)
    {
        sem_wait(&sem);

        pthread_mutex_lock(&mutex);
        BUFFER* buf = *tasks.begin();
        tasks.pop_front();
        pthread_mutex_unlock(&mutex);

        string cmd = buf->buf;

        // cmd = "name: xgl"
        if(strncmp(cmd.c_str(), "name", 4) == 0)
        {
            string name = cmd.substr(6);

            USER* user = users[name];
            if(user == NULL)
            {
                user = new USER;
              //  user = (USER*)malloc(sizeof(USER));
                users[name] = user;
                user->fd = buf->fd;
                user->username = name;
                printf("user: %s is comming\n", name.c_str());

                char resp[] = "OK";
                response(user, resp, sizeof(resp));
            }
            else
            {
                printf("username %s already exist\n", name.c_str());
                char resp[] = "ERROR: username already used";
                response(user, resp, sizeof(resp));
            }
        }

        delete buf;
     //   free(buf);
    }
    return NULL;
}
