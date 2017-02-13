// Implementation of the templated Vector class
// ECE4893/8893 lab 3
// YOUR NAME HERE

#include <iostream> // debugging
#include "Vector.h"
//#include <new>
#include "String.h"

#ifdef Debug
#include <stdio.h>
#endif


// Your implementation here
// Fill in all the necessary functions below
using namespace std;

// Default constructor
template <typename T>
Vector<T>::Vector()
{
  count = 0;
  reserved = 0;
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& rhs)
{
  count = rhs.Size();
  elements = (T*)malloc(count*sizeof(T));
  for(size_t i=0; i<count; i++)
  {
    new (&elements[i]) T(rhs[i]);
  }
}

// Assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs)
{

}

#ifdef GRAD_STUDENT
// Other constructors
template <typename T>
Vector<T>::Vector(size_t nReserved)
{ 
  // Initialize with reserved memory
  //elements = (T*)malloc(nReserved*sizeof(T)); 
}

template <typename T>
Vector<T>::Vector(size_t n, const T& t)
{ 
  // Initialize with "n" copies of "t"
  elements = (T*)malloc(n*sizeof(t)); 
  for(size_t i=0; i<n; i++)
  {
    new (&elements[i]) T(t);
  }
  count = n;

}

template <typename T>
void Vector<T>::Reserve(size_t n)
{ 
  // Reserve extra memory
  elements = (T*)malloc(n*sizeof(T)); 
  reserved = n - count;
}

#endif

// Destructor
template <typename T>
Vector<T>::~Vector()
{
  for(size_t i=0; i<count; ++i)
  {
    elements[i].~T();
  }
  free(elements);
}

// Add and access front and back
template <typename T>
void Vector<T>::Push_Back(const T& rhs)
{
  if(reserved>0)
  {
    new (&elements[count]) T(rhs);
    count++;
    reserved--;
  }


  else{
  T* temp = (T*)malloc((count+1)*sizeof(T));

  for(size_t i=0; i<count; ++i)
  {
    new (&temp[i]) T(elements[i]);
    elements[i].~T();
  }

  if(Empty()) {new (&temp[count]) T(rhs); count++; }
  
  else
  {
    new (&temp[count]) T(rhs);
    count++;
    free(elements);
  }
  elements = temp;
  }
}

template <typename T>
void Vector<T>::Push_Front(const T& rhs)
{
  T* temp = (T*)malloc((count+1)*sizeof(T));

  for(size_t i=0; i<count; i++)
  {
    new (&temp[i+1]) T(elements[i]);
    elements[i].~T();
  }

  if(Empty()) {new (&temp[0]) T(rhs); count++; }
  
  else
  {
    new (&temp[0]) T(rhs);
    count++;
    free(elements);
  }
  elements = temp;
}

template <typename T>
void Vector<T>::Pop_Back()
{ 
  // Remove last element
  elements[count-1].~T();
  count--;
}

template <typename T>
void Vector<T>::Pop_Front()
{ 
  if(!Empty()){
  T* temp = (T*)malloc((count-1)*sizeof(T));

  for(size_t i=1; i<count; i++)
  {
    new (&temp[i-1]) T(elements[i]);
    elements[i].~T();
  }
  elements[0].~T();
  count--;
  free(elements);
  elements = temp;
  }
}

// Element Access
template <typename T>
T& Vector<T>::Front() const
{
  return elements[0];
}

// Element Access
template <typename T>
T& Vector<T>::Back() const
{
  return elements[count-1];
}

template <typename T>
const T& Vector<T>::operator[](size_t i) const
{ // const element access
  return elements[i];
}

template <typename T>
T& Vector<T>::operator[](size_t i)
{//nonconst element access
  return elements[i];
}

template <typename T>
size_t Vector<T>::Size() const
{
  return count;
}

template <typename T>
bool Vector<T>::Empty() const
{
  bool judge;
  if(count==0) judge =true;
  else judge = false;
  return judge;
}

// Implement clear
template <typename T>
void Vector<T>::Clear()
{
  for(size_t i=0; i<count; i++)
  {
    elements[i].~T();
  }
  count=0;
}

// Iterator access functions
template <typename T>
VectorIterator<T> Vector<T>::Begin() const
{
  return VectorIterator<T>(elements);
}

template <typename T>
VectorIterator<T> Vector<T>::End() const
{
  return VectorIterator<T>(&elements[count]);
}

#ifdef GRAD_STUDENT
// Erase and insert
template <typename T>
void Vector<T>::Erase(const VectorIterator<T>& it)
{
  size_t num = (size_t)(it.current - elements);

  elements[num].~T();

  for(size_t i=num+1; i<count; i++)
  {
    new (&elements[i-1]) T(elements[i]);
    elements[i].~T();
  }
  count--;
}

template <typename T>
void Vector<T>::Insert(const T& rhs, const VectorIterator<T>& it)
{
  T* temp = (T*)malloc((count+1)*sizeof(T));
  size_t num = (size_t)(it.current - elements);
  for(size_t i=0; i<num; i++)
  {
    new (&temp[i]) T(elements[i]);
    elements[i].~T();
  }

  new (&temp[num]) T(rhs);
  
  for(size_t i=num; i<count; i++)
  {
    new (&temp[i+1]) T(elements[i]);
    elements[i].~T();
  }

  count++;
  free(elements);
  elements = temp;
}
#endif

// Implement the iterators

// Constructors
template <typename T>
VectorIterator<T>::VectorIterator()
{
}

template <typename T>
VectorIterator<T>::VectorIterator(T* c)
{
  current = c;
}

// Copy constructor
template <typename T>
VectorIterator<T>::VectorIterator(const VectorIterator<T>& rhs)
{
}

// Iterator defeferencing operator
template <typename T>
T& VectorIterator<T>::operator*() const
{
  return *current;
}

// Prefix increment
template <typename T>
VectorIterator<T>  VectorIterator<T>::operator++()
{
  return VectorIterator<T>(current++);
}

// Postfix increment
template <typename T>
VectorIterator<T> VectorIterator<T>::operator++(int)
{
  return VectorIterator<T>(current++);
}

// Comparison operators
template <typename T>
bool VectorIterator<T>::operator !=(const VectorIterator<T>& rhs) const
{
  if(current!=rhs.current) return true;
  else return false;
}

template <typename T>
bool VectorIterator<T>::operator ==(const VectorIterator<T>& rhs) const
{
  if(current==rhs.current) return true;
  else return false;
}




