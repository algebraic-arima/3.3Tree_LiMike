#ifndef BPTREE_MAP_H
#define BPTREE_MAP_H

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.h"
#include "error.h"
#include <iostream>

namespace arima_kana {

    template<class Key, class T, class Compare = std::less<Key>>
    class map {
    public:
      /**
       * the internal type of data.
       * it should have a default constructor, a copy constructor.
       * You can use arima_kana::map as value_type by typedef.
       */
      typedef arima_kana::pair<Key, T> value_type;
      typedef Key key_type;
      typedef T mapped_type;
      // RE if const Key

      struct Node {
        value_type *_val;
        Node *ls, *rs;
        Node *fa;
        int level;
      };

      Node *AAtree;
      size_t _size;

      /**
       * see BidirectionalIterator at CppReference for help.
       *
       * if there is anything wrong throw invalid_iterator.
       *     like it = map.begin(); --it;
       *       or it = map.end(); ++end();
       */
      class const_iterator;

      class iterator {
      public:
        map *root;
        Node *pos;

        friend class const_iterator;

      public:
        iterator() {
          root = nullptr;
          pos = nullptr;
        }

        iterator(const iterator &other) {
          root = other.root;
          pos = other.pos;
        }

        iterator(map *root, Node *pos) : root(root), pos(pos) {}

        iterator operator++(int) {
          if (pos == nullptr) error("invalid_iterator");
          Node *ptr = pos;
          if (pos->rs != nullptr) {
            pos = pos->rs;
            while (pos->ls != nullptr) {
              pos = pos->ls;
            }
          } else {
            if (pos->fa == nullptr) {
              pos = nullptr;
              return iterator(root, ptr);
            }
            while (pos == pos->fa->rs) {
              pos = pos->fa;
              if (pos->fa == nullptr) {
                pos = nullptr;
                return iterator(root, ptr);
              }
            }
            pos = pos->fa;
          }
          return iterator(root, ptr);
        }

        iterator &operator++() {
          if (pos == nullptr) error("invalid_iterator");
          if (pos->rs != nullptr) {
            pos = pos->rs;
            while (pos->ls != nullptr) {
              pos = pos->ls;
            }
          } else {
            if (pos->fa == nullptr) {
              pos = nullptr;
              return *this;
            }
            while (pos == pos->fa->rs) {
              pos = pos->fa;
              if (pos->fa == nullptr) {
                pos = nullptr;
                return *this;
              }
            }
            pos = pos->fa;
          }
          return *this;
        }

        /**
         * TODO iter--
         */
        iterator operator--(int) {
          if (root == nullptr || root->AAtree == nullptr)
            error("invalid_iterator");
          Node *ptr = pos;
          if (pos == nullptr) {
            pos = root->AAtree;
            while (pos->rs != nullptr) {
              pos = pos->rs;
            }
          } else if (pos->ls != nullptr) {
            pos = pos->ls;
            while (pos->rs != nullptr) {
              pos = pos->rs;
            }
          } else {
            while (pos == pos->fa->ls) {
              pos = pos->fa;
              if (pos->fa == nullptr) {
                pos = nullptr;
                return iterator(root, ptr);
              }
            }
            pos = pos->fa;
          }
          return iterator(root, ptr);
        }

        /**
         * TODO --iter
         */
        iterator &operator--() {
          if (root == nullptr || root->AAtree == nullptr)
            error("invalid_iterator");
          if (pos == nullptr) {
            pos = root->AAtree;
            while (pos->rs != nullptr) {
              pos = pos->rs;
            }
          } else if (pos->ls != nullptr) {
            pos = pos->ls;
            while (pos->rs != nullptr) {
              pos = pos->rs;
            }
          } else {
            while (pos == pos->fa->ls) {
              pos = pos->fa;
              if (pos->fa == nullptr) {
                pos = nullptr;
                return *this;
              }
            }
            pos = pos->fa;
          }
          return *this;
        }

        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        value_type &operator*() const {
          return *(pos->_val);
        }

        bool operator==(const iterator &rhs) const {
          return pos == rhs.pos && root == rhs.root;
        }

        bool operator==(const const_iterator &rhs) const {
          return pos == rhs.pos && root == rhs.root;
        }

        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
          return pos != rhs.pos || root != rhs.root;
        }

        bool operator!=(const const_iterator &rhs) const {
          return pos != rhs.pos || root != rhs.root;
        }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        value_type *operator->() const noexcept {
          return pos->_val;
        }
      };

