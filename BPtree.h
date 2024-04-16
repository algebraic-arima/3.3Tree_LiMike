#ifndef BPTREE_BPTREE_H
#define BPTREE_BPTREE_H
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <filesystem>
#include <utility>
#include "error.h"


template<class K, class V, size_t degree, size_t min_size>
class BPTree {
public:

  struct KV {
    K key;
    V value;

    bool operator<(const KV &rhs) const {
      return key < rhs.key || (key == rhs.key && value < rhs.value);
    }

    bool operator==(const KV &rhs) const {
      return key == rhs.key && value == rhs.value;
    }

    bool operator!=(const KV &rhs) const {
      return key != rhs.key || value != rhs.value;
    }
  };

  struct BNode {
    size_t size = 0;
    size_t par = 0, sib_next = 0, sib_pre = 0;// 1-based
    size_t _chil[degree] = {0};// in tree, point to the next node, 1-based
    // in file, point to the position of the data node
    KV _key[degree];
    bool is_leaf = false;

    void insert_pair(const K &k, const V &v, size_t val) {
      size_t l = 0, r = size;
      auto tmp_pair = KV({k, v});
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
      for (size_t i = size; i > r; --i) {
        _key[i] = _key[i - 1];
        _chil[i] = _chil[i - 1];
      }
      _key[r] = tmp_pair;
      _chil[r] = val;
      ++size;
    }

    void remove_pair(const K &k, const V &v) {
      size_t l = 0, r = size;
      auto tmp_pair = KV({k, v});
      if (tmp_pair == _key[size - 1]) {
        l = r = size - 1;
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
      for (size_t i = l; i < size - 1; ++i) {
        _key[i] = _key[i + 1];
        _chil[i] = _chil[i + 1];
      }
      --size;
    }
  };

  size_t size = 0;
  size_t root = 0;// 0 means empty
  std::fstream index_filer;
  std::string index_file;

  explicit BPTree(const std::string &ifn) : index_file(ifn + "_index") {
    index_filer.open(index_file, std::ios::in);
    if (!index_filer.is_open()) {
      index_filer.close();
      init_list();
    } else {
      index_filer.close();
      read_list();
    }
  }

  void init_list() {
    index_filer.open(index_file, std::ios::out);
    index_filer.write(reinterpret_cast<char *>(&size), sizeof(size_t));
    index_filer.write(reinterpret_cast<char *>(&root), sizeof(size_t));
    index_filer.close();
  }

  void read_list() {
    index_filer.open(index_file, std::ios::in | std::ios::binary);
    index_filer.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    index_filer.read(reinterpret_cast<char *>(&root), sizeof(size_t));
    index_filer.close();
  }

  void write_list() {
    index_filer.open(index_file, std::ios::in | std::ios::out | std::ios::binary);
    index_filer.write(reinterpret_cast<char *>(&size), sizeof(size_t));
    index_filer.write(reinterpret_cast<char *>(&root), sizeof(size_t));
    index_filer.close();
  }

  void read_node(BNode &dn, size_t pos) {
    index_filer.open(index_file, std::ios::in | std::ios::binary);
    index_filer.seekg(2 * sizeof(size_t) + (pos - 1) * sizeof(BNode));
    index_filer.read(reinterpret_cast<char *>(&dn), sizeof(BNode));
    index_filer.close();
  }

  void write_node(BNode &dn, size_t pos) {
    index_filer.open(index_file, std::ios::in | std::ios::out | std::ios::binary);
    index_filer.seekp(2 * sizeof(size_t) + (pos - 1) * sizeof(BNode));
    index_filer.write(reinterpret_cast<char *>(&dn), sizeof(BNode));
    index_filer.close();
  }

  void insert(const K &k, const V &v, size_t val) {
    if (root == 0) {
      BNode tmp;
      tmp.size = 1;
      tmp._key[0] = {k, v};
      tmp._chil[0] = val;
      tmp.is_leaf = true;
      root = 1;
      write_node(tmp, 1);
      ++size;
      return;
    }
    BNode node;
    KV kv = {k, v};
    size_t pos = list_upper_bound(kv);
    if (pos == 0) {
      insert_min_adjust(kv);
      pos = list_upper_bound(kv);
    }
    read_node(node, pos);
    node.insert_pair(k, v, val);
    write_node(node, pos);
    if (node.size == degree) {
      divide_node(pos);
    }
  }

  void remove(const K &k, const V &v) {
    KV kv = {k, v};
    size_t pos = list_upper_bound(kv);
    if (pos == 0) {
      error("Key-value pair not found");
    }
    BNode node;
    read_node(node, pos);
    if (kv == node._key[0]) {
      if (node.size < min_size) {

      } else subs(node.par, node._key[0], node._key[1]);
    }
    node.remove_pair(k, v);
    write_node(node, pos);

  }

