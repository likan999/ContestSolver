#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int n;
vector<vector<int>> options;
vector<vector<int>> distances;

void readInput() {
  cin >> n;
  options.resize(n);
  string s;
  for (int i = 0; i < n; i++) {
    vector<int>& option = options[i];
    int m;
    cin >> m;
    option.resize(m);
    for (int j = 0; j < m; j++) {
      cin >> s;
      int choice = 0;
      for (char c : s) {
        choice |= (1 << (c - 'a'));
      }
      option[j] = choice;
    }
  }
}

void solve() {
  distances.resize(n);
  for (int i = 0; i < n; i++) {
    vector<int>& distance = distances[i];
    distance.resize(n);
    fill(distance.begin(), distance.end(), -1);
    distance[i] = 0;
    int reachable = 1 << i;
    bool changed = true;
    int current = 0;
    while (changed) {
      ++current;
      int newReachable = 0;
      for (int j = 0; j < n; j++) {
        if (distance[j] == -1) {
          vector<int>& option = options[j];
          bool win = any_of(option.begin(), option.end(), [reachable](int choice) { return (choice & reachable) == choice; });
          if (win) {
            distance[j] = current;
            newReachable |= (1 << j);
          }
        }
      }
      changed = newReachable;
      reachable |= newReachable;
    }
  }
}

void printOutput() {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (j > 0) cout << ' ';
      cout << distances[j][i];
    }
    cout << endl;
  }
}

int main() {
  readInput();
  solve();
  printOutput();
}
