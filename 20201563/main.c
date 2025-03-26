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

bool cmp_list_data(const struct list_elem *a, const struct list_elem *b, void *aux) // sort 에서 사용하는 리스트내의 값 비교함수
{
    list_item *item_a = list_entry(a, list_item, elem);
    list_item *item_b = list_entry(b, list_item, elem);
    if (item_a->data < item_b->data)
        return true;
    else
        return false;
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

    if (strcmp(cmd, "list_shuffle") == 0) // list_shuffle 구현
    {
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }

        // 리스트 길이 계산
        int listSize = 0;
        struct list_elem *e;
        for (e = list_begin(&my_lists[idx]); e != list_end(&my_lists[idx]); e = list_next(e))
            listSize++;

        // 요소들을 배열에 저장 (임시배열임. 리스트 각 노드의 주소를 담고 있는 리스트임. )
        // 이론상 이 배열을 쓰지 않고 리스트 자체에서 접근하여 섞을 수도 있지만, 리스트가 다음 노드의 포인터를 지니고 있는 식으로 연결이 되어있기 때문에
        // 해당 노드의 값으로 접근하려면 계속 순회를 해서 접근해야해서 시간 복잡도가 오래걸림. 그래서
        // 해당 노드들의 주소값을 갖고있는 배열을 만들어서, 더 쉽게 접근하도록 하는 거임.
        list_item *items[listSize];
        int i = 0;
        for (e = list_begin(&my_lists[idx]); e != list_end(&my_lists[idx]); e = list_next(e))
            items[i++] = list_entry(e, list_item, elem); // list_entry는 정확히 한 단계만 올라가는 거임. 노드안에 elem 구조체가 있는데. 그 구조체의 원소를 가지고, 한 단계 올라가서, 그 노드의 주소를 반환하는거임.

        // Fisher-Yates 셔플 알고리즘
        for (int i = listSize - 1; i > 0; i--)
        {
            int j = rand() % (i + 1);
            int temp = items[i]->data;
            items[i]->data = items[j]->data;
            items[j]->data = temp;
        }

        return;
    }

    if (strcmp(cmd, "list_push_front") == 0)
    {

        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }

        list_item *item = malloc(sizeof(list_item));
        int value = atoi(arg2);
        item->data = value;

        list_push_front(&my_lists[idx], &item->elem);
        return;
    }

    if (strcmp(cmd, "list_insert") == 0) // list_insert mylist 2 99
    {
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }

        int cnt = atoi(arg2);
        int new_value = atoi(arg3);

        struct list_elem *e = list_begin(&my_lists[idx]);

        for (int i = 0; i < cnt; i++)
        {
            e = list_next(e);
        }

        list_item *item = malloc(sizeof(list_item));
        item->data = new_value;

        list_insert(e, &item->elem);
        return;
    }

    if (strcmp(cmd, "list_remove") == 0) // list_remove mylist 2
    {
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }

        int cnt = atoi(arg2);
        struct list_elem *e = list_begin(&my_lists[idx]);

        for (int i = 0; i < cnt; i++)
        {
            e = list_next(e);
        }

        struct list_elem *removed_e = list_remove(e);
        list_item *removed_item = list_entry(removed_e, list_item, elem); // list_entry 의 인자는 세개임 끝에 두개는sizeof 처럼 타입 그 자체를 넣음.
        free(removed_item);
    }

    if (strcmp(cmd, "list_max") == 0) // list_max mylist
    {
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }

        int max = list_entry(list_begin(&my_lists[idx]), list_item, elem)->data;
        // &my_list[idx]는 list* 이지, list_elem* 이 아님.
        // list_entry 는 list_elem*을 참조하여, 그 elem를 갖고있는 리스트 칸을 꺼내는 아이니까
        // 첫번쨰elem*로 바꿔줘야 하니까 list_begin() 사용
        struct list_elem *e;
        for (e = list_begin(&my_lists[idx]); e != list_end(&my_lists[idx]); e = list_next(e))
        {
            int now_data = list_entry(e, list_item, elem)->data;

            if (now_data > max)
            {
                max = now_data;
            }
        }

        printf("%d\n", max);
    }

    if (strcmp(cmd, "list_sort") == 0) // list_sort <list_name> : list_sort mylist
    {
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }

        list_sort(&my_lists[idx], cmp_list_data, NULL);
        return;
    }
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
