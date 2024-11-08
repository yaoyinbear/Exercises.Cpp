// #include <iostream>
#include <cstdio>
#include "SkipList3.h"

int main()
{
    // std::cout << "#begin" << std::endl;
    printf("begin\n");

    SkipList<int> skipList;

    skipList.insert(3);
    skipList.insert(1);
    skipList.insert(2);
    skipList.insert(4);
    skipList.insert(9);
    skipList.insert(10);
    skipList.insert(6);
    skipList.insert(7);
    skipList.insert(5);
    skipList.insert(8);

    // for (int i = 10; i > 0; --i)
    // {
    //     skipList.insert(i);
    // }

    for (int i = 1; i <= 10; ++i)
    {
        auto data = skipList.find(i);
        if (data)
        {
            // std::cout << "find " << i << std::endl;
            printf("find %d\n", i);
        }
        else
        {
            // std::cout << "not find " << i << std::endl;
            printf("not find %d\n", i);
        }
    }

    // std::cout << "#end" << std::endl;
    printf("end\n");

    return 0;
}