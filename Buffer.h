#ifndef BPTREE_BUFFER_H
#define BPTREE_BUFFER_H
#pragma once

#include <iostream>
#include <fstream>

namespace arima_kana {
    template<class T, class pre, size_t num, size_t _cap>
    class Buffer {

      static constexpr int SIZE_T = sizeof(T);
      static constexpr int SIZE_PRE = sizeof(pre);

      struct Node {
        size_t pos;
        T data;
        T copy;
        Node *next;
        Node *prev;
      };

      void read_node(T &dn, size_t pos) {
        file.open(name, std::ios::in | std::ios::out | std::ios::binary);
        file.seekg(num * SIZE_PRE + (pos - 1) * SIZE_T);
        file.read(reinterpret_cast<char *>(&dn), SIZE_T);
        file.close();
      }

      void write_node(T &dn, size_t pos) {
        file.open(name, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(num * SIZE_PRE + (pos - 1) * SIZE_T);
        file.write(reinterpret_cast<char *>(&dn), SIZE_T);
        file.close();
      }

      Node *head;
      Node *tail;
      size_t _size;
      std::fstream file;
      std::string name;
    public:

      Buffer(const std::string &fn) {
        name = fn;
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->prev = head;
        _size = 0;
      }

      void clear() {
        Node *tmp = head->next;
        while (tmp != tail) {
          Node *tmp2 = tmp;
          tmp = tmp->next;
          delete tmp2;
        }
        head->next = tail;
        tail->prev = head;
        _size = 0;
      }

      ~Buffer() {
//        std::cout << "~Buffer\n";
        Node *tmp = head->next;
        while (tmp != tail) {
          if (!(tmp->data == tmp->copy))
            write_node(tmp->data, tmp->pos);
          Node *tmp2 = tmp;
          tmp = tmp->next;
          delete tmp2;
        }
        delete head;
        delete tail;
      }

      T &operator[](size_t pos) {
        Node *tmp = head->next;
        while (tmp != tail) {
          if (tmp->pos == pos) {
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            tmp->next = head->next;
            tmp->prev = head;
            head->next->prev = tmp;
            head->next = tmp;
            return tmp->data;
          }
          tmp = tmp->next;
        }
        Node *new_n = new Node{pos, T(), T(), head->next, head};
        read_node(new_n->data, pos);
        new_n->copy = new_n->data;
        head->next->prev = new_n;
        head->next = new_n;
        ++_size;
        if (_size > _cap) {
          tmp = tail->prev;
          tmp->prev->next = tail;
          tail->prev = tmp->prev;
          if (!(tmp->data == tmp->copy))
            write_node(tmp->data, tmp->pos);
          delete tmp;
          --_size;
        }
        return new_n->data;
      }

    };

    template<class T, class pre, size_t num>
    class Table_Buffer {
      static constexpr int SIZE_T = sizeof(T);
      static constexpr int SIZE_PRE = sizeof(pre);

      void read_node(T &dn, size_t pos) {
        file.open(name, std::ios::in | std::ios::out | std::ios::binary);
        file.seekg(num * SIZE_PRE + (pos - 1) * SIZE_T);
        file.read(reinterpret_cast<char *>(&dn), SIZE_T);
        file.close();
      }

      void write_node(T &dn, size_t pos) {
        file.open(name, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(num * SIZE_PRE + (pos - 1) * SIZE_T);
        file.write(reinterpret_cast<char *>(&dn), SIZE_T);
        file.close();
      }

      vector<T *> table;
      std::fstream file;
      std::string name;

      explicit Table_Buffer(const std::string &fn, size_t c) : table(c) {
        name = fn;
      }

      ~Table_Buffer() {
        for (size_t i = 0; i < table.size(); ++i){
          if (table[i] != nullptr) {
            write_node(*table[i], i);
            delete table[i];
            table[i] = nullptr;
          }
        }
      }

      T &operator[](size_t pos) {
        if (table[pos] == nullptr) {
          table[pos] = new T();
          read_node(*table[pos], pos);
        }
        return *table[pos];
      }

    };
}

#endif //BPTREE_BUFFER_H
