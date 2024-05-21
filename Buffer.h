#ifndef BPTREE_BUFFER_H
#define BPTREE_BUFFER_H
#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include "map.h"

namespace arima_kana {
    template<class T, class pre, size_t num>
    class Buffer {
      static constexpr int SIZE_T = sizeof(T);
      static constexpr int SIZE_PRE = sizeof(pre);

    public:
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

      virtual T &operator[](size_t pos) = 0;

      virtual void clear() = 0;


      std::fstream file;
      std::string name;
    public:
      Buffer(const std::string &fn) : name(fn) {}

    };


    template<class T, class pre, size_t num, size_t _cap>
    class List_Buffer : public Buffer<T, pre, num> {

      struct Node {
        size_t pos;
        T data;
//        T copy;
        Node *next;
        Node *prev;
      };


      Node *head;
      Node *tail;
      size_t _size;

    public:

      explicit List_Buffer(const std::string &fn) :
              Buffer<T, pre, num>(fn) {
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

      ~List_Buffer() {
//        std::cout << "~Buffer\n";
        Node *tmp = head->next;
        while (tmp != tail) {
//          if (!(tmp->data == tmp->copy))
          this->write_node(tmp->data, tmp->pos);
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
        Node *new_n = new Node{pos, T(), head->next, head};
        this->read_node(new_n->data, pos);
//        new_n->copy = new_n->data;
        head->next->prev = new_n;
        head->next = new_n;
        ++_size;
        if (_size > _cap) {
          tmp = tail->prev;
          tmp->prev->next = tail;
          tail->prev = tmp->prev;
//          if (!(tmp->data == tmp->copy))
          this->write_node(tmp->data, tmp->pos);
          delete tmp;
          --_size;
        }
        return new_n->data;
      }

    };

    template<class T, class pre, size_t num>
    class Table_Buffer : public Buffer<T, pre, num> {

      struct Node {
        T data;
        bool dirty;
      };


      vector<Node> table;
    public:

      explicit Table_Buffer(const std::string &fn) : table(), Buffer<T, pre, num>(fn) {}

      ~Table_Buffer() {
        for (size_t i = 0; i < table.size(); ++i) {
          if (table[i].dirty) {
            this->write_node(table[i].data, i);
            table[i].dirty = false;
          }
        }
      }

      void clear() {
        table.clear();
      }

      T &operator[](size_t pos) {
        if (!table[pos].dirty) {
          this->read_node(table[pos].data, pos);
          table[pos].dirty = true;
        }
        return table[pos].data;
      }

      void push_back(const T &val) {
        table.push_back({val, true});
      }

      void resize(size_t size) {
        table.resize(size);
        for (size_t i = 0; i < size; ++i) {
          table[i].dirty = false;
        }
      }

    };

    template<class T, class pre, size_t num, size_t cap>
    class Map_Buffer : public Buffer<T, pre, num> {

      std::unordered_map<size_t, size_t> m;
      vector<T> table;
      vector<size_t> free_pos;

    public:
      explicit Map_Buffer(const std::string &fn) :
              Buffer<T, pre, num>(fn),
              table() {}

      T &operator[](size_t pos) {
        if (m.find(pos) == m.end()) {
          T tmp;
          this->read_node(tmp, pos);
          if (free_pos.empty()) {
            m[pos] = table.size();
            table.push_back(tmp);
          } else {
            m[pos] = free_pos.back();
            table[free_pos.back()] = tmp;
            free_pos.pop_back();
          }
          if (m.size() > cap) {
            auto it = m.begin();
            this->write_node(table[it->second], it->first);
            m.erase(it);
            free_pos.push_back(it->second);
          }
        }
        return table[m[pos]];
      }

      void clear() {
        table.clear();
        m.clear();
      }

      ~Map_Buffer() {
        for (auto &i: m) {
          this->write_node(table[i.second], i.first);
        }
      }
    };

    template<class T, class pre, size_t num, size_t cap>
    class List_Map_Buffer : public Buffer<T, pre, num> {
      struct Node {
        T data;
        Node *next;
        Node *prev;
      };

      Node *head;
      Node *tail;
      size_t _size;
      std::unordered_map<size_t, Node *> m;

      explicit List_Map_Buffer(const std::string &fn) :
              Buffer<T, pre, num>(fn),
              _size(0) {
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->prev = head;
      }

      ~List_Map_Buffer() {
        for (auto &i: m) {
          this->write_node(i.second->data, i.first);
          delete i.second;
        }
        delete head;
        delete tail;
      }


    };

}

#endif //BPTREE_BUFFER_H
