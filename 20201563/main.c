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

// hash apply square, triplr 에서 사용되는 함수.
void square(struct hash_elem *e, void *aux)
{
    hash_item *item = hash_entry(e, hash_item, elem);
    int squared_value = item->data * item->data;
    item->data = squared_value;
}

void triple(struct hash_elem *e, void *aux)
{
    hash_item *item = hash_entry(e, hash_item, elem);
    int tripled_value = item->data * item->data * item->data;
    item->data = tripled_value;
}

// 해시 관련 구조체
struct hash my_hashes[MAX_HASH];
char *hash_names[MAX_HASH];
int hash_count = 0;

typedef struct hash_item
{
    int data;
    struct hash_elem elem;
} hash_item;

int find_hash_index(const char *name)
{
    for (int i = 0; i < hash_count; i++)
    {
        if (strcmp(hash_names[i], name) == 0)
            return i;
    }
    return -1;
}

unsigned hash_func(const struct hash_elem *e, void *aux)
{
    hash_item *item = hash_entry(e, hash_item, elem);
    return hash_int(item->data);
}

bool cmp_hash_data(const struct hash_elem *a, const struct hash_elem *b, void *aux)
{
    hash_item *item_a = hash_entry(a, hash_item, elem);
    hash_item *item_b = hash_entry(b, hash_item, elem);

    if (item_a->data < item_b->data)
        return true;
    else
        return false;
}

// bitmap 관려 구조체 및 함수
struct bitmap *my_bitmaps[MAX_BITMAP];
char *bitmap_names[MAX_BITMAP];
int bitmap_count = 0;

int find_bitmap_index(const char *name)
{
    for (int i = 0; i < bitmap_count; i++)
    {
        if (strcmp(bitmap_names[i], name) == 0)
            return i;
    }
    return -1;
}

