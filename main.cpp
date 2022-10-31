#include <stdio.h>
#include <stdbool.h>

#include "list.h"

int main ()
{
    list_t list = {};

    listCtor (&list, 10);
    listDump (&list);

    listPushBegin (&list, 20);
    listDump (&list);
    listPushBegin (&list, 10);
    listDump (&list);

    listDtor (&list);

    return 0;
}

