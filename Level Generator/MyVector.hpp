#pragma once
//#include <iostream>
template <typename T>
struct vector
{
private:

	int capacity;
	
public:
	int size;
	T* arr;
	vector()
	{
		//memset(arr, -322, capacity);
		size = 0;
		capacity = 2;
		arr = new T[capacity];
	}
	void realloc()
	{
		T* temp;
		temp = new T[capacity];
		std::copy(arr, arr + capacity, temp);
		delete[] arr;
		arr = new T[2 * capacity];
		std::copy(temp, temp + capacity, arr);
		capacity *= 2;
	}
	void push_back(T element)
	{
		if (size+1 > capacity) realloc();
		arr[size] = element;
		size++;
	}
	int find(T element)
	{
		for (int i = 0; i < size; i++)
			if (arr[i] == element) return i;
		return -1;
	}
	void clear()
	{
		for (int i = 0; i < size; i++)
			arr[i] = static_cast<T>(-126);
		size = 0;
	}
	T last()
	{
		return arr[size - 1];
	}
	void delete_last()
	{
		if (size == 0) return; //throw
		arr[size - 1] = static_cast<T>(-127);
		size--;
	}
	T& operator[](unsigned int index)
	{
		return arr[index];
	}
	T*& operator& (struct vector<T>)
	{
		return arr + 0;
	}

	~vector()
	{
		delete[] arr;
	}
};

