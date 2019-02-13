#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <functional>
#include <iomanip>
#include <iostream>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
{
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using node_color = enum {
    RED,
    BLACK,
  };
  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;
  friend class ConstIterator;
  friend class Iterator;

private:
  struct Node
  {
    value_type value;
    Node *parent;
    Node *left;
    Node *right;
    node_color color;

    Node()
        : parent(nullptr),
          left(nullptr),
          right(nullptr),
          color(BLACK)
    {
    }

    Node(value_type value)
        : value(value),
          color(RED)
    {
    }

    Node(value_type value, Node *parent, Node *left, Node *right, node_color color)
        : value(value),
          parent(parent),
          left(left),
          right(right),
          color(color)
    {
    }

    ~Node()
    {
    }
  };

  bool is_left_child(Node *node) const
  {
    return node == node->parent->left;
  }

  bool is_right_child(Node *node) const
  {
    return node == node->parent->right;
  }

  bool has_left_child(Node *node) const
  {
    return node->left != sentinel;
  }

  bool has_right_child(Node *node) const
  {
    return node->right != sentinel;
  }

  bool is_black(Node *node) const
  {
    return node->color == BLACK;
  }

  bool is_red(Node *node) const
  {
    return node->color == RED;
  }

  bool should_be_new_root(Node *node) const
  {
    return node->parent == sentinel;
  }

  Node *sentinel;
  Node *root;
  size_type size;

  void insert(Node *node)
  {
    insert_to_tree(node);
    set_color(node);
    size++;
  }

  void insert_to_tree(Node *node)
  {
    if (node->parent == sentinel)
    {
      root = node;
    }
    else
    {
      // looking for appropriate leaf
      while (true)
      {
        // inserting on the left
        if (node->value.first < node->parent->value.first)
        {
          // leaf found
          if (!(has_left_child(node->parent)))
          {
            node->parent->left = node;
            break;
          }
          // going down the left branch
          node->parent = node->parent->left;
        }
        // inserting on the right
        else
        {
          // leaf found
          if (!(has_right_child(node->parent)))
          {
            node->parent->right = node;
            break;
          }
          // going down the right branch
          node->parent = node->parent->right;
        }
      }
    }
  }

  void set_color(Node *node)
  {
    node->color = RED;
    // if node is root or parent is black, everything is correct
    while (node != root && !is_black(node->parent))
    {
      // parent is left child
      if (is_left_child(node->parent))
      {
        Node *uncle = node->parent->parent->right;

        if (uncle->color == RED)
        {
          node->parent->color = BLACK;
          uncle->color = BLACK;
          node->parent->parent->color = RED;
          node = node->parent->parent;
          continue;
        }

        if (is_right_child(node))
        {
          node = node->parent;
          rotate_left(node);
        }
        // node is left child or parent of node (now node) is left child
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        rotate_right(node->parent->parent);
        break;
      }
      // parent is right child
      else
      {
        Node *uncle = node->parent->parent->left;

        if (uncle->color == RED)
        {
          node->parent->color = BLACK;
          uncle->color = BLACK;
          node->parent->parent->color = RED;
          node = node->parent->parent;
          continue;
        }

        // node is left child
        if (is_left_child(node))
        {
          node = node->parent;
          rotate_right(node);
        }

        // node is right child or parent of node (now node) is right child
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        rotate_left(node->parent->parent);
        break;
      }
    }
    // always black
    root->color = BLACK;
  }

  // void set_color_when_uncle_is_red(Node *node, Node *uncle)
  // {
  //   node->parent->color = BLACK;
  //   uncle->color = BLACK;
  //   node->parent->parent->color = RED;
  //   node = node->parent->parent;
  // }

  void rotate_left(Node *node)
  {
    Node *right = node->right;
    Node *parent;
    if (right != sentinel)
    {
      parent = node->parent;
      node->right = right->left;

      if (node->right != sentinel)
      {
        node->right->parent = node;
      }

      right->left = node;
      right->parent = parent;
      node->parent = right;

      if (parent != sentinel)
      {
        if (parent->left == node)
        {
          parent->left = right;
        }
        else
        {
          parent->right = right;
        }
      }
      else
      {
        root = right;
      }
    }
  }

  void rotate_right(Node *node)
  {
    Node *left = node->left;
    Node *parent;
    if (left != sentinel)
    {
      parent = node->parent;
      node->left = left->right;

      if (node->left != sentinel)
      {
        node->left->parent = node;
      }

      left->right = node;
      left->parent = parent;
      node->parent = left;

      if (parent != sentinel)
      {
        if (parent->left == node)
        {
          parent->left = left;
        }
        else
        {
          parent->right = left;
        }
      }
      else
      {
        root = left;
      }
    }
  }

  void remove_node(Node *node)
  {
    if (!(has_left_child(node)) || !(has_right_child(node)))
    {
      remove_node_with_one_or_zero_child(node);
    }
    else
    {
      remove_node_with_two_children(node);
    }
    size--;
  }

  void remove_node_with_one_or_zero_child(Node *node)
  {
    Node *child;
    if (!(has_left_child(node)))
    {
      child = node->right;
    }
    else
    {
      child = node->left;
    }

    // parent of node becomes new parent of child
    child->parent = node->parent;

    if (should_be_new_root(node))
    {
      root = child;
    }
    // if node is left child, child should be also left
    else if (is_left_child(node))
    {
      child->parent->left = child;
    }
    // otherwise child should be right child
    else
    {
      child->parent->right = child;
    }

    // fixing rbtree
    if (is_black(node))
    {
      child->color = BLACK;
    }

    delete node;
  }

  void remove_node_with_two_children(Node *node)
  {
    Node *successor = find_successor(node);
    Node *successor_child;

    if (has_left_child(successor))
    {
      successor_child = successor->left;
    }
    else
    {
      successor_child = successor->right;
    }

    successor_child->parent = successor->parent;

    if (should_be_new_root(successor))
    {
      root = successor_child;
    }
    else if (is_left_child(successor))
    {
      successor->parent->left = successor_child;
    }
    else
    {
      successor->parent->right = successor_child;
    }

    node->value.first = successor->value.first;
    node->value.second = successor->value.second;

    if (is_black(successor))
    {
      Node *child = successor_child;
      while (child != root && is_black(child))
      {
        if (is_left_child(child))
        {
          Node *brother = child->parent->right;
          if (is_red(brother))
          {
            brother->color = BLACK;
            child->parent->color = RED;
            rotate_left(child->parent);
            brother = child->parent->right;
          }

          if (is_black(brother->left) &&
              is_black(brother->right))
          {
            brother->color = RED;
            child = child->parent;
            continue;
          }

          if (is_black(brother->right))
          {
            brother->left->color = BLACK;
            brother->color = RED; //!!!!!
            rotate_right(brother);
            brother = child->parent->right;
          }

          brother->color = child->parent->color;
          child->parent->color = BLACK;
          brother->right->color = BLACK;
          rotate_left(child->parent);
          child = root;
          break;
        }
        else
        {
          Node *brother = child->parent->left;

          if (is_red(brother))
          {
            brother->color = BLACK;
            child->parent->color = RED;
            rotate_right(child->parent);
            brother = child->parent->left;
          }

          if (is_black(brother->left) &&
              is_black(brother->right))
          {
            brother->color = RED;
            child = child->parent;
            continue;
          }

          if (is_black(brother->left))
          {
            brother->right->color = BLACK;
            brother->color = RED;
            rotate_left(brother);
            brother = child->parent->left;
          }

          brother->color = child->parent->color;
          child->parent->color = BLACK;
          brother->left->color = BLACK;
          rotate_right(child->parent);
          break;
        }
      }
    }

    successor_child->color = BLACK;

    delete successor;
  }

  Node *find_successor(Node *node) const
  {
    Node *successor;
    if (node != sentinel)
    {
      if (has_right_child(node))
      {
        node = node->right;
        while (has_left_child(node))
        {
          node = node->left;
        }
        return node;
      }
      else
      {
        successor = node->parent;

        while (successor != sentinel && node == successor->right)
        {
          node = successor;
          successor = successor->parent;
        }

        return successor;
      }
    }

    return sentinel;
  }

  Node *find_predecessor(Node *node) const
  {
    Node *predecessor;
    if (node != sentinel)
    {
      if (has_left_child(node))
      {
        node = node->left;
        while (has_left_child(node))
        {
          node = node->right;
        }
        return node;
      }
      else
      {
        predecessor = node->parent;

        while (predecessor != sentinel && node == predecessor->left)
        {
          node = predecessor;
          predecessor = predecessor->parent;
        }

        return predecessor;
      }
    }

    return sentinel;
  }

  Node *find_node(key_type key) const
  {
    Node *node = root;

    while (node != sentinel)
    {
      if (key > node->value.first)
      {
        node = node->right;
      }
      else if (key < node->value.first)
      {
        node = node->left;
      }
      else
      {
        break;
      }
    }

    return node;
  }

  Node *find_min_node(Node *node) const
  {
    if (node != sentinel)
    {
      while (node->left != sentinel)
      {
        node = node->left;
      }
    }

    return node;
  }

  Node *find_max_node(Node *node) const
  {
    if (node != sentinel)
    {
      while (node->right != sentinel)
      {
        node = node->right;
      }
    }

    return node;
  }

  void erase()
  {
    DFS_erase(root);
    sentinel->color = BLACK;
    sentinel->right = sentinel;
    sentinel->left = sentinel;
    sentinel->right = sentinel;
    root = sentinel;
    size = 0;
  }

  void DFS_erase(Node *node)
  {
    if (node != sentinel)
    {
      DFS_erase(node->left);
      DFS_erase(node->right);
      delete node;
    }
  }

public:
  TreeMap()
  {
    sentinel = new Node();
    sentinel->color = BLACK;
    sentinel->right = sentinel;
    sentinel->left = sentinel;
    sentinel->right = sentinel;
    root = sentinel;
    size = 0;
  }

  TreeMap(std::initializer_list<value_type> list) : TreeMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      insert(new Node(*it, root, sentinel, sentinel, RED));
    }
  }

  TreeMap(const TreeMap &other) : TreeMap()
  {
    for (auto it = other.begin(); it != other.end(); ++it)
    {
      insert(new Node(*it, root, sentinel, sentinel, RED));
    }
  }

  TreeMap(TreeMap &&other)
  {
    root = std::move(other.root);
    sentinel = std::move(other.sentinel);
    size = std::move(other.size);

    other.root = nullptr;
    other.sentinel = nullptr;
    other.size = 0;
  }

  ~TreeMap()
  {
    DFS_erase(root);
    delete sentinel;
  }

  TreeMap &operator=(const TreeMap &other)
  {
    if (this != &other)
    {
      erase();

      for (auto it = other.begin(); it != other.end(); ++it)
      {
        insert(new Node(*it, root, sentinel, sentinel, RED));
      }
    }

    return *this;
  }

  TreeMap &operator=(TreeMap &&other)
  {
    if (this != &other)
    {
      erase();
      delete sentinel;

      root = other.root;
      size = other.size;
      sentinel = other.sentinel;

      other.root = nullptr;
      other.size = 0;
      other.sentinel = nullptr;
    }

    return *this;
  }

  bool isEmpty() const { return size == 0; }

  mapped_type &operator[](const key_type &key)
  {
    Node *node = find_node(key);

    if (node == sentinel)
    {
      node = new Node(std::make_pair(key, mapped_type()), root, sentinel, sentinel, RED);
      insert(node);
    }

    return node->value.second;
  }

  const mapped_type &valueOf(const key_type &key) const
  {
    Node *node = find_node(key);

    if (node == sentinel)
    {
      throw std::out_of_range("cannot get value of nonexistent node");
    }

    return node->value.second;
  }

  mapped_type &valueOf(const key_type &key)
  {
    Node *node = find_node(key);

    if (node == sentinel)
    {
      throw std::out_of_range("cannot get value of nonexistent node");
    }

    return node->value.second;
  }

  const_iterator find(const key_type &key) const
  {
    Node *node = find_node(key);
    return const_iterator(*this, node);
  }

  iterator find(const key_type &key)
  {
    Node *node = find_node(key);
    return iterator(*this, node);
  }

  void remove(const key_type &key)
  {
    Node *node = find_node(key);

    if (node == sentinel)
    {
      throw std::out_of_range("cannot get remove nonexistent node");
    }

    remove_node(node);
  }

  void remove(const const_iterator &it)
  {
    Node *node = find_node((*it).first);

    if (node == sentinel)
    {
      throw std::out_of_range("cannot get remove nonexistent node");
    }

    remove_node(node);
  }

  size_type getSize() const { return size; }

  bool operator==(const TreeMap &other) const
  {
    if (size != other.size)
    {
      return false;
    }

    for (auto it = begin(), it_other = other.begin(); it != end() && it_other != end(); ++it, ++it_other)
    {
      if (*it != *it_other)
      {
        return false;
      }
    }

    return true;
  }

  bool operator!=(const TreeMap &other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    Node *node = find_min_node(root);
    return iterator(*this, node);
  }

  iterator end()
  {
    return iterator(*this, sentinel);
  }

  const_iterator cbegin() const
  {
    Node *node = find_min_node(root);
    return const_iterator(*this, node);
  }

  const_iterator cend() const
  {
    return const_iterator(*this, sentinel);
  }

  const_iterator begin() const { return cbegin(); }

  const_iterator end() const { return cend(); }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
