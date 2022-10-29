#ifndef LIST_H
#define LIST_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef int elem_t;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const size_t ListInitValue       =      10;
const size_t ListMultiplier      =       2;
const size_t FreeValue           = 0xF3EEE;
const size_t Poison              = 0xDED32;
const size_t ParamMaxSize        = 0x00016;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct node_t
{
    size_t prev;
    elem_t value;
    size_t next;
};

struct list_t
{
    node_t* data;

    size_t head;
    size_t tale;
    size_t freeHead;
    size_t size;
    size_t capacity;

    bool isSorted;
    int status;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum ErrorTypes
{
    
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define listDump(list) \
    listDumpFunc (list, __LINE__, __FILE__, __PRETTY_FUNCTION__)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int         listCtor            (list_t* list, size_t capacity);
int         listDtor            (list_t* list);
int         isListDestructed    (list_t* list);
int         isListEmpty         (list_t* list);
void        listDumpFunc        (list_t* list, size_t line, const char file[ParamMaxSize], const char func[ParamMaxSize]);
size_t      listHead            (list_t* list);
size_t      listTale            (list_t* list);
size_t      listNextElem        (list_t* list, size_t physIndex);
size_t      listPrevElem        (list_t* list, size_t physIndex);
node_t*     listRecalloc        (list_t* list, const size_t newCapacity);
size_t      listPushBegin       (list_t* list, size_t physIndex, elem_t value);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#endif