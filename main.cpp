#include "common.h"

/* 分别在thread2.cpp, thread3.cpp中定义 */
extern void* thread2(void*);
extern void* thread3(void*);

int main()
{
    // 初始化全局的信号量和锁
    common_init();

    pthread_t tid2;
    pthread_create(&tid2, NULL, thread2, NULL);

    pthread_t tid3;
    pthread_create(&tid3, NULL, thread3, NULL);

    // 创建服务器的socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // 绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9913);
    addr.sin_addr.s_addr = 0;

    int ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        printf("bind error\n");
        exit(0);        
    }

    listen(sock, 10);
    while(1)
    {
        int newsock = accept(sock, NULL, NULL);
        if(newsock < 0)
        {
            // 当返回值指示错误时，可以使用errno获取失败的原因
            if(errno == EINTR)
                continue;

            printf("accept error\n");
            exit(0);
        }

        // accept成功
        pthread_mutex_lock(&mutex);
        clients.push_back(newsock);
        pthread_mutex_unlock(&mutex);
    }

    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    return 0;
}





