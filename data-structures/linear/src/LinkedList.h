#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <iostream>

namespace aisdi
{

template <typename Type>
class LinkedList
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
  struct Node {
    Node *prev;
    Node *next;
    Type data;
    Node() : prev(nullptr), next(nullptr)
    {}

    Node(const Type& data) : prev(nullptr), next(nullptr), data(data)
    {}

    ~Node()
    {}
  };

  Node *sentinel;
  size_type size;

public:

  LinkedList() : size(0)
  {
    sentinel = new Node;
    sentinel->prev = sentinel;
    sentinel->next = sentinel;
  }

  LinkedList(std::initializer_list<Type> l) : LinkedList()
  {
    for (auto it = l.begin(); it != l.end(); ++it)
    {
      append(*it); 
    }
  }

  LinkedList(const LinkedList& other) : LinkedList() 
  {
    for (auto it = other.begin(); it != other.end(); it++)
    {
      append(*it);
    }
  }

  LinkedList(LinkedList&& other)
  {
    sentinel = std::move(other.sentinel);
    size = std::move(other.size);
    other.sentinel = nullptr;
    other.size = 0;
  }

  ~LinkedList()
  {
    while (!isEmpty()) {
      erase(begin());
    }

    delete sentinel;
  }

  LinkedList& operator=(const LinkedList& other)
  {

    if (&other == this) {
      return *this;
    }

    while (!isEmpty()) 
    {
      erase(begin());
    }

    for (auto it = other.begin(); it != other.end(); it++)
    {
      append(*it);
    }
 
    return *this;
  }

  LinkedList& operator=(LinkedList&& other)
  {
    while (!isEmpty()) 
    {
      erase(begin());
    }

    Node* sentinelTmp = sentinel;

    sentinel = std::move(other.sentinel);
    size = std::move(other.size);

    other.sentinel = sentinelTmp;
    other.size = 0;

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
    Node *node = new Node(item);
    sentinel->prev->next = node;
    node->prev = sentinel->prev;
    sentinel->prev = node;
    node->next = sentinel;
    size++;
  }

  void prepend(const Type& item)
  {
    Node *node = new Node(item);
    node->next = sentinel->next;
    node->prev = sentinel;
    node->next->prev = node;
    sentinel->next = node;
    size++;
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    Node *node = new Node(item);
    node->next = insertPosition.node;
    node->prev = insertPosition.node->prev;
    insertPosition.node->prev->next = node;
    insertPosition.node->prev = node;

    size++;
  }

  Type popFirst()
  {
    if(isEmpty()) {
      throw std::out_of_range("cannot erase the element from the empty list");
    }

    Type tmp = *begin();
    erase(begin());

    return tmp;
  }

  Type popLast()
  {
    if(isEmpty()) {
      throw std::out_of_range("cannot erase the element from the empty list");
    }

    Type tmp = *(end()-1);
    erase(end()-1);

    return tmp;
  }

  void erase(const const_iterator& possition)
  {
    if (possition.node == sentinel)
    {
      throw std::out_of_range("cannot erase end iterator (sentinel)");
    }
    
    size--;

    possition.node->prev->next = possition.node->next;
    possition.node->next->prev = possition.node->prev;

    delete possition.node;
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    for(auto it = firstIncluded; it != lastExcluded; it++)
    {
      erase(it);
    }
  }

  iterator begin()
  {
    return iterator(*this, sentinel->next);
  }

  iterator end()
  {
    return iterator(*this, sentinel);
  }

  const_iterator cbegin() const
  {
    return const_iterator(*this, sentinel->next);
  }

  const_iterator cend() const
  {
    return const_iterator(*this, sentinel);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename Type>
class LinkedList<Type>::ConstIterator
{
protected:
  const LinkedList& list;
  Node *node;
  friend void LinkedList <Type>::erase(const const_iterator&);
  friend void LinkedList <Type>::insert(const const_iterator&, const Type&);

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename LinkedList::value_type;
  using difference_type = typename LinkedList::difference_type;
  using pointer = typename LinkedList::const_pointer;
  using reference = typename LinkedList::const_reference;

  explicit ConstIterator(const LinkedList& list, Node *node) : list(list), node(node) 
  {}

  reference operator*() const
  {
    if (node == list.sentinel) {
      throw std::out_of_range("cannot dereference this iterator");
    }
    return (node->data);
  }

  ConstIterator& operator++()
  {
    if (node == list.sentinel) {
      throw std::out_of_range("cannot increment iterator since it is end one");
    } 

    node = node -> next;
    return *this;
  }

  ConstIterator operator++(int)
  {
    auto it = *this;
    operator++();
    return it;  
  }

  ConstIterator& operator--()
  {
    if (node == list.sentinel -> next) {
      throw std::out_of_range("cannot decrement iterator since it is first one");
    } 

    node = node -> prev;
    return *this;
  }

  ConstIterator operator--(int)
  {
    auto it = *this;
    operator--();
    return it;    
  }

  ConstIterator operator+(difference_type d) const
  {
    auto it = *this;
    for (difference_type i = 0; i < d; ++i)
    {
      ++it;
    }
    return it;
  }

  ConstIterator operator-(difference_type d) const
  {
    auto it = *this;
    for (difference_type i = 0; i < d; ++i)
    {
      --it;
    }
    return it;
  }

  bool operator==(const ConstIterator& other) const
  {
    return this->node == other.node;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return this->node != other.node;
  }
};

template <typename Type>
class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator
{
public:
  using pointer = typename LinkedList::pointer;
  using reference = typename LinkedList::reference;

  explicit Iterator(const LinkedList& list, Node *node) : const_iterator(list, node)
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

#endif // AISDI_LINEAR_LINKEDLIST_H
