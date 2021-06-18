#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 4
int buffer[2][CAPACITY];
int in[2];
int out[2];

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

typedef struct
{
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sema_t;

void sema_init(sema_t *sema, int value)
{
    sema->value = value;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    while (sema->value <= 0)
        pthread_cond_wait(&sema->cond, &sema->mutex);
    sema->value--;
    pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    ++sema->value;
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}

sema_t m1;
sema_t m2;
sema_t e1;
sema_t f1;
sema_t e2;
sema_t f2;

#define ITEM_COUNT (CAPACITY * 2)

void *consume(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        sema_wait(&f2);
        sema_wait(&m2);

        item = get_item(1);
        printf("3:    consume item: %c\n", item);

        sema_signal(&e2);
        sema_signal(&m2);
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
        sema_wait(&f1);
        sema_wait(&m1);
        item = get_item(0);
        // printf("2:    consume item: %c\n", item);
        sema_signal(&e1);
        sema_signal(&m1);
        //convert item to upper case then put to buffer2
        sema_wait(&e2);
        sema_wait(&m2);
        item -= 32;
        put_item(item, 1);
        // printf("2:        produce item: %c\n", item);
        sema_signal(&f2);
        sema_signal(&m2);
    }
    return NULL;
}
void *produce(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        sema_wait(&e1);
        sema_wait(&m1);
        item = 'a' + i;
        put_item(item, 0);
        // printf("1:produce item: %c\n", item);
        sema_signal(&f1);
        sema_signal(&m1);
    }
    return NULL;
}

int main()
{
    pthread_t consumer_tid;
    pthread_t computer_tid;
    sema_init(&m1, 1);
    sema_init(&m2, 1);
    sema_init(&f1, 0);
    sema_init(&f2, 0);
    sema_init(&e1, CAPACITY);
    sema_init(&e2, CAPACITY);
    pthread_create(&computer_tid, NULL, compute, NULL);
    pthread_create(&consumer_tid, NULL, consume, NULL);
    produce(NULL);
    pthread_join(computer_tid, NULL);
    pthread_join(consumer_tid, NULL);
    return 0;
}