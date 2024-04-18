#include <iostream>
#include <set>
#include <string>
#include "BPtree.h"

using std::cin;

template<class K, class V>
void std_map_print(std::set<venillalemon::pair<K, V>> &m) {
  for (auto &i: m) {
    std::cout << '(' << i.first << ',' << i.second << ')' << ' ';
  }
}

template<class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &m) {
  if (m.empty()) {
    std::cout << "null";
    return os;
  }
  for (auto &i: m) {
    os << i << ' ';
  }
  return os;
}

template<class K, class V>
void find_key(const std::set<venillalemon::pair<K, V>> &m, const K &k) {
  auto it = m.lower_bound({k, -10000});
  while (it != m.upper_bound({k, 10000})) {
    std::cout << it->second << ' ';
    ++it;
  }
  std::cout << '\n';
}

typedef venillalemon::m_string<70> mstr;

int main() {
  venillalemon::BPTree<mstr, int, 7, 2> bp("fn");
//  std::set<venillalemon::pair<mstr, int>> mp;
  mstr s;
  int n;
  cin >> n;
  std::string op;
  int val;
  while (n--) {
    cin >> op;
    if (op == "print") {
      bp.print();
      continue;
    } else if (op == "quit") {
      break;
    } else if (op == "clear") {
      bp.clear();
//      mp.clear();
    } else {
      cin >> s;
      if (op == "insert") {
        cin >> val;
        bp.insert(s, val, val * 1248 % 3413);
//        bp.map_print(bp.root);
//        std::cout << '\n';
//        mp.insert({s, val});
//        std_map_print(mp);
//        std::cout << '\n';
      } else if (op == "delete") {
        cin >> val;
        bp.remove(s, val);
//        bp.map_print(bp.root);
//        std::cout << '\n';
//        mp.erase({s, val});
//        std_map_print(mp);
//        std::cout << '\n';
      } else if (op == "find") {
//        cin >> val;
        std::cout << bp.find(s) << '\n';
//        find_key(mp, s);
      } else {
        continue;
      }
    }
  }
  return 0;
}