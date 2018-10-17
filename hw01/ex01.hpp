#include <iostream>
#include <cassert>
#include <array>
#include <set>
#include <vector>
#include <random>
#include <ctime>

template<int MAX_SIZE>
class PriorityBufferArray
{
private:
	std::array<int, MAX_SIZE> m_items;
	int itemCnt = 0;

public:
	void insert(int item)
	{
		int i = 0;
		while (i < itemCnt && item > m_items[i])
			i++;

        if (i < MAX_SIZE && i != itemCnt && m_items[i] == item)
			return;

		if (itemCnt == MAX_SIZE)
		{
			if (i == MAX_SIZE)
				i--;

			// posun dole
			for (int j = 0; j < i; j++)
				m_items[j] = m_items[j + 1];
		}
		else
		{
			// posun hore
			for (int j = itemCnt; j > i; j--)
				m_items[j] = m_items[j - 1];
		}

		m_items[i] = item;

		if (itemCnt < MAX_SIZE)
			itemCnt++;
	}

	int top()
	{
		if (itemCnt == 0)
			return -1;

		return m_items[itemCnt - 1];
	}

	void remove()
	{
		if (itemCnt == 0)
			return;

		itemCnt--;
	}
};

template<int MAX_SIZE>
class PriorityBufferSet
{
private:
	std::set<int> m_items;

public:
	void insert(int item)
	{
		if (m_items.size() == MAX_SIZE)
		{
			if (m_items.find(item) == m_items.end())
			{
				m_items.erase(*m_items.begin());
				m_items.insert(item);
			}
		}
		m_items.insert(item);
	}

	int top()
	{
		return *m_items.rbegin();
	}

	void remove()
	{
		m_items.erase(*m_items.rbegin());
	}
};