      class const_iterator {
        // it should have similar member method as iterator.
        //  and it should be able to construct from an iterator.
      public:
        const map *root;
        Node *pos;

      public:
        const_iterator() {
          root = nullptr;
          pos = nullptr;
        }

        const_iterator(const const_iterator &other) {
          root = other.root;
          pos = other.pos;
        }

        const_iterator(const iterator &other) {
          root = other.root;
          pos = other.pos;
        }

        const_iterator(const map *root, Node *pos) : root(root), pos(pos) {}

        const_iterator operator++(int) {
          if (pos == nullptr) error("invalid_iterator");
          Node *ptr = pos;
          if (pos->rs != nullptr) {
            pos = pos->rs;
            while (pos->ls != nullptr) {
              pos = pos->ls;
            }
          } else {
            if (pos->fa == nullptr) {
              pos = nullptr;
              return const_iterator(root, ptr);
            }
            while (pos == pos->fa->rs) {
              pos = pos->fa;
              if (pos->fa == nullptr) {
                pos = nullptr;
                return const_iterator(root, ptr);
              }
            }
            pos = pos->fa;
          }
          return const_iterator(root, ptr);
        }

        const_iterator &operator++() {
          if (pos == nullptr) error("invalid_iterator");
          if (pos->rs != nullptr) {
            pos = pos->rs;
            while (pos->ls != nullptr) {
              pos = pos->ls;
            }
          } else {
            if (pos->fa == nullptr) {
              pos = nullptr;
              return *this;
            }
            while (pos == pos->fa->rs) {
              pos = pos->fa;
              if (pos->fa == nullptr) {
                pos = nullptr;
                return *this;
              }
            }
            pos = pos->fa;
          }
          return *this;
        }

        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
          if (root == nullptr || root->AAtree == nullptr)
            error("invalid_iterator");
          Node *ptr = pos;
          if (pos == nullptr) {
            pos = root->AAtree;
            while (pos->rs != nullptr) {
              pos = pos->rs;
            }
          } else if (pos->ls != nullptr) {
            pos = pos->ls;
            while (pos->rs != nullptr) {
              pos = pos->rs;
            }
          } else {
            while (pos == pos->fa->ls) {
              pos = pos->fa;
              if (pos->fa == nullptr) {
                pos = nullptr;
                return const_iterator(root, ptr);
              }
            }
            pos = pos->fa;
          }
          return const_iterator(root, ptr);
        }

        /**
         * TODO --iter
         */
        const_iterator &operator--() {
          if (root == nullptr || root->AAtree == nullptr)
            error("invalid_iterator");
          if (pos == nullptr) {
            pos = root->AAtree;
            while (pos->rs != nullptr) {
              pos = pos->rs;
            }
          } else if (pos->ls != nullptr) {
            pos = pos->ls;
            while (pos->rs != nullptr) {
              pos = pos->rs;
            }
          } else {
            while (pos == pos->fa->ls) {
              pos = pos->fa;
              if (pos->fa == nullptr) {
                pos = nullptr;
                return *this;
              }
            }
            pos = pos->fa;
          }
          return *this;
        }

        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        value_type &operator*() const {
          return *(pos->_val);
        }

        bool operator==(const iterator &rhs) const {
          return pos == rhs.pos && root == rhs.root;
        }

