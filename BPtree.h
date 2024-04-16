#ifndef BPTREE_BPTREE_H
#define BPTREE_BPTREE_H
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <filesystem>
#include <utility>
#include <vector>
#include "error.h"
#include "BNode.h"
#include "utility.h"

namespace venillalemon {
    template<class K, class V, size_t degree, size_t min_size>
    class BPTree {
    public:

      typedef BNode<K, V, degree> Node;
      typedef pair<K, V> p;

      size_t size = 0;
      size_t root = 0;// 0 means empty
      std::fstream index_filer;
      std::string index_file;
      std::vector<Node> list;

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

      /// the following 3 functions are used to modify list
      void init_list() {
        index_filer.open(index_file, std::ios::out);
        index_filer.write(reinterpret_cast<char *>(&size), sizeof(size_t));
        index_filer.write(reinterpret_cast<char *>(&root), sizeof(size_t));
        list.push_back(Node());
        index_filer.close();
      }

      void read_list() {
        index_filer.open(index_file, std::ios::in | std::ios::binary);
        index_filer.read(reinterpret_cast<char *>(&size), sizeof(size_t));
        index_filer.read(reinterpret_cast<char *>(&root), sizeof(size_t));
        Node tmp;
        list.push_back(tmp);
        for (int i = 0; i < size; i++) {
          index_filer.read(reinterpret_cast<char *>(&tmp), sizeof(Node));
          list.push_back(tmp);
        }
        index_filer.close();
      }

      void write_list() {
        index_filer.open(index_file, std::ios::in | std::ios::out | std::ios::binary);
        index_filer.write(reinterpret_cast<char *>(&size), sizeof(size_t));
        index_filer.write(reinterpret_cast<char *>(&root), sizeof(size_t));
        for (int i = 1; i < list.size(); i++) {
          index_filer.write(reinterpret_cast<char *>(&list[i]), sizeof(Node));
        }
        index_filer.close();
      }

      void read_node(Node &dn, size_t pos) {
        index_filer.open(index_file, std::ios::in | std::ios::binary);
        index_filer.seekg(2 * sizeof(size_t) + (pos - 1) * sizeof(Node));
        index_filer.read(reinterpret_cast<char *>(&dn), sizeof(Node));
        index_filer.close();
      }

      void write_node(Node &dn, size_t pos) {
        index_filer.open(index_file, std::ios::in | std::ios::out | std::ios::binary);
        index_filer.seekp(2 * sizeof(size_t) + (pos - 1) * sizeof(Node));
        index_filer.write(reinterpret_cast<char *>(&dn), sizeof(Node));
        index_filer.close();
      }

      void insert(const K &k, const V &v, size_t val) {
        if (root == 0) {
          Node tmp;
          tmp._size = 1;
          tmp._key[0] = p(k, v);
          tmp._chil[0] = val;
          tmp.is_leaf = true;
          root = 1;
          size = 1;
          list.push_back(tmp);
          return;
        }
        auto kv = p(k, v);
        size_t pos = list_lower_bound(kv);
        if (pos == 0) {
          insert_max_adjust(kv);
          pos = list_lower_bound(kv);
        }

        Node &node = list[pos];
        node.insert_pair(k, v, val);
        if (node._size == degree) {
          divide_node(pos);
        }
      }

      void remove(const K &k, const V &v) {
        auto kv = p(k, v);
        size_t pos = list_lower_bound(kv);
        if (pos == 0) {
          error("Key-value pair not found");
        }
        Node node;
        read_node(node, pos);
        if (kv == node._key[0]) {
          if (node._size < min_size) {

          } else subs(node._par, node._key[0], node._key[1]);
        }
        node.remove_pair(k, v);
        write_node(node, pos);

      }

      /// @find
      /// returns the possible position of the key-value pair
      /// i.e. the position of the last node
      /// such that kv >= node._key[0]
      size_t find(const K &k, const V &v) {
        return block_lower_bound({k, v});
      }

