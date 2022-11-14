/* 使用信号量实现生产者和消费者模型，生产者有 5 个，往链表头部添加节点，消费者也有 5 个，删除链表头部的节点。*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h> //信号量的头文件
 
//生产者线程信号量
sem_t psem;
 
//消费者线程信号量
sem_t csem;
 
pthread_mutex_t mutex;  //互斥锁变量
 
//链表的节点
struct node{
    int date;
    struct node* next;
};
 
//指向头结点的指针
struct node* head = NULL;
 
void* producer(void* arg);
void* consumer(void* arg);
 
int main()
{
 
     // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);
 
    // 初始化信号量
    // 生产者和消费者拥有的资源的总和为5
    sem_init(&psem, 0, 5);  // 生成者线程一共有5个资源
    sem_init(&csem, 0, 0);  // 消费者线程一共有0个资源
 
    pthread_t t1[5], t2[5];
    for(int i = 0; i < 5; i++)
    {
        pthread_create(&t1[i], NULL, producer, NULL);
        pthread_create(&t2[i], NULL, consumer, NULL);
    }
 
    for(int i = 0; i < 5; i++)
    {
        pthread_join(t1[i], NULL);
        pthread_join(t2[i], NULL);
    }
 
    pthread_mutex_destroy(&mutex);
    sem_destroy(&psem);
    sem_destroy(&csem);
 
    return 0;
}
 
void* producer(void* arg)
{
    while(1)
    {
        // 生产者资源减一
        sem_wait(&psem);
 
        // 加锁, 这句代码放到 sem_wait()上边, 有可能会造成死锁
        pthread_mutex_lock(&mutex);
 
        //创建一个链表的新节点
        struct node* newNode = (struct node*)malloc(sizeof(struct node));
        // 节点初始化
        newNode->date = rand()%100 + 1;
        newNode->next = head;
        head = newNode;
        printf("生产者， ID: %ld, date: %d\n", pthread_self(), newNode->date);
 
        pthread_mutex_unlock(&mutex);
 
        // 通知消费者消费, 给消费者的资源加一
        sem_post(&csem);
        sleep(rand()%3);
    }
    return NULL;
}
 
void* consumer(void* arg)
{
    while(1)
    {
        //消费者的资源减一
        sem_wait(&csem);
 
        // 加锁, 这句代码放到 sem_wait()上边, 有可能会造成死锁
        pthread_mutex_lock(&mutex);
 
        struct node* head_node = head;
        printf("消费者, ID: %ld, date: %d\n", pthread_self(), head_node->date);
        head = head->next;
        free(head_node);
 
        pthread_mutex_unlock(&mutex);
 
        // 通知生产者生成, 给生产者的资源加一
        sem_post(&psem);
        sleep(rand()%3);
    }
    return NULL;
}