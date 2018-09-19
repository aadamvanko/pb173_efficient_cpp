#include <iostream>
#include <array>

template<int MAX_SIZE>
class PriorityBuffer
{
	private:
		std::array<int, MAX_SIZE> m_items;
		int itemCnt = 0;
		
	public:
		void insert(int item) 
		{
			int i = 0;
			while (i < itemCnt && m_items[i] > item)
				i++;
				
			if (m_items[i] == item)
				return;
		
			for (int j = itemCnt; j > i; j--)
				m_items[j] = m_items[j - 1];
			
			m_items[i] = item;
			
			std::cout << "MAX_SIZE " << MAX_SIZE << std::endl;
			std::cout << "++" << itemCnt << std::endl;
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
		
			for (int i = 0; i < itemCnt - 1; i++)
				m_items[i] = m_items[i + 1];
			itemCnt--;
		}
};

int main() 
{
	PriorityBuffer<3> pb;

	pb.insert(2);
	pb.insert(1);
	pb.insert(3);
	pb.insert(0);

	for (int i = 0; i < 4; i++) 
	{
		std::cout << pb.top() << std::endl;
		pb.remove();
	}
	
	return 0;
}
