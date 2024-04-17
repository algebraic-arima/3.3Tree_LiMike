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

int main() {
  venillalemon::BPTree<int, int, 6, 2> bp("fn");
  std::set<venillalemon::pair<int, int>> mp;
  int s;
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
      mp.clear();
    } else {
      cin >> s;
      if (op == "insert") {
        cin >> val;
        bp.insert(s, val, val * 1248 % 3413);
        bp.map_print(bp.root);
        std::cout << '\n';
        mp.insert({s, val});
        std_map_print(mp);
        std::cout << '\n';
      } else if (op == "delete") {
        cin >> val;
        bp.remove(s, val);
        bp.map_print(bp.root);
        std::cout << '\n';
        mp.erase({s,val});
        std_map_print(mp);
        std::cout << '\n';
      } else if (op == "find") {
        cin >> val;
        std::cout << bp.find(s, val) << '\n';
      } else {
        continue;
      }
    }
  }
  return 0;
}