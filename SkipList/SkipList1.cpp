#include "SkipList1.h"

SkipList::SkipList(CmpFunc cmpFunc)
    : m_cmpFunc{ cmpFunc }
{
    m_head = createNode(MAX_LEVEL);
    m_level = 1;
}

SkipList::~SkipList()
{
    while (m_head)
    {
        auto node = m_head;
        m_head = m_head->m_levelArray[0].m_next;
        releaseNode(node);
    }
}

bool SkipList::insert(long long score, void *data)
{
    SkipNode* updateArray[MAX_LEVEL];
    unsigned long rankArray[MAX_LEVEL];

    findLastLessThan(score, data, updateArray, rankArray);

    auto level = genLevel();
    auto newNode = createNode(level);
    newNode->m_score = score;
    newNode->m_data = data;

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

        newNodeLevel.m_span = rankArray[i] + prevNodeLevel.m_span -  rankArray[0];
        prevNodeLevel.m_span = rankArray[0] - rankArray[i] + 1;

        if (i == 0)
        {
            if (!newNodeLevel.m_next)
            {
                m_tail = newNode;
            }
            else
            {
                newNodeLevel.m_next->m_prev = newNode;
            }

            if (prevNode == m_head)
            {
                newNode->m_prev = nullptr;
            }
            else
            {
                newNode->m_prev = prevNode;
            }
        }
    }

    for (auto i = level; i < m_level; ++i)
    {
        updateArray[i]->m_levelArray[i].m_span++;
    }

    m_length++;

    return true;
}

bool SkipList::remove(long long score, void* data)
{
    SkipNode* updateArray[MAX_LEVEL];
    unsigned long rankArray[MAX_LEVEL];

    findLastLessThan(score, data, updateArray, rankArray);

    auto nextNode = updateArray[0]->m_levelArray[0].m_next;
    if (!nextNode || nextNode->m_score != score || m_cmpFunc(nextNode->m_data, data) != 0)
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

unsigned char SkipList::genLevel()
{
    unsigned char level = 1;
    while (level < MAX_LEVEL && rand() <= LEVEL_THRESHOLD) ++level;
    return level;
}

SkipList::SkipNode* SkipList::createNode(unsigned char level)
{
    auto node = new SkipNode;
    node->m_levelArray = new SkipLevel[level];
    return node;
}

void SkipList::releaseNode(SkipNode *node)
{
    if (!node) return;
    delete[] node->m_levelArray;
    delete node;
}

void SkipList::findLastLessThan(long long score, void *data, SkipNode** updateArray, unsigned long* rankArray)
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
               (nextNode->m_score < score 
                || (nextNode->m_score == score 
                    && m_cmpFunc(nextNode->m_data, data) < 0)))
        { 
            rankArray[curLevel - 1] += curNode->m_levelArray[curLevel - 1].m_span;
            curNode = nextNode;
            nextNode = curNode->m_levelArray[curLevel - 1].m_next;
        }

        updateArray[curLevel - 1] = curNode;

        --curLevel;
    }
}
