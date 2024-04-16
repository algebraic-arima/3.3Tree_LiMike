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
      typedef BNode<K, V, degree> Node;
      typedef pair<K, V> p;

      size_t vacant_pos() {
        if (free_pos.empty()) {
          list.push_back(Node());
          return ++size;
        } else {
          size_t pos = free_pos.back();
          free_pos.pop_back();
          return pos;
        }
      }

      /// @divide_node
      /// divide the node at pos, with
      /// the first half in the new node
      void divide_node(size_t pos) {
        size_t new_pos = vacant_pos();
        Node &node = list[pos], &new_node = list[new_pos];
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
            list[new_node._chil[i]]._par = new_pos;
          }
        }
        if (node._par == 0) {
          size_t new_root_pos = vacant_pos();
          Node &root_node = list[new_root_pos];
          root_node._size = 2;
          root_node._key[1] = node._key[node._size - 1];
          root_node._chil[1] = pos;
          root_node._key[0] = new_node._key[new_node._size - 1];
          root_node._chil[0] = new_pos;
          root_node.is_leaf = false;
          new_node._par = new_root_pos, node._par = new_root_pos;
          root = new_root_pos;
        } else {
          Node &par_node = list[node._par];
          par_node.insert_pair(new_node._key[node._size - 1].first,
                               new_node._key[node._size - 1].second, new_pos);
          if (par_node._size == degree) {
            divide_node(node._par);
          }
        }
      }

      /// @block_lower_bound returns the last position that kv >= *it,
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
      /// and recursively substitute the maximum in the parent nodes
      /// if the maximum is the old_kv
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

      /// @insert_max_adjust
      /// if kv is greater than all elements in the tree,
      /// adjust the maximum in the nodes to kv
      void insert_max_adjust(const p &kv) {
        size_t pos = root;
        while (!list[pos].is_leaf) {
          list[pos]._key[list[pos]._size - 1] = kv;
          pos = list[pos]._chil[list[pos]._size - 1];
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

      size_t next_sibling(size_t pos) {
        if (list[pos]._par == 0) return 0;
        size_t par = list[pos]._par;
        size_t i = list[par]._size - 1;
        while (i > 0 && list[par]._chil[i] != pos) --i;
        int cnt = 0;
        while (i == list[par]._size - 1) {
          pos = list[pos]._par;
          ++cnt;
          if (list[par]._par == 0) return 0;// no next sibling
          par = list[par]._par;
          i = list[par]._size - 1;
          while (i > 0 && list[par]._chil[i] != pos) --i;
        }
        ++i;
        pos = list[par]._chil[i];
        while (cnt--) {
          pos = list[pos]._chil[0];
        }
        return pos;
      }

      size_t prev_sibling(size_t pos) {
        if (list[pos]._par == 0) return 0;
        size_t par = list[pos]._par;
        size_t i = 0;
        while (i < list[par]._size - 1 && list[par]._chil[i] != pos) ++i;
        int cnt = 0;
        while (i == 0) {
          pos = list[pos]._par;
          ++cnt;
          if (list[par]._par == 0) return 0;// no next sibling
          par = list[par]._par;
          i = 0;
          while (i < list[par]._size - 1 && list[par]._chil[i] != pos) ++i;
        }
        --i;
        pos = list[par]._chil[i];
        while (cnt--) {
          pos = list[pos]._chil[list[pos]._size - 1];
        }
        return pos;
      }

      /// @next_sp_sibling
      /// returns the next sibling in the same parent node
      size_t next_sp_sibling(size_t pos) {
        if (list[pos]._par == 0) return 0;
        size_t par = list[pos]._par;
        size_t i = list[par]._size - 1;
        while (i > 0 && list[par]._chil[i] != pos) --i;
        if (i == list[par]._size - 1) {
          return 0;
        }
        ++i;
        pos = list[par]._chil[i];
        return pos;
      }

      /// @prev_sp_sibling
      /// returns the previous sibling in the same parent node
      size_t prev_sp_sibling(size_t pos) {
        if (list[pos]._par == 0) return 0;
        size_t par = list[pos]._par;
        size_t i = 0;
        while (i < list[par]._size - 1 && list[par]._chil[i] != pos) ++i;
        if (i == 0) {
          return 0;
        }
        --i;
        pos = list[par]._chil[i];
        return pos;
      }

      void merge(size_t l, size_t r) {
        size_t par = list[l]._par;
        for (int j = list[r]._size - 1; j >= 0; j--) {
          list[r]._key[j + list[l]._size] = list[r]._key[j];
          list[r]._chil[j + list[l]._size] = list[r]._chil[j];
        }
        for (int j = 0; j < list[l]._size; j++) {
          list[r]._key[j] = list[l]._key[j];
          list[r]._chil[j] = list[l]._chil[j];
        }
        list[par].remove_pair(list[l]._key[list[l]._size - 1].first, list[l]._key[list[l]._size - 1].second);
        list[r]._size += list[l]._size;
        list[l]._size = 0;
        free_pos.push_back(l);
        if(list[par]._size < min_size) {
          l = prev_sp_sibling(par), r = next_sp_sibling(par);
          if (l != 0) {
            if (list[l]._size > min_size) borrow_from_left(l, par);
            else merge(l, par);
          } else if (r != 0) {
            if (list[r]._size > min_size) borrow_from_right(par, r);
            else merge(par, r);
          }
        }
      }

      void borrow_from_left(size_t l, size_t r) {
        size_t par = list[l]._par;
        size_t bor_num = (list[l]._size - list[r]._size) / 2;
        size_t bor_st = list[l]._size - bor_num;
        for (int j = list[r]._size - 1; j >= 0; j--) {
          list[r]._key[j + bor_num] = list[r]._key[j];
          list[r]._chil[j + bor_num] = list[r]._chil[j];
        }
        for (int j = 0; j < bor_num; j++) {
          list[r]._key[j] = list[l]._key[j + bor_st];
          list[r]._chil[j] = list[l]._chil[j + bor_st];
        }
        list[l]._size -= bor_num;
        list[r]._size += bor_num;
        list[par].modify_pair(list[r]._key[bor_num - 1], list[l]._key[list[l]._size - 1]);
      }

      void borrow_from_right(size_t l, size_t r) {
        size_t par = list[l]._par;
        size_t bor_num = (list[r]._size - list[l]._size) / 2;
        for (int j = 0; j < bor_num; j++) {
          list[l]._key[list[l]._size + j] = list[r]._key[j];
          list[l]._chil[list[l]._size + j] = list[r]._chil[j];
        }
        for (int j = 0; j < list[r]._size - bor_num; j++) {
          list[r]._key[j] = list[r]._key[j + bor_num];
          list[r]._chil[j] = list[r]._chil[j + bor_num];
        }
        list[r]._size -= bor_num;
        list[l]._size += bor_num;
        list[par].modify_pair(list[l]._key[list[l]._size - bor_num - 1], list[r]._key[bor_num]);
      }

    public:

      size_t size = 0;
      size_t root = 0;// 0 means empty
      std::fstream index_filer;
      std::string index_file;
      std::vector<Node> list;
      std::vector<size_t> free_pos;

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
        Node &node = list[pos];
        if (kv == node._key[node._size - 1]) {
          if (node._size < min_size) {
            size_t l = prev_sp_sibling(pos), r = next_sp_sibling(pos);
            if (l != 0) {
              if (list[l]._size > min_size) borrow_from_left(l, pos);
              else merge(l, pos);
            } else if (r != 0) {
              if (list[r]._size > min_size) borrow_from_right(pos, r);
              else merge(pos, r);
            }
          } else subs(node._par, node._key[node._size - 1], node._key[node._size - 2]);
        }
        node.remove_pair(k, v);
      }

      /// @find
      /// returns the possible position of the key-value pair
      /// i.e. the position of the last node
      /// such that kv >= node._key[0]
      size_t find(const K &k, const V &v) {
        return block_lower_bound({k, v});
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
