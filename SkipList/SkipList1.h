#pragma once

#include<functional>

class SkipList
{
protected:
    using CmpFunc = int(*)(void*, void*);

    struct SkipNode;

    struct SkipLevel
    {
        SkipNode *m_next = nullptr;
        unsigned long m_span = 0;
    };

    struct SkipNode
    {
        long long m_score = 0;
        void *m_data = nullptr;
        SkipLevel *m_levelArray = nullptr;
        SkipNode *m_prev = nullptr;
    };


public:
    SkipList(CmpFunc cmpFunc = [](void *a, void *b) ->int { return a < b ? -1 : a == b ? 0 : 1; });
    ~SkipList();

    bool insert(long long score, void *data);
    bool remove(long long score, void *data);


protected:
    // 层高上限
    const static unsigned char MAX_LEVEL = 32;
    // 增加1层的概率参数
    const static int LEVEL_THRESHOLD = 0.25 * RAND_MAX;

    // 生成节点层高
    unsigned char genLevel();

    // 创建有level层的节点
    SkipNode* createNode(unsigned char level);
    // 释放节点
    void releaseNode(SkipNode *node);

    // 找到最后一个小于{score, data}的节点，
    void findLastLessThan(long long score, void *data, SkipNode **updateArray, unsigned long *rankArray);

    // 用户数据比较函数指针
    CmpFunc m_cmpFunc;

    SkipNode *m_head = nullptr;
    SkipNode *m_tail = nullptr;

    // 当前的最大层高
    unsigned char m_level = 0;
    // 节点总数
    unsigned long m_length = 0;
};

