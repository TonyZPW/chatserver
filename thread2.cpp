#include "common.h"


/*
 *  "Hello"
 *
 *  short len = strlen(Hello) + 1;
 *  len = htons(len);
 *
 *  send(fd, &len, 2, 0);
 *  send(fd, "Hello", 6, 0);
 * */

void* thread2(void*)
{

    list<int> closesockets;

    while(1)
    {
        int n;
        fd_set set;
        struct timeval tv;

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        list<int>::iterator it;
        n = 0;
        pthread_mutex_lock(&mutex);
        FD_ZERO(&set);
        for(it = clients.begin(); it!=clients.end(); )
        {
            if(std::find(closesockets.begin(), closesockets.end(), *it) != closesockets.end())
            {
                close(*it);
                closesockets.remove(*it);
                it = clients.erase(it);
                continue;
            }

            FD_SET(*it, &set);
            if(*it > n) n = *it;
            ++it;
        }
        pthread_mutex_unlock(&mutex);

        int ret = select(n+1, &set, NULL, NULL, &tv);
        if(ret > 0)
        {
            pthread_mutex_lock(&mutex);
            printf("clients count=%d\n", (int)clients.size());
            for(it = clients.begin(); it!=clients.end(); ++it)
            {
                int fd = *it;
                if(FD_ISSET(fd, &set))
                {
                    BUFFER* recvBuf = buffers[fd];
                    if(recvBuf == NULL)
                    {
                    ///    recvBuf = (BUFFER*)malloc(sizeof(BUFFER));
                        recvBuf = new BUFFER;

                        recvBuf->total_size = -1;
                        recvBuf->recv_size = 0;
                        recvBuf->fd = fd;

                        buffers[fd] = recvBuf;
                    }

                    if(recvBuf->recv_size < 2)
                    {
                        printf("recv cmd head\n");
                        char* p = (char*)&recvBuf->total_size;
                        int ret = recv(fd, p, 2-recvBuf->recv_size, 0);
                        if(ret > 0)
                        {
                            recvBuf->recv_size += ret;
                            if(recvBuf->recv_size == 2)
                                recvBuf->total_size = ntohs(recvBuf->total_size);
                        }
                        else // ret <= 0 表示这个客户端已经退出了
                        {
                            closesockets.push_back(fd);
                        }
                    }
                    else
                    {
                        printf("recv cmd data\n");
                        char buf[1024];
                        int ret = recv(fd, buf, recvBuf->total_size + 2 - recvBuf->recv_size, 0);
                        if(ret > 0)
                        {
                            recvBuf->recv_size += ret;
                            recvBuf->buf += buf;
                        //    recvBuf->buf = recvBuf->buf + string(buf);
                        //    recvBuf->buf.append(buf);
                            
                        }
                        else // ret <= 0表明这个用户出异常
                        {
                            closesockets.push_back(fd);
                        }
                    }

                    printf("now fd=%d, buff is %s, total_size=%d, recv_size=%d\n", 
                            recvBuf->fd, recvBuf->buf.c_str(), 
                            (int)recvBuf->total_size, (int)recvBuf->recv_size);

                    if(recvBuf->total_size == recvBuf->recv_size - 2)
                    {
                        // 把buffer发送给线程3
                        tasks.push_back(recvBuf);
                        buffers[fd] = NULL;
                        sem_post(&sem);
                    }
#if 0
                    char buf[1024];
                    ret = recv(fd, buf, sizeof(buf), 0);
                    if(ret > 0)
                    {
                        // 收到的数据要保存在BUFFER里
                        BUFFER* recvBuf = buffers[fd];
                        if(recvBuf == NULL)
                        {
                            recvBuf = malloc(sizeof(BUFFER));
                            recvBuf->fd = fd;
                            recvBuf->string = buf;
                            recvBuf->total_size = 0;// TODO: this is problem
                            buffers[fd] = recvBuf;
                        }
                        else
                        {
                            recvBuf->buf += buf;
                        }
                        // 判断是否已经接收完毕
                        if(recvBuf->buf.size() == recvBuf->total_size)
                        {
                            // 将buf发送给线程3
                            tasks.push_back(recvBuf);
                            buffers[fd] = NULL;
                            sem_post(&sem);
                        }
                    }

#endif
                }
            }
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}


