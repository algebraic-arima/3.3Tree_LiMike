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
#include "DataNode.h"
#include "Buffer.h"

namespace arima_kana {
    template<class K, class V, size_t block>
    class BlockRiver {
    public:

      typedef pair<K, V> KV;
      typedef DataNode<K, V, block> DNode;
      typedef BPTree<K, V, 200, 80> map;
      typedef Buffer<DNode, size_t, 1, 120> buffer;

      size_t block_num = 0;
      std::fstream data_filer;
      std::string data_file;
      map list;
      buffer data_list;

      explicit BlockRiver(const std::string &df) :
              data_file(df),
              list(df),
              data_list(df) {
        data_filer.open(data_file, std::ios::in);
        if (!data_filer.is_open()) {
          data_filer.close();
          init_data();
        } else {
          data_filer.close();
          read_data();
        }
      }

      void write_data() {
        data_filer.open(data_file, std::ios::in | std::ios::out | std::ios::binary);
        data_filer.write(reinterpret_cast<char *>(&block_num), sizeof(size_t));
        data_filer.close();
      }

      ~BlockRiver() {
//        std::cout << "~BlockRiver\n";
        write_data();
      }

      void init_data() {
        data_filer.open(data_file, std::ios::out | std::ios::binary);
        data_filer.write(reinterpret_cast<char *>(&block_num), sizeof(size_t));
        data_filer.close();
      }

      void read_data() {
        data_filer.open(data_file, std::ios::in | std::ios::binary);
        data_filer.read(reinterpret_cast<char *>(&block_num), sizeof(size_t));
        data_filer.close();
      }

      void append_main(DNode &t) {
        data_filer.open(data_file, std::ios::app);
        data_filer.write(reinterpret_cast<char *>(&t), sizeof(DNode));
        data_filer.close();
        ++block_num;
      }

      void write_main(DNode &t, const int pos) {
        if (pos > block_num) return;
        data_filer.open(data_file, std::ios::out | std::ios::in | std::ios::binary);
        data_filer.seekp(sizeof(block_num) + (pos - 1) * sizeof(DNode));
        data_filer.write(reinterpret_cast<char *> (&t), sizeof(DNode));
        data_filer.close();
      }

      void read_main(DNode &t, const int pos) {
        if (pos > block_num) return;
        data_filer.open(data_file, std::ios::in | std::ios::binary);
        data_filer.seekg(sizeof(block_num) + (pos - 1) * sizeof(DNode));
        data_filer.read(reinterpret_cast<char *> (&t), sizeof(DNode));
        data_filer.close();
      }

      void insert(const K &k, V &v) {
        KV kv = {k, v};
        //std::cout<<tv.key<<tv.pos;
        if (list.empty()) {
          //std::cout << "empty\n";
          DNode tmp(kv);
          append_main(tmp);
          list.insert(k, v, block_num);
          data_list[block_num] = tmp;
          return;
        }
        size_t it = list.block_lower_bound(kv);

        if (it == 0) {
          list.adjust_max(kv);
          // adjust the maximum to kv
          it = list.block_lower_bound(kv);
          // it points to a min node
        }// kv is greater than the maximum
        DNode &tmp = data_list[it];
        try { tmp.insert_pair(k, v); }
        catch (...) { return; }
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
          list.insert(new_node._data[new_node.size - 1].first, new_node._data[new_node.size - 1].second, block_num + 1);
          //std::cout << new_node.first.key << new_node.first.pos << block_num + 1 << '\n';
          append_main(new_node);
          data_list[block_num] = new_node;
        }
      }

      void remove(const K &k, V &v) {
        KV kv = {k, v};
        auto it = list.block_lower_bound(kv);
        if (it == 0) return;
        DNode &tmp = data_list[it];

        if (kv == tmp._data[tmp.size - 1]) {
          list.remove(k, v);
          if (tmp.size != 1) {
            list.insert(tmp._data[tmp.size - 2].first, tmp._data[tmp.size - 2].second, it);
          }
        }
        try { tmp.remove_pair(k, v); }
        catch (...) { return; }
      }

      void find(const K &k) {
        bool flag = false;
        arima_kana::vector<size_t> tmp = list.find(k);
        for (int i = 0; i < tmp.size(); i++) {
          DNode &t = data_list[tmp[i]];
          for (int j = 0; j < t.size; j++) {
            if (t._data[j].first == k) {
              std::cout << t._data[j].second << ' ';
              flag = true;
            }
          }
        }
        if (!flag) std::cout << "null";
        std::cout << '\n';
      }

      void print() {
        list.print();
        for (int i = 1; i <= block_num; i++) {
          DNode &tmp = data_list[i];
          std::cout << i << '\n';
          tmp.print();
        }
      }

      void clear() {
        list.clear();
        block_num = 0;
        init_data();
      }

    };

}

#endif
