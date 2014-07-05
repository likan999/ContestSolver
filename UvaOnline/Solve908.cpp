#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>
#include <tuple>
#include <vector>

using namespace std;

random_device rd;
mt19937 gen(rd());

struct Edge {
  int weight;
  int u;
  int v;
  int uNext;
  int vNext;
};

struct Vertex {
  int parent;
  int parentEdge;
  int depth;
};

int firstEdges[1000000];
Edge edges[1000000];
Vertex vertices[1000000];
vector<int> queue;
vector<int> uToRoot, vToRoot;

void skipLines() {
  int m;
  cin >> m;
  for (int i = 0; i <= m; i++) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }
}

bool solve(int t) {
  int n;
  if (!(cin >> n)) {
    return false;
  }
  if (n <= 1) {
    skipLines();
    skipLines();
    cout << (t > 0 ? "\n" : "") << "0\n0\n";
    return true;
  }

  // Read in edges.
  for (int i = 0; i < n; i++) {
    firstEdges[i] = -1;
  }
  int originalTotal = 0;
  for (int i = 0; i < n - 1; i++) {
    int u, v, weight;
    cin >> u >> v >> weight;
    auto& edge = edges[i];
    edge.u = --u;
    edge.v = --v;
    edge.weight = weight;
    edge.uNext = firstEdges[u];
    edge.vNext = firstEdges[v];
    firstEdges[u] = i;
    firstEdges[v] = i;
    originalTotal += weight;
  }

  for (int i = 0; i < n; i++) {
    vertices[i].depth = -1;
  }
  uniform_int_distribution<> dist(0, n - 1);
  int root = dist(gen);
  vertices[root].depth = 0;
  queue.clear();
  queue.emplace_back(root);
  while (!queue.empty()) {
    int i = queue.back();
    queue.pop_back();
    const auto& parent = vertices[i];
    int depth = parent.depth + 1;
    for (int e = firstEdges[i]; e != -1; ) {
      const auto& edge = edges[e];
      int other, oldE = e;
      if (edge.u == i) {
        other = edge.v;
        e = edge.uNext;
      } else {
        other = edge.u;
        e = edge.vNext;
      }
      auto& child = vertices[other];
      if (child.depth == -1) {
        child.parent = i;
        child.parentEdge = oldE;
        child.depth = depth;
        queue.emplace_back(other);
      }
    }
  }
  int k;
  cin >> k;
  int total = originalTotal;
  for (int i = 0; i < k; i++) {
    int u, v, weight;
    cin >> u >> v >> weight;
    if (u == v) {
      continue;
    }
    --u;
    --v;
    uToRoot.clear();
    vToRoot.clear();
    int diff = vertices[u].depth - vertices[v].depth;
    int uCurrent = u, vCurrent = v;
    if (diff > 0) {
      for (int j = 0; j < diff; j++) {
        uToRoot.emplace_back(uCurrent);
        uCurrent = vertices[uCurrent].parent;
      }
    } else {
      for (int j = 0; j < -diff; j++) {
        vToRoot.emplace_back(vCurrent);
        vCurrent = vertices[vCurrent].parent;
      }
    }
    while (uCurrent != vCurrent) {
      uToRoot.emplace_back(uCurrent);
      vToRoot.emplace_back(vCurrent);
      uCurrent = vertices[uCurrent].parent;
      vCurrent = vertices[vCurrent].parent;
    }
    int maxWeight = weight;
    int best;
    bool bestOnUSide;
    for (auto j: uToRoot) {
      auto& vertex = vertices[j];
      int w = edges[vertex.parentEdge].weight;
      if (w > maxWeight) {
        maxWeight = w;
        best = j;
        bestOnUSide = true;
      }
    }
    for (auto j: vToRoot) {
      auto& vertex = vertices[j];
      int w = edges[vertex.parentEdge].weight;
      if (w > maxWeight) {
        maxWeight = w;
        best = j;
        bestOnUSide = false;
      }
    }
    if (maxWeight == weight) {
      continue;
    }

    int e = vertices[best].parentEdge;
    edges[e].u = u;
    edges[e].v = v;
    edges[e].weight = weight;
    total += weight - maxWeight;

    int current = bestOnUSide ? u : v;
    int parent = bestOnUSide ? v : u;
    for (int stop = vertices[best].parent, depth = vertices[parent].depth + 1; current != stop; ++depth) {
      Vertex& currentVertex = vertices[current];
      tie(parent, current, currentVertex.parent) = make_tuple(current, currentVertex.parent, parent);
      swap(e, currentVertex.parentEdge);
      currentVertex.depth = depth;
    }
  }
  skipLines();

  cout << (t > 0 ? "\n" : "") << originalTotal << '\n' << total << '\n';
  return true;
}

int main() {
  cin.sync_with_stdio(false);
  for (int i = 0; solve(i); i++);
  quick_exit(0);
}
