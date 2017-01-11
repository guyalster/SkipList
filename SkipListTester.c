#include "SkipList.h"
#include <stdlib.h>


typedef struct _TestStruct
{
    int K;
    int V;
    SkipNode Node;
    
} TestStruct;

static int NodeCompare(SkipNode *N1, SkipNode *N2)
{
    TestStruct *M1 = NULL, *M2 = NULL;
    GET_SKIP_CONTAINER(TestStruct, M1, N1, Node);
    GET_SKIP_CONTAINER(TestStruct, M2, N2, Node);
    return (M1->K - M2->K);
}

static int ValueCompare(void *V, SkipNode *N1)
{
    TestStruct *M1;
    int *K = (int*)V;
    GET_SKIP_CONTAINER(TestStruct, M1, N1, Node);

    return (*K - M1->K);
}

TestStruct Arr[1024*1024];

int main()
{
    
    int Levels = 20;

    SkipList *SL = InitializeSkipList(Levels, NodeCompare, ValueCompare);
    ESkipResult Res1, Res2;
    SkipNode *N;
    
    for (int i = 0; i < (1024 * 1024); i++)
    {
        Arr[i].K = i;
        Arr[i].V = (rand() % (1024 * 1024));
        SL->InsertNewNode(SL,&(Arr[i].Node));
    }

    int Key = 593023;
    Res1 = SL->GetSkipNode(SL, &Key, &N);

    if (Res1== enOk)
    {
        printf("Received OK\n");
        TestStruct *M1;
        GET_SKIP_CONTAINER(TestStruct, M1, N, Node);

        printf("%d %d\n", M1->K, M1->V);
    }

    Res2 = SL->DeleteSkipNode(SL, &Key, true);

    if (Res2 != enOk)
    {
        printf("Couldnt delete Node\n");
    }
    else
    {
        Res1 = SL->GetSkipNode(SL, &Key, &N);

        if (Res1 == enOk)
        {
            TestStruct *M1;
            GET_SKIP_CONTAINER(TestStruct, M1, N, Node);

            printf("%d %d\n", M1->K, M1->V);
        }
        else
        {
            printf("Could not find node 593023 %d\n", Res1);
        }
    }

    DestroySkipList(SL);

    return (0);
}