      void divide_node(size_t pos) {
        list.push_back(Node());
        ++size;
        Node &node = list[pos], &new_node = list.back();
        size_t mid = node._size / 2;
        for (size_t i = 0; i < mid; ++i) {
          new_node._key[i] = node._key[i];
          new_node._chil[i] = node._chil[i];
        }
        for (size_t i = mid; i < node._size; ++i) {
          node._key[i - mid] = node._key[i];
          node._chil[i - mid] = node._chil[i];
        }
        new_node.is_leaf = node.is_leaf;
        new_node._par = node._par;
        new_node._size = mid;
        node._size -= mid;
        if (!node.is_leaf) {
          for (size_t i = 0; i < new_node._size; ++i) {
            list[new_node._chil[i]]._par = size;
          }
        }
        if (node._par == 0) {
          list.push_back(Node());
          ++size;
          Node &root_node = list.back();
          root_node._size = 2;
          root_node._key[1] = node._key[node._size - 1];
          root_node._chil[1] = pos;
          root_node._key[0] = new_node._key[new_node._size - 1];
          root_node._chil[0] = size - 1;
          root_node.is_leaf = false;
          new_node._par = size, node._par = size;
          root = size;
        } else {
          Node &par_node = list[node._par];
          par_node.insert_pair(new_node._key[node._size - 1].first,
                               new_node._key[node._size - 1].second, size);
          if (par_node._size == degree) {
            divide_node(node._par);
          }
        }
      }

      /// @list_lower_bound
      /// returns the position of the last node
      /// whose first element is no greater than kv,
      /// in the leaf node layer
      size_t list_lower_bound(const p &kv) {
        size_t pos = root;
        while (!list[pos].is_leaf) {
          size_t i = list[pos].lower_bound(kv);
          if (i == list[pos]._size) return 0;
          pos = list[pos]._chil[i];
        }
        return pos;
      }

      /// @upper_bound returns the last position that kv >= *it,
      /// i.e. the position of the last element that is no greater than kv.
      /// If all elements are less than kv, the node on it is the max node;
      /// or if all elements are greater than kv, it = 0.
      size_t block_lower_bound(const p &kv) {
        Node node;
        size_t pos = list_lower_bound(kv);
        if (pos == 0) return 0;
        read_node(node, pos);
        size_t i = node._size;
        while (i > 0 && kv < node._key[i - 1]) --i;
        return node._chil[i - 1];
      }

      /// @subs substitute old_kv with new_kv in the node
      /// and recursively substitute the minimum in the parent nodes
      /// if the minimum is the old_kv
      void subs(size_t pos, const p &old_kv, const p &new_kv) {
        Node &node = list[pos];
        size_t i = 0;
        while (i < node._size && node._key[i] < old_kv) ++i;
        if (node._key[i] != old_kv) {
          error("Key-value pair not found");
        }
        node._key[i] = new_kv;
        if (i == 0 && node._par != 0) {
          subs(node._par, old_kv, new_kv);
        }
      }

      /// @insert_min_adjust
      /// if kv is less than all elements in the tree,
      /// adjust the minimum in the nodes to kv
      void insert_max_adjust(const p &kv) {
        size_t pos = root;
        while (!list[pos].is_leaf) {
          list[pos]._key[list[pos]._size - 1] = kv;
          pos = list[pos]._chil[list[pos]._size - 1];
        }
      }

      void clear() {
        size = 0;
        index_filer.open(index_file, std::ios::out);
        index_filer.close();
        init_list();
      }

      bool empty() {
        return size == 0;
      }

      void print() {
        std::cout << "root=" << root << '\n';
        for (int i = 1; i <= size; i++) {
          Node &node = list[i];
          std::cout << i << (root == i ? ": root" : (node.is_leaf ? ": leaf" : ": branch")) << '\n' << node._par
                    << '\n';
          for (int j = 0; j < node._size; j++) {
            std::cout << node._key[j];
          }
          std::cout << '\n';
          for (int j = 0; j < node._size; j++) {
            std::cout << "  " << node._chil[j];
          }
          std::cout << '\n';
        }
      }

      ~BPTree() {
        write_list();
      }

    };

}
#endif
