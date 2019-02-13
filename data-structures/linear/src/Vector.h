#ifndef AISDI_LINEAR_VECTOR_H
#define AISDI_LINEAR_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

#define INITIAL_CAPACITY 8

namespace aisdi
{

template <typename Type>
class Vector
{
public:
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using const_pointer = const Type*;
  using const_reference = const Type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  size_type size;
  size_type capacity;
  pointer storage;
  friend class ConstIterator;

public:
  Vector() : size(0),  capacity(INITIAL_CAPACITY), storage(new Type[capacity])
  {}

  Vector(std::initializer_list<Type> l): size(l.size()), capacity(l.size()), storage(new Type[capacity])
  {
    size_type i = 0;
    for (auto it = l.begin(); it != l.end(); ++it)
    {
      storage[i++] = std::move(*it);
    }
  }

  Vector(const Vector& other): size(other.size), capacity(other.capacity), storage(new Type[capacity])
  {
     for (size_type i = 0; i < other.getSize(); i++)
     {
      storage[i] = other.storage[i];
     }
  }

  Vector(Vector&& other) 
  {
    size = other.size;
    storage = other.storage;
    capacity = size;

    other.size = 0;
    other.capacity = 0;
    other.storage = nullptr;
  }

  ~Vector()
  {
    delete[] storage;
  }

  Vector& operator=(const Vector& other)
  {
    if(this == &other)
    {
      return *this;
    }
    size = other.size;

    delete[] storage;
    storage = new Type[other.capacity];

    capacity = other.capacity;

    for (size_type i = 0; i < other.size; ++i)
    {
      storage[i] = other.storage[i];
    }

    return *this;
  }

  Vector& operator=(Vector&& other)
  {
    delete[] storage;
    size = other.size;
    capacity = size;
    other.size = 0;
    other.capacity = 0;

    storage = other.storage;
    other.storage = nullptr;

    return *this;
  }

  bool isEmpty() const
  {
    return size == 0;
  }

  size_type getSize() const
  {
    return size;
  }

  void append(const Type& item)
  {
    if(isFull())
    {
      increaseCapacity();
    }

    storage[size] = item;
    size++;
  }

  void prepend(const Type& item)
  {
    if(isFull())
    {
      increaseCapacity();
    }

    for(size_type i = size; i > 0; --i)
    {
      storage[i] = storage[i-1];
    }

    storage[0] = item;
    size++;
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    if(isFull())
    {
      increaseCapacity();
    }
    if(insertPosition.index > size)
    {
      throw std::out_of_range("cannot insert element");
    } 
    if(insertPosition == cbegin())
    {
      prepend(item);
      return;
    }

    if(insertPosition == cend())
    {
      append(item);
      return;
    }

    for(size_type i = size; i > insertPosition.index; --i)
    {
      storage[i] = storage[i-1];
    }

    storage[insertPosition.index] = item;
    size++;
  }

  Type popFirst()
  {

    if(isEmpty())
    {
      throw std::logic_error("cannot remove element from empty vector");
    }

    Type item = storage[0];

    for(size_type i = 0; i < size; i++)
    {
      storage[i] = storage[i+1];
    }

    size--;

    return item;
  }

  Type popLast()
  {
    if(isEmpty()) {
      throw std::logic_error("cannot remove element from empty vector");
    }
    return storage[--size];
  }

  void erase(const const_iterator& possition)
  {
    if(possition.index >= size) {
      throw std::out_of_range("cannot remove element");
    }

    for(size_type i = possition.index; i < size; i++)
    {
      storage[i] = storage[i+1];
    }

    size--;
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    for(size_type i = firstIncluded.index, j = lastExcluded.index; j < size; i++, j++)
    {
      storage[i] = storage[j];
    }

    size = size - (lastExcluded.index - firstIncluded.index);
  }

  iterator begin()
  {
    return iterator(*this, 0);
  }

  iterator end()
  {
    return iterator(*this, size);
  }

  const_iterator cbegin() const
  {
    return const_iterator(*this, 0);
  }

  const_iterator cend() const
  {
    return const_iterator(*this, size);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }

private: 
  bool isFull() {
    return size == capacity;
  }

  void increaseCapacity()
  {
    capacity = capacity * 2;

    pointer tmpStorage = new Type[capacity];

    for (size_type i = 0; i < size; ++i)
    {
        tmpStorage[i] = storage[i];
    }

    delete[] storage;
    storage = tmpStorage;
  }
};

template <typename Type>
class Vector<Type>::ConstIterator
{
  friend class Vector<Type>;
protected:
  const Vector<Type>& vector;
  size_type index;
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  using pointer = typename Vector::const_pointer;
  using reference = typename Vector::const_reference;

  explicit ConstIterator(const Vector<Type>& vector, size_type index) : vector(vector), index(index)
  {}

  reference operator*() const
  {
    if (index == vector.size)
    {
      throw std::out_of_range("cannot dereference iterator");
    }
    return vector.storage[index];
  }

  ConstIterator& operator++()
  {
    if(index == vector.size) 
    {
      throw std::out_of_range("cannot increment iterator");
    }
    index++;
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator tmp = *this;
    operator++();
    return tmp;
  }

  ConstIterator& operator--()
  {
    if(index <= 0) 
    {
      throw std::out_of_range("cannot decrement iterator");
    }
    index--;
    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator tmp = *this;
    operator--();
    return tmp;
  }

  ConstIterator operator+(difference_type d) const
  {
    size_type newIndex = index + d;
    if (newIndex > vector.size)
    {
      throw std::out_of_range("cannot increase iterator");
    }

    return ConstIterator(vector, newIndex);
  }

  ConstIterator operator-(difference_type d) const
  {
    size_type newIndex = index - d;
    if (newIndex < 0)
    {
      throw std::out_of_range("cannot decrease iterator");
    }
    
    return ConstIterator(vector, newIndex);
  }

  bool operator==(const ConstIterator& other) const
  {
    return index == other.index;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return index != other.index;
  }
};

template <typename Type>
class Vector<Type>::Iterator : public Vector<Type>::ConstIterator
{
  friend class Vector<Type>;
public:
  using pointer = typename Vector::pointer;
  using reference = typename Vector::reference;

  explicit Iterator(const Vector<Type>& vector, size_type index) : ConstIterator(vector, index)
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  Iterator operator+(difference_type d) const
  {
    return ConstIterator::operator+(d);
  }

  Iterator operator-(difference_type d) const
  {
    return ConstIterator::operator-(d);
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif // AISDI_LINEAR_VECTOR_H
