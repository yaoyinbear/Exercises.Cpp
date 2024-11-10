#include <cstdio>
#include "QSort2.h"

int main()
{
    printf("begin\n");

    int array[] = {3, 1, 2, 4, 9, 10, 8, 10, 6, 7, 5, 8};
    quickSort<int>(array, sizeof(array) / sizeof(array[0]), [](const int &a, const int &b) -> bool
                   { return a < b; });

    for (auto i : array)
    {
        printf("%d ", i);
    }

    printf("\nend\n");

    return 0;
}