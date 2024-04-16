#ifndef BPTREE_VECTOR_HPP
#define BPTREE_VECTOR_HPP
#pragma once

#include <iostream>

namespace venillalemon {

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


    template<class T>
    class vector {
      T *data;
      size_t _size;
      size_t _capacity;

      void double_space() {
        _capacity *= 2;
        T *tmp = new T[_capacity];
        for (size_t i = 0; i < _size; i++) {
          tmp[i] = data[i];
        }
        delete[] data;
        data = tmp;
      }

    public:
      vector(size_t cap = 10) : _size(0), _capacity(cap) {
        data = new T[_capacity];
      }

      void push_back(const T &val) {
        if (_size == _capacity) {
          double_space();
        }
        data[_size++] = val;
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

    };
}


#endif