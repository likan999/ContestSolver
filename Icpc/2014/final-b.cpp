#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

using namespace std;

int d, w;
map<int, vector<pair<int, int>>> discreteDishes;
map<int, vector<int>> concreteDishes;
vector<int64_t> discreteTastes;
vector<bool> discretePossible;
vector<double> concreteTastes;

void readInput() {
  cin >> d >> w;
  for (int i = 0; i < d; i++) {
    char c;
    cin >> c;
    if (c == 'D') {
      int w, t, delta;
      cin >> w >> t >> delta;
      discreteDishes[w].emplace_back(t, delta);
    } else {
      int t, delta;
      cin >> t >> delta;
      concreteDishes[t].emplace_back(delta);
    }
  }
}

void solveDiscrete() {
  discreteTastes.resize(w + 1);
  discretePossible.resize(w + 1);
  discreteTastes[0] = 0;
  discretePossible[0] = true;

  auto heapComparator = [](const pair<int, int>& left, const pair<int, int>& right) {
    return left.second < right.second;
  };
  vector<pair<int, int64_t>> tastes;
  vector<pair<int, int>> heap;

  for (const auto& p: discreteDishes) {
    int wi = p.first;
    const auto& dishes = p.second;
    heap.clear();
    for (int i = 0; i < dishes.size(); i++) {
      heap.emplace_back(i, dishes[i].first);
    }
    make_heap(heap.begin(), heap.end(), heapComparator);
    int64_t taste = 0;
    tastes.clear();
    for (int weight = wi; weight <= w; weight += wi) {
      taste += heap.front().second;
      tastes.emplace_back(weight, taste);
      pop_heap(heap.begin(), heap.end(), heapComparator);
      auto& t = heap.back();
      t.second -= dishes[t.first].second;
      push_heap(heap.begin(), heap.end(), heapComparator);
    }
    for (int i = w; i >= 0; i--) {
      bool possible = discretePossible[i];
      int64_t bestTaste = discreteTastes[i];
      for (const auto& t: tastes) {
        int weight = t.first;
        if (weight > i) {
          break;
        } else if (discretePossible[i - weight]) {
          int64_t newTaste = discreteTastes[i - weight] + t.second;
          if (!possible || newTaste > bestTaste) {
            bestTaste = newTaste;
            possible = true;
          }
        }
      }
      discretePossible[i] = possible;
      discreteTastes[i] = bestTaste;
    }
  }
}

void solveConcrete() {
  concreteTastes.resize(w + 1);

  int nextWeight = 0;
  double weight = 0;
  double total = 0;
  vector<double> deltas;
  for (auto it = concreteDishes.rbegin(); it != concreteDishes.rend(); ++it) {
    int t = it->first;
    const auto& d = it->second;
    deltas.insert(deltas.end(), d.begin(), d.end());
    double rate = 0;
    if (find(deltas.begin(), deltas.end(), 0) == deltas.end()) {
      for (int delta: deltas) {
        rate += 1.0 / delta;
      }
      rate = 1 / rate;
    }
    bool hasTarget;
    double target;
    auto nextIt = it;
    ++nextIt;
    if (nextIt == concreteDishes.rend()) {
      hasTarget = false;
    } else {
      hasTarget = true;
      target = nextIt->first;
    }
    double currentMargin = t;
    for (; nextWeight <= w; nextWeight++) {
      double nextMargin = currentMargin - rate * (nextWeight - weight);
      if (hasTarget && nextMargin < target) {
        double aWeight = (currentMargin - target) / rate;
        total += aWeight * (currentMargin + target) * 0.5;
        weight += aWeight;
        break;
      }
      total += (currentMargin + nextMargin) * 0.5 * (nextWeight - weight);
      concreteTastes[nextWeight] = total;
      currentMargin = nextMargin;
      weight = nextWeight;
    }
    if (nextWeight > w) {
      break;
    }
  }
}

int main() {
  cin.sync_with_stdio();
  cout << fixed << setprecision(9);

  readInput();
  if (!discreteDishes.empty()) {
    solveDiscrete();
  }
  if (!concreteDishes.empty()) {
    solveConcrete();
  }
  if (discreteDishes.empty()) {
    if (concreteDishes.empty()) {
      cout << "impossible" << endl;
    } else {
      cout << concreteTastes[w] << endl;
    }
  } else if (concreteDishes.empty()) {
    if (discretePossible[w]) {
      cout << discreteTastes[w] << endl;
    } else {
      cout << "impossible" << endl;
    }
  } else {
    double best = concreteTastes[w];
    for (int i = 1; i <= w; i++) {
      if (discretePossible[i]) {
        best = max(best, discreteTastes[i] + concreteTastes[w - i]);
      }
    }
    cout << best << endl;
  }
  quick_exit(0);
}
