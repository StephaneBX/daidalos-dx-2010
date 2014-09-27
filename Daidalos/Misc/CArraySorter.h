#ifndef __CARRAYSORTER_H
#define	__CARRAYSORTER_H

/**
* CArraySorter template class.
* Manages array sorting algorithms.
*/
template <typename T> class CArraySorter
{
private:
	int m_temp;
	int * m_order;
	T m_temp2;
	T * m_arrayList;

public:

	/**
	* Constructor
	*/
	CArraySorter()
	{
		this->m_order = NULL;
		this->m_arrayList = NULL;
	}
	
	/**
	* Destructor
	*/
	~CArraySorter(){}

	void BubbleSort(T * const arrayList, int * const order, const int arraySize)
	{
		this->m_arrayList = arrayList;
		this->m_order = order;
		BubbleSortPerform(arraySize);
	}

	void QuickSort(T * const arrayList, int * const order, const int first, const int last)
	{
		this->m_arrayList = arrayList;
		this->m_order = order;
		QuickSortPerform(first, last);
	}

private:

	void BubbleSortPerform(const int arraySize)
	{
		bool toDo = true;

		for(int i = 0 ; i < arraySize && toDo ; i++)
		{
			toDo = false;
			for(int j = 0 ; j < arraySize - 1 - i ; j++)
			{
				if(this->m_arrayList[j] < this->m_arrayList[j + 1])
				{
					Swap(j, j + 1);
					toDo = true;
				}
			}
		}
	}

	void QuickSortPerform(const int first, const int last)
	{
		int pivot;
		if(first < last)
		{
			pivot = QuickSortPivotSelection(first, last);
			pivot = QuickSortPartition(first, last, pivot);
			QuickSortPerform(first, pivot - 1);
			QuickSortPerform(pivot + 1, last);
		}
	}

	void Swap(const int a, const int b)
	{
		this->m_temp2 = this->m_arrayList[a];
		this->m_arrayList[a] = this->m_arrayList[b];
		this->m_arrayList[b] = this->m_temp2;

		if(this->m_order != NULL)
			SwapOrder(a, b);
	}

	void SwapOrder(const int a, const int b)
	{
		this->m_temp = this->m_order[a];
		this->m_order[a] = this->m_order[b];
		this->m_order[b] = this->m_temp;
	}

	int QuickSortPartition(const int first, const int last, const int pivot)
	{
		int j;
		Swap(pivot, last);
		j = first;
		for(int i = first ; i < last ; i++)
		{
			if(this->m_arrayList[i] > this->m_arrayList[last])
			{
				Swap(i, j);
				j++;
			}
		}
		Swap(last, j);
		return j;
	}

	int QuickSortPivotSelection(const int first, const int last)
	{
		return first;
	}
};

#endif
