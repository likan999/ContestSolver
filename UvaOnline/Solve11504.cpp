#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

struct Vertex {
  vector<int> outs;
  int scc;
  int preorder;
};

struct StackFrame {
  int ip = 0;
  int i;
  size_t nextOutIndex = 0;
};

Vertex vertices[100000];
int sccs, C;
vector<int> S, P;
bool ins[100000];
vector<StackFrame> stack;

void findScc(int i) {
  stack.clear();
  stack.emplace_back();
  stack.back().i = i;
outer:
  while (!stack.empty()) {
    StackFrame& current = stack.back();
    i = current.i;
    Vertex& v = vertices[i];
    switch (current.ip) {
      case 0:
        P.emplace_back(v.preorder = C++);
        S.emplace_back(i);
        current.ip = 1;

      case 1:
        while (current.nextOutIndex < v.outs.size()) {
          int out = v.outs[current.nextOutIndex++];
          Vertex& w = vertices[out];
          int p = w.preorder;
          if (p == -1) {
            stack.emplace_back();
            stack.back().i = out;
            goto outer;
          } else if (w.scc == -1) {
            while (P.back() > p) {
              P.pop_back();
            }
          }
        }
        if (P.back() == v.preorder) {
          P.pop_back();
          int top;
          do {
            top = S.back();
            S.pop_back();
            vertices[top].scc = sccs;
          } while (top != i);
          ++sccs;
        }
        stack.pop_back();
        break;

      default:
        __builtin_unreachable();
    }
  }
}

int compute() {
  int n, m;
  cin >> n >> m;
  for (int i = 0; i < n; i++) {
    Vertex& v = vertices[i];
    v.outs.clear();
    v.preorder = v.scc = -1;
  }
  for (int i = 0; i < m; i++) {
    int from, to;
    cin >> from >> to;
    vertices[from - 1].outs.emplace_back(to - 1);
  }
  for (int i = 0; i < n; i++) {
    vector<int>& outs = vertices[i].outs;
    sort(outs.begin(), outs.end());
    auto last = unique(outs.begin(), outs.end());
    outs.resize(last - outs.begin());
  }
  sccs = C = 0;
  S.clear();
  P.clear();
  for (int i = 0; i < n; i++) {
    if (vertices[i].preorder == -1) {
      findScc(i);
    }
  }
  fill(ins, ins + sccs, false);
  for (int i = 0; i < n; i++) {
    int from = vertices[i].scc;
    for (int out: vertices[i].outs) {
      int to = vertices[out].scc;
      if (to != from) {
        ins[to] = true;
      }
    }
  }
  return count(ins, ins + sccs, false);
}

int main() {
  istream::sync_with_stdio(false);
  int n;
  cin >> n;
  for (int i = 0; i < n; i++) {
    cout << compute() << '\n';
  }
  cout << flush;
  quick_exit(0);
}
