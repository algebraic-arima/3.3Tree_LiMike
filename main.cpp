#include <iostream>
#include <fstream>
#include <string>
#include "BPtree.h"

using std::cin;

int main() {
  venillalemon::BPTree<int, int, 6, 2> bp("fn");
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
    } else {
      cin >> s;
      if (op == "insert") {
        cin >> val;
        bp.insert(s, val, val * 1248 % 3413);
        bp.print();
      } else if (op == "delete") {
        cin >> val;
        bp.remove(s, val);
        bp.print();
      } else if (op == "find") {
        cin >> val;
        std::cout << bp.find(s, val) << '\n';
      } else if (op == "ns") {
//        std::cout<<bp.next_sibling(s)<<'\n';
      } else if (op == "ps") {
//        std::cout<<bp.prev_sibling(s)<<'\n';
      } else {
        continue;
      }
    }
  }
  return 0;
}