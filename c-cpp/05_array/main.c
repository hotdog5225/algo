#include "Array_gp.h"

int main() {

    // 获取数组结构体指针
    struct Array* myarr = arrayCreate();
    // 初始化数组结构体（分配数组内存）
    arrayInit(myarr, 3, 4);// 这里分配3个int（4字节大小）
    // 将elem插入到第一个位置
    int elem = 10;
    arrayInsert(myarr, 1, &elem);

    printf("%02d", ((int *)(myarr->p))[0]);

    return 0;
}
