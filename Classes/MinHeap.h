#ifndef __MinHeap_H__
#define __MinHeap_H__
#include <vector>

using namespace std;
/*最小堆 */
template <typename T>
class MinHeap
{
public:
	vector<T> m_heap;
	int m_currentSize;
public:
	static const int DEFAULT_HEAP_SIZE = 10;
public:
	MinHeap(int sz = DEFAULT_HEAP_SIZE)
		:m_currentSize(0)
	{
		m_heap = vector<T>();
	}
	MinHeap(T arr[], int n)
	:m_currentSize(n)
	{
		//赋值
		m_heap = vector<T>(arr);

		int currentPos = (m_currentSize - 2) / 2;
		//局部自上而下下滑调整
		while (currentPos >= 0)
		{
			siftDown(currentPos, m_currentSize - 1);
			currentPos--;
		}
	}
	~MinHeap()
	{
		m_heap.clear();
	}
	bool insert(const T& x)
	{
		m_heap.push_back(x);
		//元素个数大于0，才向上调整
		if (m_currentSize > 0)
			this->siftUp(m_currentSize);
		m_currentSize++;

		return true;
	}
	bool removeMin(T& x)
	{
		//获取最小的数值
		x = m_heap.front();

		m_heap[0] = m_heap[--m_currentSize];
		m_heap.pop_back();

		if (m_currentSize > 0)
			siftDown(0,m_currentSize - 1);

		return true;
	}
	bool empty() const { return  m_currentSize == 0; }
	void clear() { m_heap.clear(); m_currentSize = 0;}
public:
	//从start到m下滑调整成为最小堆
	void siftDown(int start, int m)
	{
		int i = start;
		//j是i的左子女
		int j = 2 * i + 1;
		T temp = m_heap[i];

		while (j <= m)
		{
			//j指向两个子女中的小者
			if (j < m && m_heap[j] > m_heap[j + 1])
				j++;
			//小则不做调整
			if (temp < m_heap[j])
				break;
			else//进行调整，小者上移
			{
				m_heap[i] = m_heap[j];

				i = j;
				j = 2 * j + 1;
			}
		}
		m_heap[i] = temp;
	}
	//从start到0上滑称为最小堆
	void siftUp(int start)
	{
		int j = start;
		int i = (j - 1) / 2;
		T temp = m_heap[j];
		//沿父节点路径向上直达根
		while (j > 0)
		{
			//父节点值小，不做调整
			if (m_heap[i] <= temp)
				break;
			else//调整
			{
				m_heap[j] = m_heap[i];

				j = i;
				i = (i - 1) / 2;
			}
		}
		m_heap[j] = temp;
	}
};
#endif