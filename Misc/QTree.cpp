#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>
using namespace std;

class RMQ {
 public:
  void initialize(vector<int>&& v) {
    int n = v.size();
    data.clear();
    data.emplace_back(move(v));
    while (n > 1) {
      int m = (n + 1) / 2;
      vector<int> w;
      w.reserve(m);
      for (auto it = data.back().cbegin(), end = data.back().cend(); it != end;) {
        int val = *it++;
        if (it != end) {
          val = max(val, *it++);
        }
        w.emplace_back(val);
      }
      data.emplace_back(move(w));
      n = m;
    }
  }

  // Max of [i, j).
  int query(int i, int j) {
    int val = numeric_limits<int>::min();
    for (auto it = data.cbegin(); j > i; ++it, i /= 2, j /= 2) {
      const auto& level = *it;
      if (i % 2 != 0) {
        val = max(val, level[i++]);
      }
      if (j % 2 != 0) {
        val = max(val, level[--j]);
      }
    }
    return val;
  }

  void update(int i, int val) {
    for (auto it = data.begin(), end = data.end(); it != end; ++it, i /= 2) {
      auto& level = *it;
      if (level[i] == val) {
        break;
      }
      level[i] = val;
      if (i % 2 != 0) {
        val = max(val, level[i - 1]);
      } else if (i + 1 < static_cast<int>(level.size())) {
        val = max(val, level[i + 1]);
      }
    }
  }
 private:
  vector<vector<int>> data;
};

class Graph {
 public:
  struct EdgeDescription {
    int u, v;
    int cost;
  };

  Graph(const vector<EdgeDescription>& descriptions) {
    int N = descriptions.size() + 1;
    vertices.resize(N);
    edges.resize(N - 1);
    for (int i = 0; i < N - 1; i++) {
      auto& e = edges[i];
      const auto& desc = descriptions[i];
      e.cost = desc.cost;
      e.u.vertex = &vertices[desc.u];
      e.u.vertex->edges.emplace_back(&e);
      e.v.vertex = &vertices[desc.v];
      e.v.vertex->edges.emplace_back(&e);
    }
    vector<Vertex*> q;
    for (auto& v : vertices) {
      if ((v.degree = v.edges.size()) == 1) {
        q.emplace_back(&v);
      }
    }
    while (!q.empty()) {
      auto v = q.back();
      q.pop_back();
      if (v->degree != 1) {
        continue;
      }
      int total = 1;
      Edge* next;
      for (auto e : v->edges) {
        if (e->self(v).size == -1) {
          next = e;
        } else {
          total += e->other(v).size;
        }
      }
      next->self(v).size = total;
      auto& other = next->other(v);
      other.size = N - total;
      if (--other.vertex->degree == 1) {
        q.emplace_back(other.vertex);
      }
    }
    root = createQTree(&vertices.front(), nullptr, nullptr, -1, 0);
  }

  void change(int e, int cost) {
    auto& edge = edges[e];
    edge.cost = cost;
    if (edge.rmq) {
      edge.rmq->update(edge.rmqIndex, cost);
    }
  }

  int query(int i, int j) {
    const Vertex& u = vertices[i];
    const Vertex& v = vertices[j];
    QTreeNode* uNode = u.node;
    QTreeNode* vNode = v.node;
    int uIndex = u.nodeIndex;
    int vIndex = v.nodeIndex;
    int val = numeric_limits<int>::min();

    int diff = uNode->depth - vNode->depth;
    for ( ; diff > 0; --diff) {
      up(uNode, uIndex, val);
    }
    for ( ; diff < 0; ++diff) {
      up(vNode, vIndex, val);
    }
    while (uNode != vNode) {
      up(uNode, uIndex, val);
      up(vNode, vIndex, val);
    }
    val = max(val, uNode->rmq.query(min(uIndex, vIndex), max(uIndex, vIndex)));
    return val;
  }

 private:
  struct Vertex;
  struct QTreeNode;

  struct Edge {
    int cost;
    // Index in the RMQ.
    int rmqIndex;
    RMQ* rmq = nullptr;
    // Whether the edge is connected.
    bool connected = true;

    struct VertexInfo {
      int size = -1;
      Vertex* vertex;
    } u,v;

    VertexInfo& self(Vertex* vertex) {
      return u.vertex == vertex ? u : v;
    }

    VertexInfo& other(Vertex* vertex) {
      return u.vertex == vertex ? v : u;
    }
  };

  struct Vertex {
    QTreeNode* node;
    int nodeIndex;

    int degree;
    vector<Edge*> edges;
  };

  struct QTreeNode {
    QTreeNode* parent;
    int parentIndex;
    int depth;
    RMQ rmq;
    vector<unique_ptr<QTreeNode>> children;
  };

  unique_ptr<QTreeNode> createQTree(Vertex* vertex, QTreeNode* parent, Edge* parentEdge, int parentIndex, int depth) {
    unique_ptr<QTreeNode> root(new QTreeNode);
    root->parent = parent;
    root->parentIndex = parentIndex;
    root->depth = depth;

    vector<int> edges;
    if (parentEdge) {
      edges.emplace_back(parentEdge->cost);
      parentEdge->rmq = &root->rmq;
      parentEdge->rmqIndex = 0;
    } else {
      edges.emplace_back(numeric_limits<int>::min());
    }

    for (int index = 1; ; ++index) {
      vertex->node = root.get();
      vertex->nodeIndex = index;
      Edge* maxEdge = nullptr;
      int maxSize = 0;
      for (auto e : vertex->edges) {
        if (e->connected) {
          int size = e->other(vertex).size;
          if (size > maxSize) {
            maxSize = size;
            maxEdge = e;
          }
        }
      }
      if (!maxEdge) {
        break;
      }
      for (auto e : vertex->edges) {
        if (e != maxEdge && e->connected) {
          e->connected = false;
          root->children.emplace_back(createQTree(e->other(vertex).vertex, root.get(), e, index, depth + 1));
        }
      }
      maxEdge->connected = false;
      maxEdge->rmq = &root->rmq;
      maxEdge->rmqIndex = index;
      edges.emplace_back(maxEdge->cost);
      vertex = maxEdge->other(vertex).vertex;
    }
    root->rmq.initialize(move(edges));

    return root;
  }

  void up(QTreeNode*& node, int& index, int& val) {
    val = max(val, node->rmq.query(0, index));
    index = node->parentIndex;
    node = node->parent;
  }

  vector<Vertex> vertices;
  vector<Edge> edges;
  unique_ptr<QTreeNode> root;
};

int main() {
  istream::sync_with_stdio(false);
  ostream::sync_with_stdio(false);
  cin.tie(nullptr);
  int t;
  cin >> t;
  vector<unique_ptr<Graph>> graphs;
  for (int i = 0; i < t; i++) {
    int N;
    cin >> N;
    vector<Graph::EdgeDescription> edges;
    for (int j = 1; j < N; j++) {
      edges.emplace_back();
      auto& desc = edges.back();
      cin >> desc.u >> desc.v >> desc.cost;
      --desc.u;
      --desc.v;
    }
    unique_ptr<Graph> graph(new Graph(edges));
    string command;
    while (command != "DONE") {
      cin >> command;
      if (command == "QUERY") {
        int i, j;
        cin >> i >> j;
        cout << graph->query(i - 1, j - 1) << '\n';
      } else if (command == "CHANGE") {
        int i, c;
        cin >> i >> c;
        graph->change(i - 1, c);
      }
    }
    graphs.emplace_back(move(graph));
  }
  cout.flush();
  quick_exit(0);
}
