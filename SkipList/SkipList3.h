#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <cstdlib>
#include <functional>

template <typename T, class CmpLess = std::less<T>>
class SkipList
{
protected:
    struct SkipNode;

    struct SkipLevel
    {
        SkipNode *m_next = nullptr;
        unsigned long m_span = 0;
    };

    struct SkipNode
    {
        T m_data;
        SkipLevel *m_levelArray = nullptr;
        SkipNode *m_prev = nullptr;
    };

public:
    SkipList();
    ~SkipList();

    template <typename U>
    const T *find(U &&data);

    template <typename U>
    bool insert(U &&data);

    template <typename U>
    bool remove(U &&data);

protected:
    // 层高上限
    const static unsigned char MAX_LEVEL = 32;
    // 增加1层的概率参数
    const static int LEVEL_THRESHOLD = 0.25 * RAND_MAX;

    // 用户数据比较: a < b
    template <typename U>
    bool customDataLess(U &&a, U &&b) { return m_cmpLess(a, b); }
    // 用户数据比较: a == b
    template <typename U>
    bool customDataEqual(U &&a, U &&b) { return !m_cmpLess(a, b) && !m_cmpLess(b, a); }

    // 生成节点层高
    unsigned char genLevel();

    // 创建有level层的节点
    SkipNode *createNode(unsigned char level);
    // 释放节点
    void releaseNode(SkipNode *node);

    // 找到最后一个小于data的节点，并返回updateArray和rankArray
    template <typename U>
    SkipNode *findLastLessThan(U &&data, SkipNode **updateArray, unsigned long *rankArray);

    // 用户数据比较对象
    CmpLess m_cmpLess;

    SkipNode *m_head = nullptr;
    SkipNode *m_tail = nullptr;

    // 当前的最大层高
    unsigned char m_level = 0;
    // 节点总数
    unsigned long m_length = 0;
};

template <typename T, class CmpLess>
SkipList<T, CmpLess>::SkipList()
{
    m_head = createNode(MAX_LEVEL);
    m_level = 1;
}

template <typename T, class CmpLess>
SkipList<T, CmpLess>::~SkipList()
{
    while (m_head)
    {
        auto node = m_head;
        m_head = m_head->m_levelArray[0].m_next;
        releaseNode(node);
    }
}

template <typename T, class CmpLess>
template <typename U>
const T *SkipList<T, CmpLess>::find(U &&data)
{
    SkipNode *updateArray[MAX_LEVEL];
    unsigned long rankArray[MAX_LEVEL];

    findLastLessThan(std::forward<U>(data), updateArray, rankArray);
    auto nextNode = updateArray[0]->m_levelArray[0].m_next;
    if (nextNode && customDataEqual(std::forward<U>(nextNode->m_data), std::forward<U>(data)))
    {
        return &nextNode->m_data;
    }

    return nullptr;
}

template <typename T, class CmpLess>
template <typename U>
bool SkipList<T, CmpLess>::insert(U &&data)
{
    SkipNode *updateArray[MAX_LEVEL];
    unsigned long rankArray[MAX_LEVEL];

    findLastLessThan(std::forward<U>(data), updateArray, rankArray);
    auto nextNode = updateArray[0]->m_levelArray[0].m_next;
    if (nextNode && customDataEqual(std::forward<U>(nextNode->m_data), std::forward<U>(data)))
    {
        return false;
    }

    auto level = genLevel();
    auto newNode = createNode(level);
    newNode->m_data = std::forward<U>(data);

    if (level > m_level)
    {
        for (auto i = m_level; i < level; ++i)
        {
            updateArray[i] = m_head;
            updateArray[i]->m_levelArray[i].m_span = m_length;
            rankArray[i] = 0;
        }
        m_level = level;
    }

    for (auto i = 0; i < level; ++i)
    {
        auto prevNode = updateArray[i];
        auto &newNodeLevel = newNode->m_levelArray[i];
        auto &prevNodeLevel = prevNode->m_levelArray[i];

        newNodeLevel.m_next = prevNodeLevel.m_next;
        prevNodeLevel.m_next = newNode;

        newNodeLevel.m_span = rankArray[i] + prevNodeLevel.m_span - rankArray[0];
        prevNodeLevel.m_span = rankArray[0] - rankArray[i] + 1;
    }

    for (auto i = level; i < m_level; ++i)
    {
        updateArray[i]->m_levelArray[i].m_span++;
    }

    if (newNode->m_levelArray[0].m_next)
    {
        newNode->m_levelArray[0].m_next->m_prev = newNode;
    }
    else
    {
        m_tail = newNode;
    }

    newNode->m_prev = updateArray[0] == m_head ? nullptr : updateArray[0];

    m_length++;

    return true;
}

