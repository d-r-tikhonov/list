#include <stdio.h>
#include <stdbool.h>

#include "list.h"

int main ()
{
    list_t list = {};

    listCtor (&list, 2);
    listDump (&list);

    listPushHead (&list, 0xF1);
    listDump (&list);

    listPushHead (&list, 0xF2);
    listDump (&list);

    listRecalloc (&list, 3);
    listDump (&list);

    listPushAfter (&list, 2, 0xDED32);
    listDump (&list);

    listLinearize(&list);
    listDump(&list);

    clearList (&list);
    listDump (&list);

    listDtor (&list);

    return 0;
}

