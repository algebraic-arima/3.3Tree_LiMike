#ifndef BPTREE_BLOCKRIVER_H
#define BPTREE_BLOCKRIVER_H
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <filesystem>
#include <utility>
#include <vector>
#include "error.h"
#include "BPtree.h"
#include "DataNode.h"

namespace venillalemon {
    template<class K, class V, size_t block>
    class BlockRiver {
    public:

      typedef pair<K, V> KV;
      typedef DataNode<K, V, block> DNode;
      typedef BPTree<K, V, 6, 2> map;

      size_t block_num = 0;
      std::fstream data_filer;
      std::string data_file;
      map list;

      explicit BlockRiver(const std::string &df) : data_file(df) {
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

      void append_main(DNode &t) {
        data_filer.open(data_file, std::ofstream::app);
        data_filer.write(reinterpret_cast<char *>(&t), sizeof(DNode));
        data_filer.close();
        ++block_num;
      }

      void write_main(DNode &t, const int pos) {
        if (pos > block_num) return;
        data_filer.open(data_file, std::ofstream::out | std::ifstream::in);
        data_filer.seekp(sizeof(block_num) + (pos - 1) * sizeof(DNode));
        data_filer.write(reinterpret_cast<char *> (&t), sizeof(DNode));
        data_filer.close();
      }

      void read_main(DNode &t, const int pos) {
        if (pos > block_num) return;
        data_filer.open(data_file, std::ifstream::in);
        data_filer.seekg(sizeof(block_num) + (pos - 1) * sizeof(DNode));
        data_filer.read(reinterpret_cast<char *> (&t), sizeof(DNode));
        data_filer.close();
      }

      void insert(K &k, V &v) {
        KV kv = {k, v};
        //std::cout<<tv.key<<tv.pos;
        if (list.empty()) {
          //std::cout << "empty\n";
          DNode tmp(kv);
          append_main(tmp);
          list.insert(k, v, 1);
          return;
        }
        size_t it = list.block_lower_bound(kv);

        if (it == 0) {
          list.insert_max_adjust(kv);
          // adjust the maximum to kv
          it = list.block_lower_bound(kv);
          // it points to a min node
        }// kv is greater than the maximum
        DNode tmp;
        read_main(tmp, it);
        tmp.insert_pair(k, v);
        if (tmp.size >= block) {
          DNode new_node;
          new_node.size = tmp.size / 2;
          for (int i = 0; i < tmp.size / 2; i++) {
            new_node._data[i] = tmp._data[i];
          }
          for (int i = tmp.size / 2; i < tmp.size; i++) {
            tmp._data[i - tmp.size / 2] = tmp._data[i];
          }
          tmp.size -= tmp.size / 2;
          list.insert(new_node._data[new_node.size - 1].key, new_node._data[new_node.size - 1].value, block_num + 1);
          //std::cout << new_node.first.key << new_node.first.pos << block_num + 1 << '\n';
          append_main(new_node);
        }
        write_main(tmp, it);
      }

      void remove(K &k, V &v) {
        KV kv = {k, v};
        auto it = list.block_lower_bound(kv);
        if (it == 0) return;
        DNode tmp;
        read_main(tmp, it);

        if (kv == tmp._data[tmp.size - 1]) {
          list.remove(k, v);
          if (tmp.size != 1) {
            list.insert(tmp._data[tmp.size - 2].key, tmp._data[tmp.size - 2].value, it);
          }
        }
        tmp.remove_pair(k, v);
        write_main(tmp, it);
      }

      void print() {
        list.print();
        DNode tmp;
        for (int i = 1; i <= block_num; i++) {
          read_main(tmp, i);
          std::cout << i;
          tmp.print();
        }
      }

    };

}

#endif