        bool operator==(const const_iterator &rhs) const {
          return pos == rhs.pos && root == rhs.root;
        }

        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
          return pos != rhs.pos || root != rhs.root;
        }

        bool operator!=(const const_iterator &rhs) const {
          return pos != rhs.pos || root != rhs.root;
        }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        value_type *operator->() const noexcept {
          return pos->_val;
        }
      };

    private:
      inline void split(Node *&node) {
        if (node == nullptr) return;
        if (node->rs != nullptr && node->rs->rs != nullptr && node->rs->rs->level == node->level) {
          Node *tmp = node->rs;
          Node *father = node->fa;
          node->rs = tmp->ls;
          tmp->ls = node;


          tmp->fa = father;

          node->fa = tmp;
          if (node->rs != nullptr) node->rs->fa = node;

          node = tmp;
          ++node->level;
        }
      }

      inline void skew(Node *&node) {
        if (node == nullptr) return;
        if (node->ls != nullptr && node->ls->level == node->level) {
          Node *tmp = node->ls;
          Node *father = node->fa;
          node->ls = tmp->rs;
          tmp->rs = node;

          tmp->fa = father;

          node->fa = tmp;
          if (node->ls != nullptr) node->ls->fa = node;

          node = tmp;
        }
      }


    public:
      map() {
        AAtree = nullptr;
        _size = 0;
      }

      map(const map &other) {
        AAtree = copy_ptr(other.AAtree, nullptr);
        _size = other._size;
      }

      Node *copy_ptr(Node *ptr, Node *fa) {
        if (ptr == nullptr) return nullptr;
        Node *n = new Node{new value_type(*(ptr->_val)), nullptr, nullptr, fa,
                           ptr->level};
        n->ls = copy_ptr(ptr->ls, n);
        n->rs = copy_ptr(ptr->rs, n);// do not eat a fat man with a mouth
        return n;
      }

      map &operator=(const map &other) {
        if (&other == this) return *this;
        clear();
        AAtree = copy_ptr(other.AAtree, nullptr);
        _size = other._size;
        return *this;
      }

      ~map() {
        clear();
      }

      /**
       * access specified element with bounds checking
       * Returns a reference to the mapped value of the element with key equivalent to key.
       * If no such element exists, an exception of type `index_out_of_bound'
       */
      T &at(const Key &key) {
        Node *ptr = find_ptr(key);
        if (ptr == nullptr) error("index_out_of_bound");
        return ptr->_val->second;
      }

      const T &at(const Key &key) const {
        Node *ptr = find_ptr(key);
        if (ptr == nullptr) error("index_out_of_bound");
        return ptr->_val->second;
      }

      /**
       * TODO
       * access specified element
       * Returns a reference to the value that is mapped to a key equivalent to key,
       *   performing an insertion if such key does not already exist.
       */
      T &operator[](const Key &key) {
        Node *ptr = find_ptr(key);
        if (ptr == nullptr) {
          insert(AAtree, key, T());
          ptr = find_ptr(key);
        }
        return ptr->_val->second;
      }

      /**
       * behave like at() throw index_out_of_bound if such key does not exist.
       */
      const T &operator[](const Key &key) const {
        Node *ptr = find_ptr(key);
        if (ptr == nullptr) error("index_out_of_bound");
        return ptr->_val->second;
      }

      /**
       * return a iterator to the beginning
       */
      iterator begin() {
        Node *ptr = AAtree;
        if (ptr == nullptr) return iterator(this, nullptr);
        while (ptr->ls != nullptr) {
          ptr = ptr->ls;
        }
        return iterator(this, ptr);
      }

      const_iterator cbegin() const {
        Node *ptr = AAtree;
        if (ptr == nullptr) return const_iterator(this, nullptr);
        while (ptr->ls != nullptr) {
          ptr = ptr->ls;
        }
        return const_iterator(this, ptr);
      }

      /**
       * return a iterator to the end
       * in fact, it returns past-the-end.
       */
      iterator end() {
        return iterator(this, nullptr);
      }

      const_iterator cend() const {
        return const_iterator(this, nullptr);
      }

      /**
       * checks whether the container is empty
       * return true if empty, otherwise false.
       */
      bool empty() const {
        return AAtree == nullptr;
      }

      /**
       * returns the number of elements.
       */
      size_t size() const {
        return _size;
      }

      /**
       * clears the contents
       */
      void clear() {
        clear(AAtree);
        delete AAtree;
        AAtree = nullptr;
        _size = 0;
      }

      void clear(Node *ptr) {
        if (ptr == nullptr) return;
        clear(ptr->ls), clear(ptr->rs);
        delete ptr->ls;
        delete ptr->_val;
        delete ptr->rs;
        ptr->ls = ptr->rs = nullptr;
        ptr->_val = nullptr;
      }

      /**
       * insert an element.
       * return a pair, the first of the pair is
       *   the iterator to the new element (or the element that prevented the insertion),
       *   the second one is true if insert successfully, or false.
       */
      pair<iterator, bool> insert(const value_type &value) {
//        Node *ptr = find_ptr(value.first);
//        if (ptr != nullptr) {
//          return pair<iterator, bool>(iterator(AAtree, ptr), false);
//        }
        bool inserted = insert(AAtree, value.first, value.second);
        return pair<iterator, bool>(find(value.first), inserted);
      }

      bool insert(Node *&cur, const Key &key, const T &value) {
        bool inserted = false;
        if (cur == nullptr) {
          cur = new Node{new value_type(key, value), nullptr, nullptr, nullptr, 1};
          ++_size;
          inserted = true;
        } else if (Compare()(key, cur->_val->first)) {
          if (cur->ls == nullptr) {
            cur->ls = new Node{new value_type(key, value), nullptr, nullptr, cur, 1};
            ++_size;
            inserted = true;
          } else inserted = insert(cur->ls, key, value);
        } else if (Compare()(cur->_val->first, key)) {
          if (cur->rs == nullptr) {
            cur->rs = new Node{new value_type(key, value), nullptr, nullptr, cur, 1};
            ++_size;
            inserted = true;
          } else inserted = insert(cur->rs, key, value);
        } else {
          return false;
        }
        skew(cur);
        split(cur);
        return inserted;
      }

      /**
       * erase the element at pos.
       *
       * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
       */
      void erase(iterator pos) {
        if (pos.pos == nullptr || pos.root == nullptr || pos.root->AAtree != AAtree)
          error("invalid_iterator");
        _erase(AAtree, pos.pos->_val->first);
      }

      bool _erase(Node *&cur, const Key &key) {
        if (cur == nullptr) return false;
        if (Compare()(key, cur->_val->first)) {
          bool erased = _erase(cur->ls, key);
          if (erased) _adjust(cur);
          return erased;
        } else if (Compare()(cur->_val->first, key)) {
          bool erased = _erase(cur->rs, key);
          if (erased) _adjust(cur);
          return erased;
        }// compare

        // equal
        if (cur->ls == nullptr || cur->rs == nullptr) {
          Node *tmp = cur;
          cur = (cur->ls == nullptr ? cur->rs : cur->ls);
          if (cur != nullptr) cur->fa = tmp->fa;
          tmp->fa = tmp->ls = tmp->rs = nullptr;
          delete tmp->_val;
          delete tmp;
          --_size;
        } else {
          Node *cur_pos = cur;
          Node *tmp = cur->rs;
          while (tmp->ls != nullptr) {
            tmp = tmp->ls;
          }
          Node *copy = new Node{new value_type(*(tmp->_val)), tmp->ls, tmp->rs, tmp->fa, tmp->level};
          if (tmp->fa != nullptr)
            tmp->fa->ls == tmp ? tmp->fa->ls = copy : tmp->fa->rs = copy;
          if (tmp->ls != nullptr)
            tmp->ls->fa = copy;
          if (tmp->rs != nullptr)
            tmp->rs->fa = copy;
          tmp->ls = cur_pos->ls;
          tmp->rs = cur_pos->rs;
          tmp->fa = cur_pos->fa;
          tmp->level = cur_pos->level;
          if (cur_pos->fa != nullptr)
            cur_pos->fa->ls == cur_pos ? cur_pos->fa->ls = tmp : cur_pos->fa->rs = tmp;
          if (cur_pos->ls != nullptr)
            cur_pos->ls->fa = tmp;
          if (cur_pos->rs != nullptr)
            cur_pos->rs->fa = tmp;
          cur_pos->fa = cur_pos->ls = cur_pos->rs = nullptr;
          delete cur_pos->_val;
          delete cur_pos;
          cur = tmp;
          _erase(cur->rs, cur->_val->first);
          _adjust(cur);
        }
        return true;
      }


    private:
      void _adjust(Node *&cur) {
        Node *l_son = cur->ls, *r_son = cur->rs;
        int l_lev = l_son == nullptr ? 0 : l_son->level;
        int r_lev = r_son == nullptr ? 0 : r_son->level;
        if (cur->level > l_lev + 1 || cur->level > r_lev + 1) {
          --cur->level;
          if (r_son != nullptr && r_son->level > cur->level)
            r_son->level = cur->level;
          skew(cur);
          skew(cur->rs);
          if (cur->rs != nullptr) skew(cur->rs->rs);
          split(cur);
          split(cur->rs);
        }
      }

      Node *find_ptr(const Key &key) const {
        Node *cur = AAtree;
        while (cur != nullptr) {
          if (Compare()(key, cur->_val->first)) {
            cur = cur->ls;
          } else if (Compare()(cur->_val->first, key)) {
            cur = cur->rs;
          } else {
            return cur;
          }
        }
        return nullptr;
      }


    public:

      iterator find(const Key &key) {
        return iterator(this, find_ptr(key));
      }

      const_iterator find(const Key &key) const {
        return const_iterator(this, find_ptr(key));
      }
    };

}

#endif