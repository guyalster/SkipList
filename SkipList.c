/*-----------------------------------------------------------------------*
Copyright(C) <2017>  <Guy Alster>
This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*---------------------------------------------------------------------------*/

#include "SkipList.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SKIP_ALLOC(i,TYPE)\
    (TYPE*)calloc(i, sizeof(TYPE))

#define SKIP_FREE(PTR) free(PTR)


typedef struct _SkipNodeSentinal
{
    SkipList Interface;
    SkipNode MainNode;
    SkipCompare SkipCompFunc;
    ValCompare  ValCompFunc;
    size_t MaxSkipLevels;
    size_t CurrLevels;
} SkipNodeSentinal;



static
size_t
GetMaxLevel(
    size_t MaxLevels,
    size_t *CurrLevels)
{
    int Res = (rand() % 2);
    size_t Level = Res;

    while (Res && Level < MaxLevels)
    {
        Res = (rand() % 2);
        Level += Res;
    }

    *CurrLevels = max(Level, *CurrLevels);
    return (Level);
}

static
ESkipResult 
InsertNewNode(
    SkipList *SL,
    SkipNode *pNode)
{
    SkipNodeSentinal *Head = (SkipNodeSentinal*)(SL);
    SkipNode *Curr = NULL;
    SkipNode *Prev = NULL;
    SkipCompare CompFunc = NULL;
    int Level = 0;
    int LevelToStart = 0;
    int Result = 0;

    if (pNode == NULL)
    {
        return (enUnititializedNode);
    }

    if (Head == NULL)
    {
        return enUnititializedLib;
    }

    //
    // The compFunc will be used to compare two skipnodes
    //

    CompFunc = Head->SkipCompFunc;

    //
    // Generate a level for this new node such that:
    // 0 <= Level < MaxSkipLevels
    // If the new Level is > than Head->CurrLevels,
    // we need to update Head->CurrLevels.
    //

    Level = pNode->Level =
        GetMaxLevel(
        Head->MaxSkipLevels,
        &(Head->CurrLevels));

    //
    // Allocate the memory for the new node's tower
    //
    pNode->ppNext =
        SKIP_ALLOC(Level + 1, SkipNode*);

    if (pNode->ppNext == NULL)
    {
        return (enUnititializedNode);
    }
    
    Curr = &(Head->MainNode);

    for (
        LevelToStart = Head->CurrLevels;
        LevelToStart >= 0;
        LevelToStart--)
    {
        //
        // For each level we first traverse all the way to the right
        // using the next pointer. Until we either hit NULL or a Node
        // that has a value greater than our Node.
        // We use the CompFunc to determine this
        // 

        while (Curr->ppNext[LevelToStart] != NULL)
        {
            Prev = Curr;
            Curr = Curr->ppNext[LevelToStart];

            Result = CompFunc(Curr, pNode);

            if (Curr == NULL || Result > 0)
            {
                Curr = Prev;
                break;
            }
        }

        //
        // We add the new Node only if the Current Level we are at
        // is less than or equal to the Top level of the new node
        //

        if (LevelToStart <= Level)
        {
            pNode->ppNext[LevelToStart] = Curr->ppNext[LevelToStart];
            Curr->ppNext[LevelToStart] = pNode;
        }
    }

    return (enOk); 
}

static
ESkipResult 
GetSkipNode(
    SkipList *SL,
    void *Key, 
    SkipNode **ppOutNode)
{
    SkipNodeSentinal *Head = (SkipNodeSentinal *)SL;
    SkipNode *Curr = NULL;
    SkipNode *Prev = NULL;
    ValCompare ValCompFunc = NULL;
    int LevelToStart = 0;
    int Result = 0;

    if (Head == NULL)
    {
        return (enUnititializedLib);
    }

    if (ppOutNode == NULL)
    {
        return (enUnititializedNode);
    }

    ValCompFunc = Head->ValCompFunc;
    Curr = &(Head->MainNode);

    for (
        LevelToStart = Head->CurrLevels; 
        LevelToStart >= 0; 
        LevelToStart--)
    {
        while (Curr->ppNext[LevelToStart] != NULL)
        {
            Prev = Curr;
            Curr = Curr->ppNext[LevelToStart];

            //
            // We use the ValCompFunc to determine
            // if the current Key is greater/less than or 
            // equal to the current Node we are currently traversing
            //

            Result = ValCompFunc(Key, Curr);

            if (Curr == NULL || Result < 0)
            {
                Curr = Prev;
                break;
            }

            if (Result == 0)
            {
                //
                // We found our Node
                //

                *ppOutNode = Curr;
                return (enOk);
            }
        }
    }

    *ppOutNode = NULL;
    return (enNotExists);
}