template <typename T, class CmpLess>
template <typename U>
bool SkipList<T, CmpLess>::remove(U &&data)
{
    SkipNode *updateArray[MAX_LEVEL];
    unsigned long rankArray[MAX_LEVEL];

    findLastLessThan(std::forward<U>(data), updateArray, rankArray);

    auto nextNode = updateArray[0]->m_levelArray[0].m_next;
    if (!nextNode || !customDataEqual(std::forward<U>(nextNode->m_data), std::forward<U>(data)))
    {
        return false;
    }

    for (auto i = 0; i < m_level; ++i)
    {
        auto curNode = updateArray[i];
        if (curNode->m_levelArray[i].m_next == nextNode)
        {
            curNode->m_levelArray[i].m_next = nextNode->m_levelArray[i].m_next;
            curNode->m_levelArray[i].m_span += nextNode->m_levelArray[i].m_span - 1;
        }
        else
        {
            curNode->m_levelArray[i].m_span--;
        }
    }

    if (nextNode->m_levelArray[0].m_next)
    {
        nextNode->m_levelArray[0].m_next->m_prev = nextNode->m_prev;
    }
    else
    {
        m_tail = nextNode->m_prev;
    }

    while (m_level > 1 && m_head->m_levelArray[m_level - 1].m_next == nullptr)
    {
        --m_level;
    }

    m_length--;

    releaseNode(nextNode);

    return true;
}

template <typename T, class CmpLess>
unsigned char SkipList<T, CmpLess>::genLevel()
{
    unsigned char level = 1;
    while (level < MAX_LEVEL && rand() <= LEVEL_THRESHOLD)
        ++level;
    return level;
}

template <typename T, class CmpLess>
typename SkipList<T, CmpLess>::SkipNode *SkipList<T, CmpLess>::createNode(unsigned char level)
{
    auto node = new SkipNode;
    node->m_levelArray = new SkipLevel[level];
    return node;
}

template <typename T, class CmpLess>
void SkipList<T, CmpLess>::releaseNode(SkipNode *node)
{
    if (!node)
        return;
    delete[] node->m_levelArray;
    delete node;
}

template <typename T, class CmpLess>
template <typename U>
typename SkipList<T, CmpLess>::SkipNode *SkipList<T, CmpLess>::findLastLessThan(U &&data, SkipNode **updateArray, unsigned long *rankArray)
{
    auto curNode = m_head;
    auto curLevel = m_level;

    rankArray[curLevel - 1] = 0;
    while (curLevel)
    {
        if (curLevel < m_level)
        {
            rankArray[curLevel - 1] = rankArray[curLevel];
        }

        auto nextNode = curNode->m_levelArray[curLevel - 1].m_next;
        while (nextNode &&
               customDataLess(std::forward<U>(nextNode->m_data), std::forward<U>(data)))
        {
            rankArray[curLevel - 1] += curNode->m_levelArray[curLevel - 1].m_span;
            curNode = nextNode;
            nextNode = curNode->m_levelArray[curLevel - 1].m_next;
        }

        updateArray[curLevel - 1] = curNode;

        --curLevel;
    }

    return curNode;
}

#endif // _SKIPLIST_H_