/*

/////////////////
실제 명령어 처리함수
/////////////////

*/
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

        if (list_empty(&my_lists[idx]))
        {
            printf("List is empty.\n");
            return;
        }
        else
        {
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

<<<<<<< HEAD
    // list_min <list_name>
    // ex: list_min mylist
    if (strcmp(cmd, "list_min") == 0)
    {
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }

        if (list_empty(&my_lists[idx]))
        {
            printf("List is empty.\n");
            return;
        }
        else
        {
            int min = list_entry(list_begin(&my_lists[idx]), list_item, elem)->data;
            // &my_list[idx]는 list* 이지, list_elem* 이 아님.
            // list_entry 는 list_elem*을 참조하여, 그 elem를 갖고있는 리스트 칸을 꺼내는 아이니까
            // 첫번쨰elem*로 바꿔줘야 하니까 list_begin() 사용
            struct list_elem *e;
            for (e = list_begin(&my_lists[idx]); e != list_end(&my_lists[idx]); e = list_next(e))
            {
                int now_data = list_entry(e, list_item, elem)->data;

                if (now_data > min)
                {
                    min = now_data;
                }
            }

            printf("%d\n", min);
        }
    }

    // list_size <list_name>
    // ex: list_size mylist
    if (strcmp(cmd, "list_size") == 0)
    {
        int idx = find_list_index(arg1);
        if (idx == -1)
        {
            printf("List not found.\n");
            return;
        }

        int size = list_size(&my_lists[idx]);
        printf("%d\n", size);
        == == == =
                     // list_insert_ordered <list_name> <value>
            if (strcmp(cmd, "list_insert_ordered") == 0)
        {
            int idx = find_list_index(arg1);
            if (idx == -1)
                return;
            list_item *item = malloc(sizeof(list_item));
            item->data = atoi(arg2);
            list_insert_ordered(&my_lists[idx], &item->elem, cmp_list_data, NULL);
            return;
        }

        // list_unique <list_name>
        if (strcmp(cmd, "list_unique") == 0)
        {
            int idx = find_list_index(arg1);
            if (idx == -1)
                return;

            // 중복 제거 전 정렬 필요
            list_sort(&my_lists[idx], cmp_list_data, NULL);
            list_unique(&my_lists[idx], NULL, cmp_list_data, NULL);
            return;
        }

        // list_splice <dest_list> <index> <src_list>
        if (strcmp(cmd, "list_splice") == 0)
        {
            int dest_idx = find_list_index(arg1);
            int src_idx = find_list_index(arg3);
            if (dest_idx == -1 || src_idx == -1)
                return;
            int pos = atoi(arg2);
            struct list_elem *e = list_begin(&my_lists[dest_idx]);
            for (int i = 0; i < pos && e != list_end(&my_lists[dest_idx]); i++)
                e = list_next(e);
            list_splice(e, list_begin(&my_lists[src_idx]), list_end(&my_lists[src_idx]));

            // src 리스트는 요소를 모두 옮겼으므로 초기화
            list_init(&my_lists[src_idx]);
            return;
        }

        // list_pop_front <list_name>
        if (strcmp(cmd, "list_pop_front") == 0)
        {
            int idx = find_list_index(arg1);
            if (idx == -1 || list_empty(&my_lists[idx]))
                return;
            struct list_elem *e = list_pop_front(&my_lists[idx]);
            list_item *front_item = list_entry(e, list_item, elem);
            free(front_item);
            return;
        }

        // list_pop_back <list_name>
        if (strcmp(cmd, "list_pop_back") == 0)
        {
            int idx = find_list_index(arg1);
            if (idx == -1 || list_empty(&my_lists[idx]))
                return;
            struct list_elem *e = list_pop_back(&my_lists[idx]);
            list_item *back_item = list_entry(e, list_item, elem);
            free(back_item);
            return;
        }

        // delete <list_name>
        if (strcmp(cmd, "delete") == 0)
        {
            int idx = find_list_index(arg1);
            if (idx == -1)
                return;
            struct list_elem *e = list_begin(&my_lists[idx]);
            while (e != list_end(&my_lists[idx]))
            {
                struct list_elem *next = list_next(e);
                list_item *item = list_entry(e, list_item, elem);
                list_remove(e);
                free(item);
                e = next;
            }
            // 리스트 이름 메모리도 해제
            free(list_names[idx]);
            list_names[idx] = NULL;
            return;
        }

        // 해시 관련
        if (strcmp(cmd, "create") == 0 && strcmp(arg1, "hash") == 0)
        {
            if (hash_count >= MAX_HASH)
            {
                printf("Too many hashes.\n");
                return;
            }
            hash_init(&my_hashes[hash_count], hash_func, cmp_hash_data, NULL);
            hash_names[hash_count] = strdup(arg2);
            hash_count++;
            return;
        }

        if (strcmp(cmd, "hash_insert") == 0)
        {
            int idx = find_hash_index(arg1);
            if (idx == -1)
                return;
            hash_item *item = malloc(sizeof(hash_item));
            item->data = atoi(arg2);
            hash_insert(&my_hashes[idx], &item->elem);
            return;
        }

        if (strcmp(cmd, "hash_find") == 0)
        {
            int idx = find_hash_index(arg1);
            if (idx == -1)
                return;
            hash_item temp;
            temp.data = atoi(arg2);
            struct hash_elem *e = hash_find(&my_hashes[idx], &temp.elem);
            printf("%s\n", e ? "true" : "false");
            return;
        }

        if (strcmp(cmd, "hash_delete") == 0)
        {
            int idx = find_hash_index(arg1);
            if (idx == -1)
                return;
            hash_item temp;
            temp.data = atoi(arg2);
            struct hash_elem *e = hash_delete(&my_hashes[idx], &temp.elem);
            free(hash_entry(e, hash_item, elem));
        }

        if (strcmp(cmd, "hash_clear") == 0)
        {
            int idx = find_hash_index(arg1);
            if (idx == -1)
                return;
            hash_clear(&my_hashes[idx], free);
            return;
        }

        if (strcmp(cmd, "hash_empty") == 0)
        {
            int idx = find_hash_index(arg1);
            if (idx == -1)
                return;
            printf("%s\n", hash_empty(&my_hashes[idx]) ? "true" : "false");
            return;
        }

        if (strcmp(cmd, "hash_size") == 0)
        {
            int idx = find_hash_index(arg1);
            if (idx == -1)
                return;
            printf("%zu\n", hash_size(&my_hashes[idx]));
            return;
        }

        if (strcmp(cmd, "hash_apply") == 0)
        {
            int idx = find_hash_index(arg1);
            if (idx == -1)
                return;

            if (strcmp(arg2, "square") == 0)
                hash_apply(&my_hashes[idx], square);
            else if (strcmp(arg2, "triple") == 0)
                hash_apply(&my_hashes[idx], triple);
>>>>>>> 58522df (dd)

            return;
        }

<<<<<<< HEAD
        // list_empty <list_name>
        // ex: list_empty mylist
        if (strcmp(cmd, "list_empty") == 0)
        {
            int idx = find_list_index(arg1);
            if (idx == -1)
            {
                printf("List not found.\n");
                return;
            }

            bool is_empty = list_empty(&my_lists[idx]);
            printf("%s\n", is_empty ? "true" : "false");

            return;
        }

        // list_front <list_name>
        // ex: list_front mylist
        if (strcmp(cmd, "list_front") == 0)
        {
            int idx = find_list_index(arg1);
            if (idx == -1)
            {
                printf("List not found.\n");
                return;
            }

            if (list_empty(&my_lists[idx]))
            {
                printf("List is empty.\n");
                return;
            }
            else
            {
                struct list_elem *list_front_e = list_begin(&my_lists[idx]);
                // list_begin()은 흔히 말하는 “head 노드”가 아니라,실제 첫 번째 노드를 가리킴.
                list_item *list_front_i = list_entry(list_front_e, list_item, elem);

                int list_front_value = list_front_i->data;
                printf("%d\n", list_front_value);

                return;
            }

            return;
        }

        // list_back <list_name>
        // ex: list_back mylist
        if (strcmp(cmd, "list_back") == 0)
        {
            int idx = find_list_index(arg1);
            if (idx == -1)
            {
                printf("List not found.\n");
                return;
            }

            if (list_empty(&my_lists[idx]))
            {
                printf("List is empty.\n");
                return;
            }
            else
            {
                struct list_elem *list_last_e = list_end(&my_lists[idx])->prev;
                // list_end()는 데이터가 없는 더미(dumb) 노드이고, list_begin 이랑 다름
                // 리스트 순회할 때 도착지 역할만 함.list_item *list_last_i = list_entry(list_last_e, list_item, elem);
                list_item *list_last_i = list_entry(list_last_e, list_item, elem);

                int list_last_value = list_last_i->data;
                printf("%d\n", list_last_value);

                return;
            }

            return;
        }

        // list_reverse <list_name>
        // ex: list_reverse mylist
        if (strcmp(cmd, "list_reverse") == 0)
        {
            int idx = find_list_index(arg1);
            if (idx == -1)
            {
                printf("List not found.\n");
                return;
            }

            if (list_empty(&my_lists[idx]))
            {
                printf("List is empty.\n");
                return;
            }
            else
            {
                list_reverse(&my_lists[idx]);
            }

            == == == =
                         /* 비트맵 함수 명령어 처림림*/

                if (strcmp(cmd, "create") == 0 && strcmp(arg1, "bitmap") == 0)
            {
                if (bitmap_count >= MAX_BITMAP)
                {
                    printf("Too many bitmaps.\n");
                    return;
                }

                int size = atoi(arg3);
                my_bitmaps[bitmap_count] = bitmap_create(size);
                bitmap_names[bitmap_count] = strdup(arg2);
                bitmap_count++;
                return;
            }

            // bitmap_size <name>
            if (strcmp(cmd, "bitmap_size") == 0)
            {
                int idx = find_bitmap_index(arg1);
                if (idx == -1)
                    return;
                printf("%zu\n", bitmap_size(my_bitmaps[idx]));
                return;
            }

            // bitmap_mark <name> <index>
            if (strcmp(cmd, "bitmap_mark") == 0)
            {
                int idx = find_bitmap_index(arg1);
                if (idx == -1)
                    return;
                int index = atoi(arg2);
                bitmap_mark(my_bitmaps[idx], index);
                return;
            }

            // bitmap_reset <name> <index>
            if (strcmp(cmd, "bitmap_reset") == 0)
            {
                int idx = find_bitmap_index(arg1);
                if (idx == -1)
                    return;
                int index = atoi(arg2);
                bitmap_reset(my_bitmaps[idx], index);
                return;
            }

            // bitmap_flip <name> <index>
            if (strcmp(cmd, "bitmap_flip") == 0)
            {
                int idx = find_bitmap_index(arg1);
                if (idx == -1)
                    return;
                int index = atoi(arg2);
                bitmap_flip(my_bitmaps[idx], index);
                return;
            }

            // bitmap_test <name> <index>
            if (strcmp(cmd, "bitmap_test") == 0)
            {
                int idx = find_bitmap_index(arg1);
                if (idx == -1)
                    return;
                int index = atoi(arg2);
                printf("%s\n", bitmap_test(my_bitmaps[idx], index) ? "true" : "false");
                return;
            }

            // bitmap_set_all <name> <0 or 1>
            if (strcmp(cmd, "bitmap_set_all") == 0)
            {
                int idx = find_bitmap_index(arg1);
                if (idx == -1)
                    return;
                bool value = atoi(arg2);
                bitmap_set_all(my_bitmaps[idx], value);
                return;
            }

            // bitmap_set <name> <index> <0 or 1>
            if (strcmp(cmd, "bitmap_set") == 0)
            {
                int idx = find_bitmap_index(arg1);
                if (idx == -1)
                    return;
                int index = atoi(arg2);
                bool value = atoi(arg3);
                bitmap_set(my_bitmaps[idx], index, value);
                return;
            }

            // bitmap_dump <name>
            if (strcmp(cmd, "bitmap_dump") == 0)
            {
                int idx = find_bitmap_index(arg1);
                if (idx == -1)
                    return;
                bitmap_dump(my_bitmaps[idx]);
>>>>>>> 58522df (dd)
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
