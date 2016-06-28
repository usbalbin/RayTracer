#pragma once

#include <vector>

template<typename T>
class ArraySlice
{
public:
	ArraySlice(std::vector<T> vector, int startElement);
	ArraySlice(std::vector<T> vector, int startElement, int size);
	ArraySlice(T* pointer, int startElement, int size);

	T& operator[](int index);
	int size();
	T* data();
private:
	T* pointer;
	int elementCount;
};



template<typename T>
inline ArraySlice<T>::ArraySlice(std::vector<T> vector, int startElement)
{
	int size = vector.size() - startElement;
	this(vector, startElement, size);
}

template<typename T>
inline ArraySlice<T>::ArraySlice(std::vector<T> vector, int startElement, int size)
{
	T* pointer = vector.data();
	this->pointer = &pointer[startElement];
	this->elementCount = size;
}

template<typename T>
inline ArraySlice<T>::ArraySlice(T* pointer, int startElement, int size)
{
	this->pointer = &pointer[startElement];
	this->elementCount = size;
}

template<typename T>
inline T& ArraySlice<T>::operator[](int index)
{
	return pointer[index];
}

template<typename T>
inline int ArraySlice<T>::size()
{
	return size;
}

template<typename T>
inline T * ArraySlice<T>::data()
{
	return pointer;
}


