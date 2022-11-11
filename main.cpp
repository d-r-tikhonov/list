#include <stdio.h>
#include <stdbool.h>

#include "list.h"

int main ()
{
    list_t list = {};

    listCtor (&list, 5);
    listDump (&list);

    listRecalloc (&list, 6);
    listDump (&list);

    listPushBegin (&list, 20);
    listDump (&list);

    listPushBegin (&list, 10);
    listDump (&list);

    listPushAfter (&list, 2, 0xDED32);
    listDump (&list);

    // listDestroyNode (&list, 2);
    // listDump (&list);

    listLinearize(&list);
    listDump(&list);

    clearList (&list);
    listDump (&list);

    listDtor (&list);

    return 0;
}

