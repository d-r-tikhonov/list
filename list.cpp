//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "list.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FILE* logFile = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int openLog (void)
{
     #ifdef GRAPHVIZ_DUMP
        if ((logFile = fopen ("log.html", "w")) == nullptr)
        {
            printf ("Error in function: %s. Error opening logFile!\n", __func__);
            return errno;
        }
    #else
        if ((logFile = fopen ("log.txt", "w")) == nullptr)
        {
            printf ("Error in function: %s. Error opening logFile!\n", __func__);
            return errno;
        }
    #endif

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int closeLog (void)
{
    if ((fclose (logFile)) != 0)
    {
        printf ("Error in function: %s. Error closing logFile!\n", __func__);
        return errno;
    }

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int listCtor (list_t* list, size_t capacity)
{
    ASSERT (list != nullptr);

    if (capacity == 0)
    {
        printf ("Error in function: %s. Error capacity = 0!\n", __func__);
        return -1;
    }    

    list->data = (node_t*) calloc (capacity + 1, sizeof (node_t));

    ASSERT (list->data != nullptr);
    
    list->head      = 1;
    list->tail      = 1;
    list->freeHead  = 1;
    list->size      = 0;
    list->capacity  = capacity;

    for (size_t index = 1; index < list->capacity; index++)
    {
        list->data[index].prev = FreeValuePrev;
        list->data[index].next = index + 1;
    }

    list->data[0].value = ValueNullData;

    list->data[list->capacity].prev = FreeValuePrev;
    list->data[list->capacity].next = 0;

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int listDtor (list_t* list)
{
    ASSERT (list != nullptr);

    if (isListDestructed (list) == 1)
    {
        printf ("Error in function: %s. Error list already destroyed!\n", __func__);
        return errno;
    }
    
    free (list->data);

    list->head      = Poison;
    list->tail      = Poison;
    list->freeHead  = Poison;
    list->size      = Poison;
    list->capacity  = Poison;

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int isListDestructed (list_t* list)
{
    ASSERT (list != nullptr);
    
    if (list-> head == Poison && list->tail == Poison && list->freeHead == Poison && list->size == Poison && list->capacity == Poison)
        return 1;

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int isListEmpty (list_t* list)
{
    ASSERT (list != nullptr);

    if (list->head == 1 && list->tail == 1 && list->freeHead == 1 && list->size == 0)
        return 1;

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void listGraphDump (const list_t* list)
{
    ASSERT (list != nullptr);
    static unsigned dumpNum = 1;

    static char buf[512] = {0};
    snprintf (buf, sizeof(buf), "dump%u.dot", dumpNum);

    FILE* dotFile = fopen (buf, "w");

    static const char Header[] = R"(
                                    digraph {
                                            dpi      = 57
                                            fontname = "Aqum"
                                            edge [color = darkgrey, arrowhead = onormal, arrowsize = 1.6, penwidth = 1.2]
                                            graph[fillcolor = lightgreen, ranksep = 2.6, nodesep = 1,
                                                    style = "rounded, filled", color = green, penwidth = 2]
                                            node [penwidth = 2, shape = box, color = grey, 
                                                    fillcolor = white, style = "rounded, filled", fontname = "Aqum"]
                                            compound  =  true;
                                            newrank   =  true;
                                            rankdir   =  LR; 
                                    )";

    fprintf (dotFile, Header);

    fprintf (dotFile, "0");
    for (size_t index = 0; index <= list->capacity; index++) 
    {
        fprintf (dotFile, "-> %d", index);
    }
    fprintf (dotFile, "[style=invis, weight = 1, minlen = \"1.5\"]");


    fprintf (dotFile, "free->%d:n[color=cadetblue]\n", list->freeHead);
    fprintf (dotFile, "tail->%d:p[color=cadetblue]\n", list->tail);
    fprintf (dotFile, "head->%d:n[color=cadetblue]\n", list->head);


    for (size_t index = 0; index <= list->capacity; index++) 
    {
        fprintf (dotFile, "subgraph cluster%d { \n"
                        "       label = %d;  \n"
                        "       fontsize = 14; \n", index, index);


        fprintf (dotFile, "%d [shape=record, label=\"<p>prev(HEX): %X | data(HEX): %X | <n>next(HEX): %X\"] \n} \n",
                index, list->data[index].prev, list->data[index].value, list->data[index].next);

        if (list->data[index].prev != FreeValuePrev)
                fprintf (dotFile, "%u:n -> %d:n[color=darkgoldenrod2, style=dashed]\n", index, list->data[index].next);
        else
                fprintf (dotFile, "%u:n -> %d:n[color=mediumpurple4 ]\n", index, list->data[index].next);

        if (list->data[index].prev != FreeValuePrev)
                fprintf (dotFile, "%u:p -> %d:p[color=darkslategray, style=dashed]\n", index, list->data[index].prev);
    }

    fprintf (dotFile, "\n}");

    fclose (dotFile);

    snprintf (buf, sizeof (buf), "dot -Tsvg dump%u.dot -o dump%u.svg", dumpNum, dumpNum);
    system (buf);

    fprintf (logFile, "<center""><h1"">LIST DUMP - INVOCATION %u</h1"">""</center"">\n", dumpNum);
    fprintf (logFile, "<img width=\"1920\" height=\"1080\" src=\"dump%u.svg\"/>\n", dumpNum);

    dumpNum++;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void listTextDump (list_t* list, size_t line, const char file[ParamMaxSize], const char func[ParamMaxSize])
{
    fprintf (logFile, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~START LIST DUMP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

    fprintf (logFile, "Called at %s at %s(%d)\n", file, func, line);
    fprintf (logFile, "List status: OK!\n"); 


    fprintf (logFile, "Head: %d\nTail: %d\nFreeHead: %d\nSize: %d\nCapacity: %d\n", 
        list->head, list->tail, list->freeHead, list->size, list->capacity);

    fprintf (logFile, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");


    fprintf (logFile, "Num.    Previous    Value   Next\n");

    for (size_t index = 0; index <= list->capacity; ++index)
    {
        fprintf (logFile, "%4d    %8d    %5d   %4d\n", index, list->data[index].prev, list->data[index].value, list->data[index].next);
    }

    fprintf (logFile, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~END LIST DUMP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listHead (list_t* list)
{
    return list->head;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listTail (list_t* list)
{
    return list->tail;
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

bool isListCorrect (list_t* list)
{
    ASSERT (list != nullptr);

    node_t* data = list->data;

    if (data[0].prev != 0 || data[0].next != 0)
    {
        printf ("Error in function: %s. Verification FAILED! Condition: data[0].prev != 0 OR data[0].next != 0\n", __func__);
        listDump (list);
        return 0;
    }

    for (size_t index = 1; index < list->capacity; index++)
    {
        if (data[index].prev == FreeValuePrev) 
        {
            if (data[index].next > list->capacity)
            {
                printf ("Error in function: %s. Verification FAILED! Condition: next > capacity.\n", __func__);
                listDump (list);
                return 0;
            }
            continue;
        }

        if (data[index].prev > list->capacity)
        {
            printf ("Error in function: %s. Verification FAILED! Condition: prev > capacity.\n", __func__);
            listDump (list);
            return 0;
        }

        if (data[index].next > list->capacity)
        {
            printf ("Error in function: %s. Verification FAILED! Condition: next > capacity.\n", __func__);
            listDump (list);
            return 0;
        }

        if (data[data[index].prev].next != index && index != list->head)
        {
            printf ("Error in function: %s. Verification FAILED!"
                    "Condition: data[data[index].prev].next != index && index != list->head.\n", __func__);
            listDump (list);
            return 0;
        }

        if (data[data[index].next].prev != index && index != list->tail)
        {
            printf ("Error in function: %s. Verification FAILED!"
                    "Condition: data[data[index].next].prev != index && index != list->tail.\n", __func__);
            listDump (list);
            return 0;
        }
    }
    
    fprintf (logFile, "<font color=\"green\">Verification is OK!</font>\n");

    return 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

node_t* listRecalloc (list_t* list, const size_t newCapacity)
{
    ASSERT (list != nullptr);
    ASSERT (newCapacity > list->capacity);

    listLinearize (list);
    size_t capacity = (newCapacity + 1) * sizeof (node_t);

    node_t* const data = (node_t*) realloc (list->data, capacity);
    ASSERT (data != nullptr);

    data[list->capacity].next = list->capacity + 1;

    for (size_t index = list->capacity + 1; index < newCapacity; index++)
    {
        data[index].next  = index + 1;
        data[index].value = FreeValue;
        data[index].prev  = FreeValuePrev;
    }

    data[newCapacity].next  = 0;
    data[newCapacity].value = FreeValue;
    data[newCapacity].prev  = FreeValuePrev;

    if (list->size == list->capacity)
    {
        list->freeHead = list->capacity + 1;
    }

    list->capacity = newCapacity;
    list->data     = data;

    return data;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPushHead (list_t* list, elem_t pushValue)
{
    ASSERT (list != nullptr);

    size_t currentIndex = 0;
    currentIndex = list->freeHead;

    list->freeHead = list->data[currentIndex].next;

    list->data[currentIndex].value = pushValue;

    if (list->data[list->head].next == currentIndex)
    {
        list->data[list->head].next = list->data[currentIndex].next;
    }

    if (list->size != 0)
    {
        list->data[currentIndex].next = list->head;
    }

    list->size = list->size + 1;

    if (list->capacity == list->size)
    {
        list->data[list->head].next = 0;
    }
    
    list->data[list->head].prev = currentIndex;
    list->data[currentIndex].prev = 0;

    list->head = currentIndex;

    ASSERT (isListCorrect (list));

    return currentIndex;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPushTail (list_t* list, elem_t pushValue)
{
    ASSERT (list != nullptr);

    size_t currentIndex = 0;
    currentIndex = list->freeHead;

    list->freeHead = list->data[currentIndex].next;

    list->data[currentIndex].value = pushValue;

    list->data[currentIndex].prev = list->tail;
    list->data[currentIndex].next = 0;

    list->data[list->tail].next = currentIndex;

    list->tail = currentIndex;
    list->size = list->size + 1;

    ASSERT (isListCorrect (list));

    return currentIndex;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPushBefore (list_t* list, size_t physIndex, elem_t pushValue)
{
    ASSERT (list != nullptr);
    ASSERT (physIndex <= list->capacity + 1);
    ASSERT (!(list->data[physIndex].prev == FreeValuePrev && (list->size != 0 || physIndex != list->tail)));

    size_t currentIndex = 0;
    currentIndex = list->freeHead;

    list->freeHead = list->data[currentIndex].next;
    
    size_t indexPrev = 0;
    indexPrev = list->data[physIndex].prev;

    list->data[currentIndex].value  = pushValue;
    list->data[currentIndex].prev   = indexPrev;
    list->data[currentIndex].next   = physIndex;

    list->data[indexPrev].next = currentIndex;
    list->data[physIndex].prev = currentIndex;

    list->size = list->size + 1;

    ASSERT (isListCorrect (list));

    return currentIndex;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPushAfter (list_t* list, size_t physIndex, elem_t pushValue)
{
    ASSERT (list != nullptr);
    ASSERT (physIndex <= list->capacity + 1);
    ASSERT (!(list->data[physIndex].prev == FreeValue && (list->size != 0 || physIndex != list->tail)));

    size_t currentIndex = 0;
    currentIndex = list->freeHead;

    list->freeHead = list->data[currentIndex].next;

    size_t indexNext = list->data[physIndex].next;

    if (list->data[indexNext].next == currentIndex)
    {
        list->data[indexNext].next = list->freeHead;
        list->freeHead = list->data[list->freeHead].next;
    }

    list->data[currentIndex].value = pushValue;
    list->data[currentIndex].prev  = physIndex;
    list->data[currentIndex].next  = indexNext;

    list->data[indexNext].prev = currentIndex;

    list->data[physIndex].next = currentIndex;

    list->size = list->size + 1;

    ASSERT (isListCorrect (list));

    return currentIndex;
} 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void listDestroyNode (list_t* list, size_t physIndex)
{
    ASSERT (list != nullptr);

    size_t prev = list->data[physIndex].prev;
    size_t next = list->data[physIndex].next;

    list->data[physIndex].value = DeletePoison;
    list->data[physIndex].next  = list->freeHead;
    list->data[physIndex].prev  = FreeValue;

    list->freeHead = physIndex;

    list->data[prev].next = next;
    list->data[next].prev = prev;

    list->size = list->size - 1;

    ASSERT (isListCorrect (list));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t getPhysicalByLogical (list_t* list, size_t logicalIndex)
{
    ASSERT (list != nullptr);

    if (logicalIndex > list->size)
    {
        printf ("Error in function: %s. The index is larger than the list size!\n", __func__);
        return 0;
    }

    if (list->isListSorted == true)
    {
        return list->head - 1 + logicalIndex;
    }

    size_t index = 1;
    size_t physicalIndex = list->head;

    while (index < logicalIndex)
    {
        physicalIndex = list->data[physicalIndex].next;
        index++;
    }

    return physicalIndex;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t getLogicalByPhysical (list_t* list, size_t physIndex)
{
    size_t logicalIndex = 1;
    size_t currentIndex = list->head;

    while (currentIndex != physIndex)
    {
        currentIndex = list->data[currentIndex].next;

        if (currentIndex == 0)
        {
            printf ("Error in function: %s. The currentIndex == 0!\n", __func__);
            return 0; 
        }
        
        logicalIndex++;
    }

    return logicalIndex;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

elem_t listTailRemove (list_t* list)
{
    elem_t value = list->data[list->tail].value;

    size_t newTail = 0;

    if (list->data[list->tail].prev != 0)
    {
        newTail = list->data[list->tail].prev;
    }
    else
    {
        newTail = 1;
    }

    list->data[list->tail].prev  = FreeValuePrev;
    list->data[list->tail].value = DeletePoison;
    list->data[list->tail].next  = list->freeHead;

    list->freeHead = list->tail;
    list->tail     = newTail;

    list->size = list->size - 1;

    return value;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

elem_t listHeadRemove (list_t* list)
{
    elem_t value = list->data[list->head].value;

    size_t newHead = list->data[list->head].next;

    list->data[list->head].prev  = FreeValuePrev;
    list->data[list->head].value = DeletePoison;
    list->data[list->head].next  = list->freeHead;

    list->freeHead = list->head;

    list->head = newHead;

    list->size = list->size - 1;
    
    return value;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void clearList (list_t* list)
{
    ASSERT (list != nullptr);
    ASSERT (isListCorrect (list));

    if (isListEmpty (list))
    {
        printf ("Error in function: %s. There is nothing to clean, the list is already empty!\n", __func__);
    }

    while (list->size != 0)
    {
        listTailRemove (list);
    }

    ASSERT (isListCorrect (list));
    ASSERT (isListEmpty   (list));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void listLinearize (list_t* list)
{
    ASSERT (list != nullptr);

    node_t* newNodesArray = (node_t*) calloc (list->capacity + 1, sizeof (node_t)); 
    ASSERT (newNodesArray != nullptr);

    newNodesArray[0].value = ValueNullData;

    size_t node = list->head;

    if (list->size == 0)
    {
        node = 0;
    }

    list->freeHead = 0;

    for (size_t index = 1; index <= list->capacity; index++)
    {
        if (list->data[node].prev != FreeValuePrev)
        {
            newNodesArray[index].prev  = index - 1;
            newNodesArray[index].next  = index + 1;
            newNodesArray[index].value = list->data[node].value;
        }
        else
        {
            if (list->freeHead == 0)
            {
                list->freeHead = index;
            }

            newNodesArray[index].prev = FreeValuePrev;
            newNodesArray[index].next = (index + 1) % (list->capacity + 1);
        }

        node = list->data[node].next;
    }

    free (list->data);

    list->isListSorted = true;
    list->head         = 1;
    list->tail         = list->size;
    list->data         = newNodesArray;

    ASSERT (isListCorrect (list));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~