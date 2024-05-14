#ifndef BPTREE_UTILITY_H
#define BPTREE_UTILITY_H
#pragma once

#include <iostream>
#include <cstring>
#include <memory>

namespace arima_kana {

    template<int length>
    class m_string {
    public:
      char id[length]{};

      m_string() = default;

      explicit m_string(const char _key[]) {
        strcpy(id, _key);
      }

      m_string &operator=(const m_string &rhs) {
        if (this == &rhs) return *this;
        strcpy(id, rhs.id);
        return *this;
      }

      bool operator==(const m_string &rhs) const {
        return (strcmp(id, rhs.id) == 0);
      }

      bool operator!=(const m_string &rhs) const {
        return (strcmp(id, rhs.id) != 0);
      }

      bool operator<(const m_string &rhs) const {
        return (strcmp(id, rhs.id) < 0);
      }
    };

    template<int length>
    std::ostream &operator<<(std::ostream &os, const m_string<length> m) {
      os << "\"" << m.id << "\"";
      return os;
    }

    template<int length>
    std::istream &operator>>(std::istream &is, m_string<length> &m) {
      is >> m.id;
      return is;
    }

    template<int length>
    unsigned long long hash(const m_string<length> &key) {
      unsigned long long h = 0;
      for (int i = 0; i < length; i++) {
        h = h * 1471 + key.id[i];
      }
      return h;
    }

    template<class T1, class T2>
    class pair {
    public:
      T1 first;   // first element
      T2 second;  // second element

      constexpr pair() = default;

      constexpr pair(const pair &other) = default;

      constexpr pair(pair &&other) = default;

      pair &operator=(const pair &other) = default;

      template<class U1 = T1, class U2 = T2>
      constexpr pair(U1 &&x, U2 &&y)
              : first(std::forward<U1>(x)), second(std::forward<U2>(y)) {}

      template<class U1, class U2>
      constexpr pair(const pair<U1, U2> &other)
              : first(other.first), second(other.second) {}

      template<class U1, class U2>
      constexpr pair(pair<U1, U2> &&other)
              : first(std::move(other.first)), second(std::move(other.second)) {}

      bool operator<(const pair &other) const {
        return first < other.first || (first == other.first && second < other.second);
      }

      bool operator==(const pair &other) const {
        return first == other.first && second == other.second;
      }

      bool operator!=(const pair &other) const {
        return first != other.first || second != other.second;
      }


    };

    template<class T1, class T2>
    std::ostream &operator<<(std::ostream &os, const pair<T1, T2> &p) {
      os << '(' << p.first << ',' << p.second << ')';
      return os;
    }

    template<class T1, class T2>
    pair(T1, T2) -> pair<T1, T2>;

    template<typename T>
    struct allocator {
      T *allocate(size_t size) {
        return (T *) malloc(sizeof(T) * size);
      }

      void deallocate(void *p) {
        free(p);
      }

      void construct(T *p, const T &val) {
        new(p) T(val);
      }

      void destroy(T *p) {
        p->~T();
      }
    };

    template<class T, class _alloc=std::allocator<T>>
    class vector {
      T *data;
      size_t _size;
      size_t _capacity;
      _alloc alloc;

      void double_space() {
        _capacity *= 2;
        T *tmp = alloc.allocate(_capacity);
        for (size_t i = 0; i < _size; i++) {
          alloc.construct(tmp + i, data[i]);
        }
        alloc.deallocate(data, _capacity / 2);
        data = tmp;
      }

    public:
      vector(size_t cap = 10) : _size(0), _capacity(cap) {
        data = alloc.allocate(_capacity);
      }

      void push_back(const T &val) {
        if (_size == _capacity) {
          double_space();
        }
        alloc.construct(data + _size, val);
        _size++;
      }

      size_t size() const {
        return _size;
      }

      T &operator[](size_t idx) {
        return data[idx];
      }

      T &back() {
        return data[_size - 1];
      }

      T &front() {
        return data[0];
      }

      bool empty() {
        return _size == 0;
      }

      void pop_back() {
        if (_size != 0) {
          alloc.destroy(data + _size - 1);
          _size--;
        }
//        if (_size < _capacity / 3) {
//          _capacity /= 2;
//          T *tmp = new T[_capacity];
//          for (size_t i = 0; i < _size; i++) {
//            tmp[i] = data[i];
//          }
//          delete[] data;
//          data = tmp;
//        }
      }

      void clear() {
        for (size_t i = 0; i < _size; i++) {
          alloc.destroy(data + i);
        }
        alloc.deallocate(data, _capacity);
        _size = 0;
        _capacity = 10;
        data = alloc.allocate(_capacity);
      }

      ~vector() {
        for (size_t i = 0; i < _size; i++) {
          alloc.destroy(data + i);
        }
        alloc.deallocate(data, _capacity);
      }

    };
}


#endif