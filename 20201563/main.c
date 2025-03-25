#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"

#define MAX_INPUT 128
#define MAX_LIST 10

struct list my_lists[MAX_LIST];
char *list_names[MAX_LIST];
int list_count = 0;

typedef struct list_item
{
    int data;
    struct list_elem elem;
} list_item;

int find_list_index(const char *name)
{
    for (int i = 0; i < list_count; i++)
    {
        if (strcmp(list_names[i], name) == 0)
            return i;
    }
    return -1;
}

void process_command(char *line)
{
    char cmd[32], arg1[32], arg2[32], arg3[32];
    int value;
    sscanf(line, "%s %s %s %s", cmd, arg1, arg2, arg3);

    if (strcmp(cmd, "create") == 0 && strcmp(arg1, "list") == 0)
    {
        if (list_count >= MAX_LIST)
        {
            printf("Too many lists.\n");
            return;
        }
        list_init(&my_lists[list_count]);
        list_names[list_count] = strdup(arg2);
        list_count++;
        return;
    }

    if (strcmp(cmd, "list_push_back") == 0)
    {
        value = atoi(arg2);
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }
        list_item *item = malloc(sizeof(list_item));
        item->data = value;
        list_push_back(&my_lists[idx], &item->elem);
        return;
    }

    if (strcmp(cmd, "dumpdata") == 0)
    {
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }
        struct list_elem *e;
        for (e = list_begin(&my_lists[idx]); e != list_end(&my_lists[idx]); e = list_next(e))
        {
            list_item *item = list_entry(e, list_item, elem);
            printf("%d ", item->data);
        }
        printf("\n");
        return;
    }

    if (strcmp(cmd, "list_swap") == 0)
    {
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }
        int i = atoi(arg2);
        int j = atoi(arg3);
        struct list_elem *e1 = list_begin(&my_lists[idx]);
        struct list_elem *e2 = list_begin(&my_lists[idx]);
        for (int k = 0; k < i && e1 != list_end(&my_lists[idx]); k++)
            e1 = list_next(e1);
        for (int k = 0; k < j && e2 != list_end(&my_lists[idx]); k++)
            e2 = list_next(e2);
        if (e1 == list_end(&my_lists[idx]) || e2 == list_end(&my_lists[idx]))
        {
            printf("Index out of range.\n");
            return;
        }
        list_item *item1 = list_entry(e1, list_item, elem);
        list_item *item2 = list_entry(e2, list_item, elem);
        int temp = item1->data;
        item1->data = item2->data;
        item2->data = temp;
        return;
    }

    if (strcmp(cmd, "quit") == 0)
    {
        exit(0);
    }

    printf("Unknown command: %s\n", line);
}

int main()
{
    char line[MAX_INPUT];

    while (1)
    {
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        process_command(line);
    }

    return 0;
}
