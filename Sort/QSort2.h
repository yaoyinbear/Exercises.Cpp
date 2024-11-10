#ifndef _QSORT_H_
#define _QSORT_H_

#include <cstdlib>
#include <functional>

template <typename T>
void quickSort(T *array, size_t size, bool (*cmpLess)(const T &, const T &))
{
    if (size <= 1)
    {
        return;
    }

    const auto pivot = array[rand() % size];
    auto i = 0;
    auto j = size - 1;
    auto index = 0;
    while (index <= j)
    {
        if (cmpLess(array[index], pivot))
        {
            std::swap(array[i++], array[index++]);
        }
        else if (cmpLess(pivot, array[index]))
        {
            std::swap(array[index], array[j--]);
        }
        else
        {
            ++index;
        }
    }
    quickSort(array, i, cmpLess);
    quickSort(array + j + 1, size - j - 1, cmpLess);
}

#endif // _QSORT_H_