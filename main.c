#include <stdio.h>/*生产者消费者问题，设定初始时候buffer里面有5个产品，buffer容量为10*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>//锁的头文件
#define INT_NUM  5   //buffer里面最开始有5个products
#define  TATALL_NUM   10   //buffer容量为10

sem_t mutex, full, empty;      //三个量分别为01锁的参数、buffer里面有多少个产品、buffer里面还空多少个产品
int num = INT_NUM ;             //num记录buffer里面的产品个数

void *productor(void){
              while(1){
			sleep(1);
                  sem_wait(&empty);
                  sem_wait(&mutex);
                                  //生产速度
                  num++;
                  printf("生产一个产品目前缓冲区里面有产品数:%d\n", num);
                  sem_post(&mutex);
                  sem_post(&full);
              }


}

void*consumer(void){
         while(1){
		sleep(5);
               sem_wait(&full);
               sem_wait(&mutex);
               
               num--;
               printf("消费一个产品目前缓冲区里面还有产品数:%d\n",  num);
               sem_post(&mutex);
               sem_post(&empty);
         }


}

void main(){
           pthread_t c_thread, p_thread;
           sem_init(&empty, 0, TATALL_NUM-INT_NUM);
           sem_init(&full, 0, INT_NUM);
           sem_init(&mutex,0, 1);
           pthread_create(&p_thread,NULL,productor,NULL);
           pthread_create(&c_thread,NULL,consumer,NULL);
           sleep(20);
	   exit(0);
	   pthread_join(p_thread,NULL);
           pthread_join(c_thread,NULL);
           return 0;

}



