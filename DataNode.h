#ifndef BPTREE_DATANODE_H
#define BPTREE_DATANODE_H
#pragma once

#include <iostream>
#include "utility.h"
#include "error.h"

namespace arima_kana {
    template<class K, class V, size_t block>
    class DataNode {
    public:

      typedef pair<K, V> p;

      size_t size = 0;
      p _data[block];

      DataNode() = default;

      explicit DataNode(const p &kv) : size(1) {
        _data[0] = kv;
      }

      void insert_pair(K key, V val) {
        int l = 0, r = size;
        auto tmp_pair = p({key, val});
        while (l < r) {
          int mid = (l + r) / 2;
          if (_data[mid] < tmp_pair) l = mid + 1;
          else r = mid;
        }
        if (_data[l] == tmp_pair) {
          error("Duplicated key and value");
        }
        for (size_t i = size; i > r; --i) {
          _data[i] = _data[i - 1];
        }
        _data[r] = tmp_pair;
        ++size;
      }

      void remove_pair(K key, V val) {
        int l = 0, r = size;
        auto tmp_pair = p({key, val});
        while (l < r) {
          int mid = (l + r) / 2;
          if (_data[mid] < tmp_pair) l = mid + 1;
          else r = mid;
        }
        if (_data[l] != tmp_pair) {
          error("Key-value pair not found");
        }
        for (size_t i = l; i < size - 1; ++i) {
          _data[i] = _data[i + 1];
        }
        --size;
      }

      V find_pair(K key) {
        static size_t pos = -1;
        if (pos != -1) {
          ++pos;
          if (_data[pos].key == key) return _data[pos].value;
          else return {};
        }
        size_t l = 0, r = size;
        while (l < r) {
          size_t mid = (l + r) / 2;
          if (_data[mid].key < key) l = mid;
          else r = mid;
        }
        if (_data[r].key == key) {
          pos = r;
          return _data[r].value;
        } else {
          pos = -1;
          return {};
        }
      }

      void print() {
        std::cout << "___" << '\n';
        for (int i = 0; i < size; ++i) {
          std::cout << "   " << _data[i].first << "   " << _data[i].second << '\n';
        }
      }

      bool operator==(const DataNode &other) const {
        if (size != other.size) return false;
        for (int i = 0; i < size; ++i) {
          if (_data[i] != other._data[i]) return false;
        }
        return true;
      }

    };

}

#endif //BPTREE_DATANODE_H
