//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FILE* logFile                    =  stdout;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int listCtor (list_t* list, size_t capacity)
{
    assert(list != nullptr);

    if (capacity == 0)
        return -1; //TODO enum error
    
    capacity = (capacity / ListInitValue + 1) * ListInitValue; //TODO why
    list->data = (node_t*) calloc (capacity + 1, sizeof (node_t));

    if (list->data == nullptr)
        return -1; //TODO enum error
    
    list->head      = 1;
    list->tale      = 1;
    list->freeHead  = 1;
    list->size      = 0;
    list->capacity  = capacity;

    for (size_t index = 1; index < list->capacity; index++)
    {
        list->data[index].prev = FreeValue;
        list->data[index].next = index + 1;
    }

    list->data[list->capacity].prev = FreeValue;
    list->data[list->capacity].next = 0;

    return 1; //TODO enum
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int listDtor (list_t* list)
{
    assert(list != nullptr);

    if (isListDestructed (list) == 1)
        return -1; //TODO enum error
    
    free (list->data);

    list->head      = Poison;
    list->tale      = Poison;
    list->freeHead  = Poison;
    list->size      = Poison;
    list->capacity  = Poison;

    return 1; //TODO enum
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int isListDestructed (list_t* list)
{
    assert(list != nullptr);
    
    if (list-> head == Poison && list->tale == Poison && list->freeHead == Poison && list->size == Poison && 
        list->capacity == Poison)
        return 1; //TODO enum
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int isListEmpty (list_t* list)
{
    assert(list != nullptr);

    if (list->head == 1 && list->tale == 1 && list->freeHead == 1 &&
        list->size == 0)
        return 1; //TODO

    return 0; //TODO
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef GRAPHVIZ_DUMP

    void listDump ()
    {
        
    }

#else

    void listDumpFunc (list_t* list, size_t line, const char file[ParamMaxSize], const char func[ParamMaxSize])
    {
        fprintf (logFile, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~START LIST DUMP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

        // TODO if (list->status & LIST_NULL_PTR)
        // {
        //     fprintf(logFile, "Lists's pointer is null\n");
        // }

        fprintf (logFile, "Called at %s at %s(%lu)\n", file, func, line);
        fprintf (logFile, "List status: OK!\n"); //TODO


        fprintf (logFile, "Head: %lu\nTale: %lu\nFreeHead: %lu\nSize: %lu\nCapacity: %lu\n", 
            list->head, list->tale, list->freeHead, list->size, list->capacity);

        fprintf (logFile, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");


        fprintf (logFile, "Num.    Previous    Value   Next\n");

        for (size_t index = 0; index <= list->capacity; ++index)
        {
            fprintf (logFile, "%4lu    %8lu    %5d   %4lu\n", index, list->data[index].prev, list->data[index].value, list->data[index].next);
        }

        fprintf (logFile, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~END LIST DUMP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }

#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listHead (list_t* list)
{
    return list->head;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listTale (list_t* list)
{
    return list->tale;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listNextElem (list_t* list, size_t physIndex)
{
    return list->data[physIndex].next;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPrevElem (list_t* list, size_t physIndex)
{
    return list->data[physIndex].prev;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

node_t* listRecalloc (list_t* list, const size_t newCapacity)
{
    assert (list != nullptr);
    
    size_t capacity = newCapacity * sizeof (node_t);

    node_t* const data = (node_t*) realloc (list->data, capacity);
    
    assert (data != nullptr);

    for (size_t index = list->capacity; index < newCapacity; index++)
    {
        data[index].next    = index + 1;
        data[index].prev    = FreeValue;
        data[index].value   = FreeValue;
    }

    data[newCapacity - 1].next = list->freeHead;

    list->freeHead = list->capacity;

    list->capacity = newCapacity;
    list->data     = data;

    return data;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPushBegin (list_t* list, size_t physIndex, elem_t value)
{
    size_t elemIndex = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* 
DONE:
конструкции
деструкции
дампа
получения указателя на первый элемент списка
получения указателя на последний элемент списка
получения указателя на элемент, следующий за данным
получения указателя на элемент, предшествующий данному


TODO:
тихой и медленной верификации
вставки элемента в начало списка
вставки элемента в конец списка
вставки элемента перед заданным элементом
вставки элемента после заданного элемента
удаления заданного элемента
поиска элемента по его значению
поиска элемента по его номеру в последовательности (операция индексации)
удаления всех элементов
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
