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

#ifndef _SKIP_LIST_H_
#define _SKIP_LIST_H_

#include <stdbool.h>
#include <stdio.h>

/*==================================================================
 forward declare _SkipNode
====================================================================*/

struct _SkipNode;

/*=================================================================
    Given a SkipNode extract a pointer to the enclosing structure
===================================================================*/

#define GET_SKIP_CONTAINER(T,CONT_PTR,SKIP_PTR,SKIP_VAR_NAME) \
    CONT_PTR = ((T*)(((char*)SKIP_PTR) - ((size_t)((char*)(&(((T*)(0))->SKIP_VAR_NAME))))))
    

/*=================================================================
    Declaration of a SkipNode
===================================================================*/

typedef struct _SkipNode
{	
    //SKIP_HANDLE Handle;
    int Level;
    struct _SkipNode **ppNext;
} SkipNode;


/*=================================================================
Comparator to compare two skipnodes. This is defined  by the user 
of this library and passed to the library upon initialization. 
Return Value is:
>0 if the first argument is greater than the second
== 0 if they are equal
< 0 if the first argument is less than the second
===================================================================*/

typedef int(*SkipCompare)(SkipNode *, SkipNode *);

/*=================================================================
Comparator to compare a Key to a skipnode. This is defined  by the user
of this library and passed to the library upon initialization.
Return Value is:
>0 if the first argument is greater than the second
== 0 if they are equal
< 0 if the first argument is less than the second
===================================================================*/

typedef int(*ValCompare)(void *, SkipNode *);


/*=================================================================
This enum defines the error codes returned by the library's 
APIs.
===================================================================*/

typedef 
enum 
_ESkipResult
{ 
    enOk, 
    enUnititializedLib, 
    enUnititializedNode,
    enAlreadyExists, 
    enNotExists 
} 
ESkipResult;


/*==================================================================
    The SkipList Interface
===================================================================*/

typedef struct _SkipList
{
    ESkipResult(*InsertNewNode)(struct _SkipList *,SkipNode *);
    ESkipResult(*GetSkipNode)(struct _SkipList *, void *, SkipNode **);
    ESkipResult(*DeleteSkipNode)(struct _SkipList *, void *, bool);
}SkipList;

/*==================================================================
 Initialization APIs for the SkipList Lib 
===================================================================*/
SkipList *InitializeSkipList(size_t, SkipCompare, ValCompare);
void DestroySkipList(SkipList *);


#endif