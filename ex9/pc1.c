#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 4
int buffer[2][CAPACITY];
int in[2];
int out[2];

int buffer_is_empty(int i)
{
    return in[i] == out[i];
}

int buffer_is_full(int i)
{
    return (in[i] + 1) % CAPACITY == out[i];
}

int get_item(int i)
{
    int item;

    item = buffer[i][out[i]];
    out[i] = (out[i] + 1) % CAPACITY;
    return item;
}

void put_item(int item, int i)
{
    buffer[i][in[i]] = item;
    in[i] = (in[i] + 1) % CAPACITY;
}

pthread_mutex_t m1;
pthread_mutex_t m2;
pthread_cond_t e1;
pthread_cond_t f1;
pthread_cond_t e2;
pthread_cond_t f2;

#define ITEM_COUNT (CAPACITY * 2)

void *consume(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        pthread_mutex_lock(&m2);
        while (buffer_is_empty(1))
            pthread_cond_wait(&f2, &m2);

        item = get_item(1);
        printf("3:    consume item: %c\n", item);

        pthread_cond_signal(&e2);
        pthread_mutex_unlock(&m2);
    }
    return NULL;
}
void *compute(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        //get item from buffer1
        pthread_mutex_lock(&m1);
        while (buffer_is_empty(0))
            pthread_cond_wait(&f1, &m1);
        item = get_item(0);
        // printf("2:    consume item: %c\n", item);
        pthread_cond_signal(&e1);
        pthread_mutex_unlock(&m1);
        //convert item to upper case then put to buffer2
        pthread_mutex_lock(&m2);
        while (buffer_is_full(1))
            pthread_cond_wait(&e2, &m2);
        item -= 32;
        put_item(item, 1);
        // printf("2:        produce item: %c\n", item);
        pthread_cond_signal(&f2);
        pthread_mutex_unlock(&m2);
    }
    return NULL;
}
void *produce(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        pthread_mutex_lock(&m1);
        while (buffer_is_full(0))
            pthread_cond_wait(&e1, &m1);
        item = 'a' + i;
        put_item(item, 0);
        // printf("1:produce item: %c\n", item);
        pthread_cond_signal(&f1);
        pthread_mutex_unlock(&m1);
    }
    return NULL;
}

int main()
{
    pthread_t consumer_tid;
    pthread_t computer_tid;
    pthread_mutex_init(&m1, NULL);
    pthread_cond_init(&e1, NULL);
    pthread_cond_init(&f1, NULL);
    pthread_mutex_init(&m2, NULL);
    pthread_cond_init(&e2, NULL);
    pthread_cond_init(&f2, NULL);
    pthread_create(&computer_tid, NULL, compute, NULL);
    pthread_create(&consumer_tid, NULL, consume, NULL);
    produce(NULL);
    pthread_join(computer_tid, NULL);
    pthread_join(consumer_tid, NULL);
    return 0;
}