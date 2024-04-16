#ifndef BPTREE_BNODE_H
#define BPTREE_BNODE_H
#pragma once

#include <iostream>
#include "error.h"
#include "utility.h"

namespace venillalemon {
    template<class K, class V, size_t degree>
    class BNode {
    public:

      typedef pair<K, V> p;

      size_t _size = 0;
      size_t _par = 0;// 1-based
      size_t _chil[degree] = {0};
      // in tree, point to the next node, 1-based
      // in file, point to the position of the data node
      p _key[degree];
      bool is_leaf = false;

      void insert_pair(const K &k, const V &v, size_t val) {
        size_t l = 0, r = _size;
        auto tmp_pair = p(k, v);
        if (tmp_pair < _key[0]) {
          l = r = 0;
        } else {
          while (l < r - 1) {
            size_t mid = (l + r) / 2;
            if (tmp_pair < _key[mid]) r = mid;
            else l = mid;
          }
        }
        if (_key[l] == tmp_pair) {
          error("Duplicated key and value");
        }
        for (size_t i = _size; i > r; --i) {
          _key[i] = _key[i - 1];
          _chil[i] = _chil[i - 1];
        }
        _key[r] = tmp_pair;
        _chil[r] = val;
        ++_size;
      }

      size_t lower_bound(const p &k) {
        size_t l = 0, r = _size;
        while (l < r) {
          size_t mid = (l + r) / 2;
          if (_key[mid] < k) l = mid + 1;
          else r = mid;
        }
        return l;
      }

      size_t upper_bound(const p &k) {
        size_t l = 0, r = _size;
        while (l < r) {
          size_t mid = (l + r) / 2;
          if (k < _key[mid]) r = mid;
          else l = mid + 1;
        }
        return l;
      }

      void remove_pair(const K &k, const V &v) {
        size_t l = 0, r = _size;
        auto tmp_pair = p(k, v);
        if (tmp_pair == _key[_size - 1]) {
          l = r = _size - 1;
        } else {
          while (l < r - 1) {
            size_t mid = (l + r) / 2;
            if (tmp_pair < _key[mid]) r = mid;
            else l = mid;
          }
          if (_key[l] != tmp_pair) {
            error("Key-value pair not found");
          }
        }
        for (size_t i = l; i < _size - 1; ++i) {
          _key[i] = _key[i + 1];
          _chil[i] = _chil[i + 1];
        }
        --_size;
      }

      void modify_pair(const p &k, const p &new_pair) {
        size_t l = 0, r = _size;
        while (l < r) {
          size_t mid = (l + r) / 2;
          if (_key[mid] < k) l = mid + 1;
          else r = mid;
        }
        if (l == _size || _key[l] != k) {
          error("Key not found");
        }
        _key[l] = new_pair;
      }
    };


}


#endif //BPTREE_BNODE_H
