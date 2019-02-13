#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>
#include <functional>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using list_type = std::list<value_type>;
  class ConstIterator;
  class Iterator;
  friend class ConstIterator;
  friend class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  static const size_type hash_table_size = 1299709;
  size_type size;
  list_type *hash_table;

  size_type hash(const key_type &key) const
  {
    return key % hash_table_size;
  }

  void insert(value_type value)
  {
    hash_table[hash(value.first)].push_back(value);
    size++;
  }

  void erase()
  {
    if (hash_table == nullptr)
    {
      return;
    }

    for (size_type i = 0; i < hash_table_size; i++)
    {
      if (!hash_table[i].empty())
      {
        // hash_table[i].erase(hash_table[i].begin(), hash_table[i].end());
        hash_table[i].clear();
      }
    }
    size = 0;
  }

  bool is_in_list(value_type val, const list_type &list) const
  {
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if ((*it).first == val.first && (*it).second == val.second)
      {
        return true;
      }
    }

    return false;
  }

public:
  HashMap() : size(0), hash_table(new list_type[hash_table_size])
  {
  }

  HashMap(std::initializer_list<value_type> list) : HashMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      insert(*it);
    }
  }

  HashMap(const HashMap &other) : HashMap()
  {
    for (auto it = other.begin(); it != other.end(); ++it)
    {
      insert(*it);
    }
  }

  HashMap(HashMap &&other) //: HashMap()
  {
    hash_table = other.hash_table;
    size = other.size;

    other.hash_table = nullptr;
    other.size = 0;
  }

  ~HashMap()
  {
    erase();
    delete[] hash_table;
  }

  HashMap &operator=(const HashMap &other)
  {
    if (this != &other)
    {
      erase();

      for (auto it = other.begin(); it != other.end(); ++it)
      {
        insert(*it);
      }
    }

    return *this;
  }

  HashMap &operator=(HashMap &&other)
  {
    if (this != &other)
    {
      erase();
      delete[] hash_table;
      hash_table = std::move(other.hash_table);
      size = std::move(other.size);
      other.hash_table = nullptr;
      other.size = 0;
    }

    return *this;
  }

  bool isEmpty() const
  {
    return size == 0;
  }

  mapped_type &operator[](const key_type &key)
  {
    auto &list = hash_table[hash(key)];
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if ((*it).first == key)
      {
        return (*it).second;
      }
    }

    list.push_back(std::make_pair(key, mapped_type{}));
    size++;

    return (*(--list.end())).second;
  }

  const mapped_type &valueOf(const key_type &key) const
  {
    auto &list = hash_table[hash(key)];
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if ((*it).first == key)
      {
        return (*it).second;
      }
    }

    throw std::out_of_range("cannot get value of nonexistent element");
  }

  mapped_type &valueOf(const key_type &key)
  {
    list_type &list = hash_table[hash(key)];
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if ((*it).first == key)
      {
        return (*it).second;
      }
    }

    throw std::out_of_range("cannot get value of nonexistent element");
  }

  const_iterator find(const key_type &key) const
  {
    size_type hash_val = hash(key);
    auto &list = hash_table[hash_val];
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if ((*it).first == key)
      {
        return const_iterator(*this, hash_val, it);
      }
    }
    return end();
  }

  iterator find(const key_type &key)
  {
    size_type hash_val = hash(key);
    list_type &list = hash_table[hash_val];
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if ((*it).first == key)
      {
        return iterator(*this, hash_val, it);
      }
    }

    return end();
  }

  void remove(const key_type &key)
  {
    list_type &list = hash_table[hash(key)];
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if ((*it).first == key)
      {
        list.erase(it);
        size--;
        return;
      }
    }

    throw std::out_of_range("cannot remove nonexistent element");
  }

  void remove(const const_iterator &it)
  {
    list_type &list = hash_table[it.index];
    if (it.it != list.end())
    {
      list.erase(it.it);
      size--;
      return;
    }

    throw std::out_of_range("cannot remove nonexistent element");
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const HashMap &other) const
  {
    if (other.size != size)
    {
      return false;
    }

    for (size_type i = 0; i < hash_table_size; ++i)
    {
      if (hash_table[i].size() != other.hash_table[i].size())
      {
        return false;
      }
      for (auto it = hash_table[i].begin(); it != hash_table[i].end(); ++it)
      {
        if (!is_in_list(*it, other.hash_table[i]))
        {
          return false;
        }
      }
    }

    return true;
  }

  bool operator!=(const HashMap &other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    for (size_type i = 0; i < hash_table_size; i++)
    {
      if (!hash_table[i].empty())
      {
        return iterator(*this, i, hash_table[i].begin());
      }
    }

    return iterator(*this, (size_type)(hash_table_size - 1), hash_table[hash_table_size - 1].end());
  }

  iterator end()
  {
    return iterator(*this, (size_type)(hash_table_size - 1), hash_table[hash_table_size - 1].end());
  }

  const_iterator cbegin() const
  {
    for (size_type i = 0; i < hash_table_size; i++)
    {
      if (!hash_table[i].empty())
      {
        return const_iterator(*this, i, hash_table[i].begin());
      }
    }

    return const_iterator(*this, (size_type)(hash_table_size - 1), hash_table[hash_table_size - 1].end());
  }

  const_iterator cend() const
  {
    return const_iterator(*this, (size_type)(hash_table_size - 1), hash_table[hash_table_size - 1].end());
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

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type *;
  using list_it_type = typename std::list<value_type>::const_iterator;
  using size_type = typename HashMap::size_type;
  friend void HashMap::remove(const const_iterator &it);

protected:
  const HashMap &map;
  size_type index;
  list_it_type it;

public:
  explicit ConstIterator()
  {
  }

  ConstIterator(const HashMap<KeyType, ValueType> &map, size_type index, list_it_type it)
      : map(map), index(index), it(it)
  {
  }

  ConstIterator(const ConstIterator &other)
      : ConstIterator(other.map, other.index, other.it)
  {
  }

  ConstIterator &operator++()
  {
    if (*this == map.end())
    {
      throw std::out_of_range("cannot increment end iterator");
    }

    if ((++it) == map.hash_table[index].end())
    {
      size_type i;
      for (i = index + 1; i < map.hash_table_size; i++)
      {
        if (!map.hash_table[i].empty())
        {
          break;
        }
      }

      if (i == map.hash_table_size)
      {
        i--;
      }
      index = i;
      it = map.hash_table[i].begin();
    }

    return *this;
  }

  ConstIterator operator++(int)
  {
    auto it = *this;
    operator++();
    return it;
  }

  ConstIterator &operator--()
  {
    if (*this == map.begin())
    {
      throw std::out_of_range("cannot decrement begin iterator");
    }

    if ((--it) == map.hash_table[index].begin())
    {
      size_type i;
      for (i = index; i >= 0; i--)
      {
        if (!map.hash_table[i].empty())
        {
          break;
        }
      }

      it = map.hash_table[i].end();
      if (!map.hash_table[i].empty())
      {
        it--;
      }
      index = i;
    }

    return *this;
  }

  ConstIterator operator--(int)
  {
    auto it = *this;
    operator--();
    return it;
  }

  reference operator*() const
  {
    if (*this == map.end())
    {
      throw std::out_of_range("cannot dereference end iterator");
    }
    return (*it);
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator &other) const
  {
    bool c1 = &map == &other.map;
    bool c2 = index == other.index;
    bool c3 = it == other.it;

    return c1 && c2 && c3;
  }

  bool operator!=(const ConstIterator &other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type *;
  using list_it_type = typename std::list<value_type>::iterator;
  using size_type = typename HashMap::size_type;

  explicit Iterator()
  {
  }

  Iterator(const HashMap &map, size_type index, list_it_type it)
      : ConstIterator(map, index, it)
  {
  }

  Iterator(const ConstIterator &other)
      : ConstIterator(other)
  {
  }

  Iterator &operator++()
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

  Iterator &operator--()
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

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

} // namespace aisdi

#endif /* AISDI_MAPS_HASHMAP_H */
