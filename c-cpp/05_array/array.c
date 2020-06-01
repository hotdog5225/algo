#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct array {
    int size;
    int used;
    int *arr;
};

void dump(struct array *array)
{
    int idx;

    for (idx = 0; idx < array->used; idx++)
        // %02d 格式化符：打印一个整数，不足2位用0补齐
        printf("[%02d]: %08d\n", idx, array->arr[idx]);
}

void alloc(struct array *array)
{
    array->arr = (int *)malloc(array->size * sizeof(int));
}

int insert(struct array *array, int elem)
{
    int idx;
    // 是否还有空间
    if (array->used >= array->size)
        return -1;

    // 找到第一个大于当前元素的数组元素
    for (idx = 0; idx < array->used; idx++) {
        if (array->arr[idx] > elem)
            break;
    }

    // 如果能找到比elem大的数组元素，idx < array->userd, 
    // 如果找不到（要插入的元素比数组所有元素都大）, idx == array->arr
    // 将所有elem大的元素，向后移一个位置,空出elem的位置
    if (idx < array->used)
        memmove(&array->arr[idx+1], &array->arr[idx],
            (array->used - idx) * sizeof(int));

    array->arr[idx] = elem;
    array->used++;
    return idx;
}

int delete(struct array *array, int idx)
{
    if (idx < 0 || idx >= array->used)
        return -1;

    memmove(&array->arr[idx], &array->arr[idx+1],
        (array->used - idx - 1) * sizeof(int));
    array->used--;
    return 0;
}

int search(struct array *array, int elem)
{
    int idx;

    for (idx = 0; idx < array->used; idx++) {
        if (array->arr[idx] == elem)
            return idx;
        // 注意不用扫描全部，只要遇到第一个比ele大的元素，就表示找不到（数组有序）
        if (array->arr[idx] > elem)
            return -1;
    }

    return -1;
}

int main()
{
    int idx;
    struct array ten_int = {10, 0, NULL};

    alloc(&ten_int);
    // malloc申请内存失败，会返回NULL，即ten_int.arr == NULL
    if (!ten_int.arr)
        return -1;
    // 插入元素，按从小到大排列
    insert(&ten_int, 1);
    insert(&ten_int, 3);
    insert(&ten_int, 2);
    printf("=== insert 1, 3, 2\n");
    dump(&ten_int);

    idx = search(&ten_int, 2);
    printf("2 is at position %d\n", idx);
    idx = search(&ten_int, 9);
    printf("9 is at position %d\n", idx);

    printf("=== delete [6] element \n");
    delete(&ten_int, 6);
    dump(&ten_int);
    printf("=== delete [0] element \n");
    delete(&ten_int, 0);
    dump(&ten_int);
    return 0;
}