protected:
  const TreeMap &map;
  Node *node;

public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type *;

  explicit ConstIterator(const TreeMap &map, Node *node)
      : map(map),
        node(node)
  {
  }

  ConstIterator(const ConstIterator &other) : ConstIterator(other.map, other.node)
  {
  }

  ConstIterator &operator++()
  {
    if (node == map.sentinel)
    {
      throw std::out_of_range("cannot increment iterator since it is end one");
    }
    node = map.find_successor(node);
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
    if (node == map.sentinel)
    {
      if (map.size == 0)
      {
        throw std::out_of_range("cannot decrement begin iterator");
      }
      else
      {
        node = map.find_max_node(map.root);
      }
    }
    else
    {
      node = map.find_predecessor(node);
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
    if (node == map.sentinel)
    {
      throw std::out_of_range("cannot dereference end iterator");
    }

    return node->value;
  }

  pointer
  operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator &other) const
  {
    return &other.map == &map && other.node == node;
  }

  bool operator!=(const ConstIterator &other) const
  {
    return !(*this == other);
  }
}; // namespace aisdi

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator
    : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type *;
  explicit Iterator(const TreeMap &map, Node *node) : const_iterator(map, node) {}

  Iterator(const ConstIterator &other) : ConstIterator(other) {}

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

  pointer operator->() const { return &this->operator*(); }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

} // namespace aisdi

#endif /* AISDI_MAPS_MAP_H */
