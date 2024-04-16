#ifndef BPTREE_BLOCKRIVER_H
#define BPTREE_BLOCKRIVER_H
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <filesystem>
#include <utility>
#include "error.h"
#include "BPtree.h"

template<class K, class V, size_t block>
class BlockRiver {
public:

  typedef KV<K, V> KV;
  typedef BPTree<K, V, 3> map;

  size_t block_num = 0;
  std::fstream data_filer;
  std::string data_file;
  map list;

  struct DataNode {
    size_t size = 0;
    KV _data[block];

    DataNode() = default;

    explicit DataNode(const KV &kv) : size(1) {
      _data[0] = kv;
    }

    void insert_pair(K key, V val) {
      size_t l = 0, r = size;
      auto tmp_pair = KV({key, val});
      while (l < r - 1) {
        size_t mid = (l + r) / 2;
        if (tmp_pair < _data[mid]) r = mid;
        else l = mid;
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
      size_t l = 0, r = size;
      auto tmp_pair = KV({key, val});
      while (l < r) {
        size_t mid = (l + r) / 2;
        if (tmp_pair < _data[mid]) r = mid;
        else l = mid;
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
      std::cout << "___" << _data[0].key << "   " << _data[0].value << '\n';
      for (int i = 0; i < size; ++i) {
        std::cout << "   " << _data[i].key << "   " << _data[i].value << '\n';
      }
    }

  };

  explicit BlockRiver(const std::string &df) : data_file(df), list(df) {
    data_filer.open(data_file, std::ios::in);
    if (!data_filer.is_open()) {
      data_filer.close();
      init_data();
    } else {
      data_filer.close();
      read_data();
    }
  }

  ~BlockRiver() = default;

  void init_data() {
    data_filer.open(data_file, std::fstream::out | std::fstream::binary);
    block_num = 0;
    data_filer.close();
  }

  void read_data() {
    data_filer.open(data_file, std::fstream::in | std::fstream::binary);
    data_filer.read(reinterpret_cast<char *>(&block_num), sizeof(size_t));
    data_filer.close();
  }

  void append_main(DataNode &t) {
    data_filer.open(data_file, std::ofstream::app);
    data_filer.write(reinterpret_cast<char *>(&t), sizeof(DataNode));
    data_filer.close();
    ++block_num;
  }

  void write_main(DataNode &t, const int pos) {
    if (pos > block_num) return;
    data_filer.open(data_file, std::ofstream::out | std::ifstream::in);
    data_filer.seekp(sizeof(block_num) + (pos - 1) * sizeof(DataNode));
    data_filer.write(reinterpret_cast<char *> (&t), sizeof(DataNode));
    data_filer.close();
  }

  void read_main(DataNode &t, const int pos) {
    if (pos > block_num) return;
    data_filer.open(data_file, std::ifstream::in);
    data_filer.seekg(sizeof(block_num) + (pos - 1) * sizeof(DataNode));
    data_filer.read(reinterpret_cast<char *> (&t), sizeof(DataNode));
    data_filer.close();
  }

  void insert(K &k, V &v) {
    KV kv = {k, v};
    //std::cout<<tv.key<<tv.pos;
    if (list.empty()) {
      //std::cout << "empty\n";
      DataNode tmp(kv);
      append_main(tmp);
      list.insert(k, v, 1);
      return;
    }
    size_t it = list.upper_bound(kv);

    if (it == 0) {
      list.adjust(kv);
      // adjust the minimum to kv
      it = list.upper_bound(kv);
      // it points to a min node
    }
    DataNode tmp;
    read_main(tmp, it);
    tmp.insert_pair(k, v);
    if (tmp.size >= block) {
      DataNode new_node;
      new_node.size = tmp.size - tmp.size / 2;
      for (int i = tmp.size / 2; i < tmp.size; i++) {
        new_node._data[i - tmp.size / 2] = tmp._data[i];
      }
      tmp.size /= 2;
      list.insert(new_node._data[0].key, new_node._data[0].value, block_num + 1);
      //std::cout << new_node.first.key << new_node.first.pos << block_num + 1 << '\n';
      append_main(new_node);
    }
    write_main(tmp, it);
  }

  void remove(K &k, V &v) {
    KV kv = {k, v};
    auto it = list.upper_bound(kv);
    if (it == 0) return;
    DataNode tmp;
    read_main(tmp, it);

    if (kv == tmp._data[0]) {
      list.remove(k, v, it);
      if (tmp.size != 1) {
        list.insert(tmp._data[1].key, tmp._data[1].value, it);
      }
    }
    tmp.remove_pair(k, v);
    write_main(tmp, it);
  }

  void print() {
    list.print();
    DataNode tmp;
    for (int i = 1; i <= block_num; i++) {
      read_main(tmp, i);
      std::cout << i;
      tmp.print();
    }
  }

};


#endif
