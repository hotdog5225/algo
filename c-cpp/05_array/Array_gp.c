#include "Array.h"

#include <string.h>
#include <stdbool.h>

// 给数据结构体分配内存（注意，只是给结构体分配成员内存，并没有给成员p去申请数组内存）
Array* arrayCreate()
{
    struct Array *array = NULL; // 最终返回数组结构体指针
    array = malloc(sizeof(*array));
    if (NULL == array)
    {
        return NULL;
    }

    array->p = NULL;// 存放数据指针

    array->size = 0;// p指针的数据空间大小, 创建的时候，没有给p申请数组内存，所以size是0
    array->typeSize = 0;// 数据类型的大小
    array->len = 0;// p指针已经使用的空间大小

    array->dup = NULL;// 值复制函数
    array->free = NULL;
    array->match = NULL;

    return array;
}

// 给结构的数组指针p申请数组内存
// size: 申请多少个元素
// typeSize： 每个元素的大小
voi darrayInit(Array *array, int size, int typeSize)
{
    if (NULL == array
        || typeSize <= 0
        || size < 0)
    {
        return;
    }

    // calloc 分配 num * size 个大小的空间,初始化为0
    // 返回内存空间的起始地址, 可以赋值给任意类型
    void *p = calloc(1, size* typeSize);
    if (NULL == p)
    {
        return;
    }

    array->p = p;
    array->len = 0;
    array->size = size;
    array->typeSize = typeSize;
}

// 将元素插入到指定位置
int arrayInsert(Array *array, size_t pos, void *const value)
{
    if (NULL == array)
    {
        return -1;
    }

    if (array->len >= array->size)
    {
        return -2;// 数据已满
    }

    if (pos > array->size || pos <= 0)
    {
        return -3;// 待插入的位置非法 pos从1开始
    }

    char *pBegin = array->p;
    // 从后向前，将元素依次向后挪一位
    for (size_t i = array->len; i > pos - 1; --i)
    {
        // 该元素要挪到的新地址
        void *pNew = pBegin + i * array->typeSize;
        // 元素地址
        void *pOld = pBegin + (i - 1) *array->typeSize;
        // 拷贝元素到新地址（向后挪一位）
        if (NULL != array->dup)
        {
            array->dup(pNew, pOld);
        }
        else
        {
            memcpy(pNew, pOld, array->typeSize);
        }
    }

    // 根据pos, 利用数组的随机访问，定位到要插入的地址
    void *pCopy = (void*)(pBegin + ((pos - 1) * array->typeSize));
    if (NULL != array->dup)
    {
        array->dup(pCopy, value);
    }
    else
    {
        memcpy(pCopy, value, array->typeSize);
    }
    ++array->len;
    return 0;
}

// 按值搜索 O(n)
size_t arraySearchValue(Array *array, void* const value)
{
    if (NULL == array)
    {
        return -1;
    }

    char *pBegin = array->p;
    size_t i = 0;
    for (; i < array->len; ++i)
    {
        int nCmp = 0;
        if (NULL != array->match)
        {
            nCmp = array->match(pBegin + i * array->typeSize, value);
        }
        else
        {
            nCmp = memcmp(pBegin + i * array->typeSize, value, array->typeSize);
        }

        if (nCmp == 0)
        {
            break;
        }
    }

    return i;
}

// 索引随机访问，O(1)
void* arrayIndex(Array *array, size_t index)
{
    if (NULL == array)
    {
        return NULL;
    }

    if (index > array->len
        || index <= 0)
    {
        return NULL;
    }

    char *pBegin = array->p;
    return pBegin + array->typeSize * (index - 1);
}

// 按索引修改，O(1)
int arrayModify(Array *array, size_t pos, void *const value)
{
    if (NULL == array)
    {
        return -1;
    }
    if (pos > array->len
        || pos <= 0)
    {
        return -2;
    }

    char *pBegin = array->p;
    void *pOld = pBegin + (pos - 1) * array->typeSize;
    if (NULL != array->dup)
    {
        array->dup(pOld, value);
    }
    else
    {
        memcpy(pOld, value, array->typeSize);
    }

    return 0;
}

size_t arrayLen(Array *array)
{
    if (NULL == array)
    {
        return 0;
    }

    return array->len;
}

size_t arraySize(Array *array)
{
    if (NULL == array)
    {
        return 0;
    }

    return array->size;
}

// 清空数组
void arrayEmpty(Array *array)
{
    if (NULL == array)
    {
        return;
    }

    free(array->p);
    array->p = NULL;
    free(array);
    array = NULL;
}

// 按值删除，O(n)
void arrayDelValue(Array *array, void *value)
{
    if (NULL == array)
    {
        return;
    }

    char* pBegin = array->p;
    bool bCopy = false;
    for (size_t i = 0; i < array->len; ++i)
    {
        if (!bCopy)
        {
            int nCmp = 0;
            if (NULL != array->match)
            {
                nCmp = array->match(pBegin + i * array->typeSize, value);
            }
            else
            {
                nCmp = memcmp(pBegin + i * array->typeSize, value, array->typeSize);
            }

            if (0 == nCmp)
            {
                bCopy = true;
                continue;
            }
        }
        else
        {
            // 将后面的元素依次往前移动一格
            void *pOld = pBegin + (i + 1) * array->typeSize;// 待移动的元素
            void *pNew = pBegin + i * array->typeSize;// 要移动的位置
            if (NULL != array->dup)
            {
                array->dup(pNew, pOld);
            }
            else
            {
                memcpy(pNew, pOld, array->typeSize);
            }
        }
    }

    if (bCopy)
    {
        --array->len;
    }
}

// 按索引删除 O(n)
void arrayDelIndex(Array *array, size_t pos)
{
    if (NULL == array)
    {
        return;
    }

    if (pos > array->len || pos <= 0)
    {
        return;
    }

    char* pBegin = array->p;
    for (size_t i = pos - 1; i < array->len - 1; ++i)
    {
        // 将pos后面的元素，依次向前移动一个位置
        void *pOld = pBegin + (i + 1) * array->typeSize;// 待移动的元素位置
        void *pNew = pBegin + i * array->typeSize;// 移动dest位置
        if (NULL != array->dup)
        {
            array->dup(pNew, pOld);
        }
        else
        {
            memcpy(pNew, pOld, array->typeSize);
        }
    }

    --array->len;
}
