#ifndef LIST_H
#define LIST_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef int elem_t;
#define GRAPHVIZ_DUMP

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// const size_t ListInitValue       = 10;
// const size_t ListMultiplier      = 2;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const size_t ParamMaxSize        = 16;
const size_t FreeValue           = 0xF3EE;
const size_t Poison              = 0xDED32;
const size_t DeletePoison        = 0xFEE1DEAD;
const size_t FreeValuePrev       = 0xCAFEBABE;
const size_t ValueNullData       = 0xBEADFACE;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef NDEBUG
    #define ASSERT(condition)                                                                   \
        if (!(condition))                                                                       \
        {                                                                                       \
            printf ("Assertion FAILED! Error in %s. FILE: %s. LINE: %d. FUNCTION: %s\n",        \
                    #condition, __FILE__, __LINE__, __PRETTY_FUNCTION__);                       \
        }
#else
    #define ASSERT(condition) ;
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct node_t
{
    size_t prev;
    size_t next;

    elem_t value;
};

struct list_t
{
    node_t* data;

    size_t head;
    size_t tail;
    size_t freeHead;
    size_t size;
    size_t capacity;

    bool   isListSorted;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef GRAPHVIZ_DUMP
    #define listDump(list) \
        listTextDump (list, __LINE__, __FILE__, __PRETTY_FUNCTION__)
    #else
    #define listDump(list) \
        listGraphDump (list)
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int         listCtor                (list_t* list, size_t capacity);                
int         listDtor                (list_t* list);                                   
int         isListDestructed        (list_t* list);
int         isListEmpty             (list_t* list);
void        listGraphDump           (const list_t* list);
void        listTextDump            (list_t* list, size_t line, const char file[ParamMaxSize], const char func[ParamMaxSize]);
size_t      listHead                (list_t* list);                                 
size_t      listTail                (list_t* list);
size_t      listNextElem            (list_t* list, size_t physIndex);               
size_t      listPrevElem            (list_t* list, size_t physIndex);            
node_t*     listRecalloc            (list_t* list, const size_t newCapacity);
bool        isListCorrect           (list_t* list); 
size_t      listPushAfter           (list_t* list, size_t physIndex, elem_t pushValue);
size_t      listPushBefore          (list_t* list, size_t physIndex, elem_t pushValue);
size_t      listPushHead            (list_t* list, elem_t pushValue);
size_t      listPushTail            (list_t* list, elem_t pushValue);
void        listDestroyNode         (list_t* list, size_t physIndex);
elem_t      listTailRemove          (list_t* list);
elem_t      listHeadRemove          (list_t* list);
void        listLinearize           (list_t* list);
void        clearList               (list_t* list);
size_t      getPhysicalByLogical    (list_t* list, size_t logicalIndex);
size_t      getLogicalByPhysical    (list_t* list, size_t physIndex);
int         openLog                 (void);
int         closeLog                (void);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#endif