static
ESkipResult 
DeleteSkipNode(
    SkipList *SL, 
    void *Key, 
    bool RemoveAll)
{
    SkipNodeSentinal *Head = (SkipNodeSentinal *)SL;
    SkipNode *Curr = NULL;
    SkipNode *Prev = NULL;
    ValCompare ValCompFunc = NULL;
    int LevelToStart = 0;
    int Result = 0;

    if (Head == NULL)
    {
        return (enUnititializedLib);
    }

    ValCompFunc = Head->ValCompFunc;
    bool HasMoreToRemove = false;
    bool Removed = false;

    //
    // The do While clause is intended
    // for the case in which RemoveAll
    // is set to true. 
    //

    do
    {
        HasMoreToRemove = false;
        Curr = &(Head->MainNode);

        for (
            LevelToStart = Head->CurrLevels;
            LevelToStart >= 0;
            LevelToStart--)
        {

            while (Curr->ppNext[LevelToStart] != NULL)
            {
                Prev = Curr;
                Curr = Curr->ppNext[LevelToStart];

                Result = ValCompFunc(Key, Curr);

                if (Result == 0)
                {
                    Prev->ppNext[LevelToStart] = Curr->ppNext[LevelToStart];

                    if (LevelToStart == 0)
                    {
                        Removed = true;
                        HasMoreToRemove = true;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }

                if (Result < 0)
                {
                    Curr = Prev;
                    break;
                }
            }
        }

    } while (RemoveAll && HasMoreToRemove);

    if (Removed)
    {
        //
        // We were able to locate the node/nodes and remove
        // at least one.
        //

        return (enOk);
    }

    return (enNotExists);
}

void 
DestroySkipList(
    SkipList *SL)
{
    SkipNodeSentinal *Head = (SkipNodeSentinal *)SL;
    SkipNode *Curr;

    if (Head == NULL)
    {
        return;
    }

    //
    // Notice that we don't deallocate the Node's themselves
    // other than the Sentinal. We only deallocate their corresponding 
    // Towers. The deallocation of the nodes is the responsibility 
    // of the user.
    //

    for (Curr = Head->MainNode.ppNext[0]; Curr != NULL;)
    {
        SkipNode *Next = Curr->ppNext[0];

        SKIP_FREE(Curr->ppNext);
        Curr = Next;
    }

    //
    // Deallocate the Sentinal node and its Tower
    //

    SKIP_FREE(Head->MainNode.ppNext);
    SKIP_FREE(Head);
}

SkipList*
InitializeSkipList(
    size_t MaxSkipLevels,
    SkipCompare SkipComp,
    ValCompare ValComp)
{
    static bool SkipListLibInit = false;
    SkipNodeSentinal *Sentinal = NULL;
    int LevelIdx = 0;

    //
    // This will only be called once
    //

    if (SkipListLibInit == false)
    {
        SkipListLibInit = true;
        srand(((unsigned int)(time(NULL))));
    }

    Sentinal =
        SKIP_ALLOC(1, SkipNodeSentinal);

    if (Sentinal == NULL)
    {
        return (NULL);
    }

    //
    // Make the sentinal point to the 
    // skilist functions
    //

    Sentinal->SkipCompFunc = SkipComp;
    Sentinal->ValCompFunc = ValComp;
    Sentinal->MaxSkipLevels = MaxSkipLevels;
    Sentinal->CurrLevels = 0;

    //
    // Allocate MaxSkipLevels for the sentinal's main node
    //

    Sentinal->MainNode.ppNext =
        SKIP_ALLOC(MaxSkipLevels + 1, SkipNode*);

    if (Sentinal->MainNode.ppNext == NULL)
    {
        SKIP_FREE(Sentinal);
        Sentinal = NULL;
		return NULL;
    }

    Sentinal->MainNode.Level = MaxSkipLevels;
    Sentinal->Interface.InsertNewNode = InsertNewNode;
    Sentinal->Interface.DeleteSkipNode = DeleteSkipNode;
    Sentinal->Interface.GetSkipNode = GetSkipNode;

    return (SkipList*)(Sentinal);
}