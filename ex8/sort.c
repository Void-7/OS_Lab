#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 15
#define MID (N - 2) / 2
int a[N] = {2, 99, 57, 32, 69, 75, 70, 11, 89, 50, 39, 66, 90, 85, 34};

struct param
{
    int *array;
    int start;
    int end;
};

void Merge(int arr[], int l, int m, int r)
{
    //declare ptrs for both halves
    //and ptr k for new space: b[]
    int i = l;
    int j = m + 1;
    int k = 0;
    //allocate mem to save sorted items
    int *b = (int *)malloc((r - l + 1) * sizeof(int));

    //copy MIN(arr[i],arr[j]) to b[]
    //this loop stops when i or j overflows
    while (i <= m && j <= r)
        if (arr[i] <= arr[j])
            b[k++] = arr[i++];
        else
            b[k++] = arr[j++];

    //copy rest items to b[]
    //the first overflow ptr(i or j)
    //decides which half to choose
    if (i > m)
        while (j <= r)
            b[k++] = arr[j++];
    else
        while (i <= m)
            b[k++] = arr[i++];

    //copy from b[] to arr[]
    i = l;
    int t;
    for (t = 0; t < k; t++)
        arr[i++] = b[t];
    free(b);
}

void SelectionSort(int arr[], int l, int r)
{

    //outer loop circulates for N-1 times,
    //N for the len of arr: r-l+1
    int i, j;
    for (i = l; i < r; i++)
    {
        int min = i;
        for (j = i + 1; j <= r; j++)
            if (arr[j] < arr[min])
                min = j;
        int temp = arr[i];
        arr[i] = arr[min];
        arr[min] = temp;
    }
}

void *compute(void *arg)
{
    struct param *param;

    param = (struct param *)arg;
    int l = param->start, r = param->end;
    SelectionSort(param->array, l, r);
}

int main()
{
    pthread_t p1, p2;int i;
    printf("Before:\n", a[i]);
    for (i = 0; i < N; i++)
        printf("%d ", a[i]);
    struct param *param1, *param2;
    param1=malloc(sizeof(struct param));
    param2=malloc(sizeof(struct param));
    param1->array = a;
    param1->start = 0;
    param1->end = MID;
    pthread_create(&p1, NULL, compute, param1);
    param2->array = a;
    param2->start = MID + 1;
    param2->end = N - 1;
    pthread_create(&p2, NULL, compute, param2); 
    pthread_join(p1,NULL);
    pthread_join(p2, NULL);
    Merge(a,0,MID,N-1);

    printf("\nAfter:\n", a[i]);
    for (i = 0; i < N; i++)
        printf("%d ", a[i]);
    printf("\n");
    return 0;
}