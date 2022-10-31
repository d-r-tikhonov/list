//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
// #include <TXLib.h>

#include "list.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FILE* logFile = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int listCtor (list_t* list, size_t capacity)
{
    assert (list != nullptr);

    if ((logFile = fopen ("log.html", "w")) == nullptr)
    {
        printf ("Error in function: %s. Error opening logFile!\n", __func__);
        return errno;
    }

    if (capacity == 0)
    {
        printf ("Error in function: %s. Error capacity = 0!\n", __func__);
        return -1;
    }    

    list->data = (node_t*) calloc (capacity + 1, sizeof (node_t));

    assert (list->data != nullptr);
    
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
    assert (list != nullptr);

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

    if ((fclose (logFile)) != 0)
    {
        printf ("Error in function: %s. Error closing logFile!\n", __func__);
        return errno;
    }

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int isListDestructed (list_t* list)
{
    assert (list != nullptr);
    
    if (list-> head == Poison && list->tail == Poison && list->freeHead == Poison && list->size == Poison && list->capacity == Poison)
        return 1;

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int isListEmpty (list_t* list)
{
    assert (list != nullptr);

    if (list->head == 1 && list->tail == 1 && list->freeHead == 1 && list->size == 0)
        return 1;

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef GRAPHVIZ_DUMP

    void listDump (const list_t* list)
    {
        assert (list != nullptr);
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
        fprintf (logFile, "<img src=\"dump%u.svg\"/>\n", dumpNum);

        dumpNum++;
    }

#else

    void listDumpFunc (list_t* list, size_t line, const char file[ParamMaxSize], const char func[ParamMaxSize])
    {
        fprintf (logFile, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~START LIST DUMP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

        if (list->status)
        {
            fprintf(logFile, "Lists's pointer is null\n");
        }

        fprintf (logFile, "Called at %s at %s(%lu)\n", file, func, line);
        fprintf (logFile, "List status: OK!\n"); 


        fprintf (logFile, "Head: %lu\nTail: %lu\nFreeHead: %lu\nSize: %lu\nCapacity: %lu\n", 
            list->head, list->tail, list->freeHead, list->size, list->capacity);

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
    assert (list != nullptr);

    node_t* data = list->data;

    if (data[0].prev != 0 || data[0].next != 0)
    {
        printf ("Error in function: %s. Verification FAILED!\n", __func__);
        return 0;
    }

    for (size_t index = 1; index < list->capacity; index++)
    {
        if (data[index].prev == FreeValuePrev) 
        {
            if (data[index].next < 0 || data[index].next > list->capacity)
            {
                printf ("Error in function: %s. Verification FAILED!\n", __func__);
                return 0;
            }
            continue;
        }

        if (data[index].prev < 0 || data[index].prev > list->capacity)
        {
            printf ("Error in function: %s. Verification FAILED!\n", __func__);
            return 0;
        }

        if (data[index].next < 0 || data[index].next > list->capacity)
        {
            printf ("Error in function: %s. Verification FAILED!\n", __func__);
            return 0;
        }

        if (data[data[index].prev].next != index && index != list->head)
        {
            printf ("Error in function: %s. Verification FAILED!\n", __func__);
            return 0;
        }

        if (data[data[index].next].prev != index && index != list->tail)
        {
            printf ("Error in function: %s. Verification FAILED!\n", __func__);
            return 0;
        }
    }
    
    fprintf (logFile, "<font color=\"green\">Verification is OK!</font>\n");

    return 1;
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
        data[index].value   = FreeValue;
        data[index].prev    = FreeValuePrev;

    }

    data[newCapacity - 1].next = list->freeHead;

    list->freeHead = list->capacity;

    list->capacity = newCapacity;
    list->data     = data;

    return data;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPushBegin (list_t* list, elem_t pushValue)
{
    assert (list != nullptr);

    size_t currentIndex = 0;
    currentIndex = list->freeHead;

    list->freeHead = list->data[currentIndex].next;

    list->data[currentIndex].value = pushValue;

    list->data[currentIndex].next = list->head;
    list->data[list->head].prev = currentIndex;
    list->data[currentIndex].prev = 0;

    list->head = currentIndex;

    list->size = list->size + 1;

    assert (isListCorrect (list));

    return currentIndex;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPushEnd (list_t* list, elem_t pushValue)
{
    assert (list != nullptr);

    size_t currentIndex = 0;
    currentIndex = list->freeHead;

    list->freeHead = list->data[currentIndex].next;

    list->data[currentIndex].value = pushValue;

    list->data[currentIndex].prev = list->tail;
    list->data[currentIndex].next = 0;

    list->data[list->tail].next = currentIndex;

    list->tail = currentIndex;
    list->size = list->size + 1;

    assert (isListCorrect (list));

    return currentIndex;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPushBefore (list_t* list, size_t physIndex, elem_t pushValue)
{
    assert (list != nullptr);
    assert (physIndex <= list->capacity + 1);
    assert (!(list->data[physIndex].prev == FreeValuePrev && (list->size != 0 || physIndex != list->tail)));

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

    assert (isListCorrect (list));

    return currentIndex;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t listPushAfter (list_t* list, size_t physIndex, elem_t pushValue)
{
    assert (list != nullptr);
    assert (physIndex <= list->capacity + 1);
    assert (!(list->data[physIndex].prev == FreeValue && (list->size != 0 || physIndex != list->tail)));

    size_t currentIndex = 0;
    currentIndex = list->freeHead;

    list->freeHead = list->data[currentIndex].next;

    size_t indexNext = list->data[physIndex].next;

    list->data[currentIndex].value = pushValue;
    list->data[currentIndex].prev  = physIndex;
    list->data[currentIndex].next  = indexNext;

    list->data[indexNext].prev = currentIndex;

    list->data[physIndex].next = currentIndex;

    list->size = list->size + 1;

    assert (isListCorrect (list));

    return currentIndex;
} 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void listDestroyNode (list_t* list, size_t physIndex)
{
    assert (list != nullptr);

    size_t prev = list->data[physIndex].prev;
    size_t next = list->data[physIndex].next;

    list->data[physIndex].value = DeletePoison;
    list->data[physIndex].next  = list->freeHead;
    list->data[physIndex].prev  = FreeValue;

    list->freeHead = physIndex;

    list->data[prev].next = next;
    list->data[next].prev = prev;

    list->size = list->size - 1;

    assert (isListCorrect (list));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//void поиска элемента по его номеру в последовательности (операция индексации)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//void totalDestroy

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
