#ifndef _QSORT_H_
#define _QSORT_H_

#include <cstdlib>
#include <functional>

class QSort
{
public:
    template <typename T>
    using CmpLess = bool (*)(const T &, const T &);

    QSort() {};
    ~QSort() {};

    template <typename T>
    void sort(T *array, size_t size, CmpLess<T> cmpLess);

protected:
    template <typename T>
    void quickSort(T *array, size_t left, size_t right, CmpLess<T> cmpLess);

    template <typename T>
    size_t partition(T *array, size_t left, size_t right, CmpLess<T> cmpLess);
};

template <typename T>
void QSort::sort(T *array, size_t size, CmpLess<T> cmpLess)
{
    quickSort(array, 0, size - 1, cmpLess);
}

template <typename T>
void QSort::quickSort(T *array, size_t left, size_t right, CmpLess<T> cmpLess)
{
    if (left >= right)
    {
        return;
    }

    auto pivot = partition(array, left, right, cmpLess);
    quickSort(array, left, pivot - 1, cmpLess);
    quickSort(array, pivot + 1, right, cmpLess);
}

template <typename T>
size_t QSort::partition(T *array, size_t left, size_t right, CmpLess<T> cmpLess)
{
    auto pivot = left + rand() % (right - left + 1);
    std::swap(array[pivot], array[right]);

    auto i = left;
    auto j = right;
    while (i < j)
    {
        while (i < j && !cmpLess(array[j], array[left]))
            --j;
        while (i < j && !cmpLess(array[left], array[i]))
            ++i;
        std::swap(array[i], array[j]);
    }
    std::swap(array[left], array[i]);

    return i;
}

#endif // _QSORT_H_