  /// @find
  /// returns the possible position of the key-value pair
  /// i.e. the position of the last node
  /// such that kv >= node._key[0]
  size_t find(const K &k, const V &v) {
    return upper_bound({k, v});
  }

  void divide_node(size_t pos) {
    BNode node, new_node;
    read_node(node, pos);
    size_t mid = degree / 2;
    for (size_t i = mid; i < node.size; ++i) {
      new_node._key[i - mid] = node._key[i];
      new_node._chil[i - mid] = node._chil[i];
    }
    new_node.is_leaf = node.is_leaf;
    new_node.par = node.par;
    new_node.sib_next = node.sib_next;
    node.sib_next = size + 1;
    new_node.size = node.size - mid;
    node.size = mid;
    write_node(node, pos);
    write_node(new_node, ++size);
    for (size_t i = 0; i < new_node.size; ++i) {
      BNode tmp;
      read_node(tmp, new_node._chil[i]);
      tmp.par = size;
      write_node(tmp, new_node._chil[i]);
    }
    if (node.par == 0) {
      BNode root_node;
      root_node.size = 2;
      root_node._key[0] = node._key[0];
      root_node._chil[0] = pos;
      root_node._key[1] = new_node._key[0];
      root_node._chil[1] = size;
      root_node.is_leaf = false;
      write_node(root_node, ++size);
      new_node.par = size, node.par = size;
      root = size;
      write_node(node, pos);
      write_node(new_node, size - 1);
    } else {
      BNode par_node;
      read_node(par_node, node.par);
      par_node.insert_pair(new_node._key[0].key, new_node._key[0].value, size);
      write_node(par_node, node.par);
      if (par_node.size == degree) {
        divide_node(node.par);
      }
    }
  }

  /// @list_upper_bound
  /// returns the position of the last node
  /// whose first element is no greater than kv,
  /// in the leaf node layer
  size_t list_upper_bound(const KV &kv) {
    size_t pos = root;
    BNode node;
    read_node(node, pos);
    while (!node.is_leaf) {
      size_t i = node.size;
      while (i > 0 && kv < node._key[i - 1]) --i;
      if (i == 0) return 0;
      pos = node._chil[i - 1];
      read_node(node, pos);
    }
    return pos;
  }

  /// @upper_bound returns the last position that kv >= *it,
  /// i.e. the position of the last element that is no greater than kv.
  /// If all elements are less than kv, the node on it is the max node;
  /// or if all elements are greater than kv, it = 0.
  size_t upper_bound(const KV &kv) {
    BNode node;
    size_t pos = list_upper_bound(kv);
    if (pos == 0) return 0;
    read_node(node, pos);
    size_t i = node.size;
    while (i > 0 && kv < node._key[i - 1]) --i;
    return node._chil[i - 1];
  }

  ///@subs substitute old_kv with new_kv in the node
  /// and recursively substitute the minimum in the parent nodes
  /// if the minimum is the old_kv
  void subs(size_t pos, const KV &old_kv, const KV &new_kv) {
    BNode node;
    read_node(node, pos);
    size_t i = 0;
    while (i < node.size && node._key[i] < old_kv) ++i;
    if (node._key[i] != old_kv) {
      error("Key-value pair not found");
    }
    node._key[i] = new_kv;
    write_node(node, pos);
    if (i == 0 && node.par != 0) {
      subs(node.par, old_kv, new_kv);
    }
  }

  /// @insert_min_adjust
  /// if kv is less than all elements in the tree,
  /// adjust the minimum in the nodes to kv
  void insert_min_adjust(const KV &kv) {
    BNode node;
    size_t pos = root;
    read_node(node, pos);
    while (!node.is_leaf) {
      node._key[0] = kv;
      write_node(node, pos);
      pos = node._chil[0];
      read_node(node, pos);
    }
  }

  void clear() {
    size = 0;
    index_filer.open(index_file, std::ios::out);
    index_filer.close();
    write_list();
  }

  bool empty() {
    return size == 0;
  }

  void print() {
    std::cout << "root=" << root << '\n';
    BNode node;
    for (int i = 1; i <= size; i++) {
      read_node(node, i);
      std::cout << i << (root == i ? ": root" : (node.is_leaf ? ": leaf" : ": branch")) << '\n' << node.par << '\n';
      for (int j = 0; j < node.size; j++) {
        std::cout << "  (" << node._key[j].key << "," << node._key[j].value << ")";
      }
      std::cout << node.sib_next << '\n';
      for (int j = 0; j < node.size; j++) {
        std::cout << "  " << node._chil[j];
      }
      std::cout << '\n';
    }
  }

  ~BPTree() {
    write_list();
  }

};

#endif
