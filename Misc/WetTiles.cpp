#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <vector>

using namespace std;

vector<int> tiles;
deque<int> queue;
vector<int> leaks;
int numWets;

void setWet(int n, int t) {
  int& tile = tiles[n];
  if (tile == 0) {
    tile = t;
    queue.emplace_back(n);
    ++numWets;
  }
}

bool compute() {
  int X, Y, T, L, W;
  cin >> X >> Y >> T >> L >> W;
  if (X == -1) {
    return false;
  }
  tiles.clear();
  tiles.resize(X * Y);
  queue.clear();
  leaks.clear();
  numWets = 0;
  for (int i = 0; i < L; i++) {
    int x, y;
    cin >> x >> y;
    leaks.emplace_back((x - 1) * Y + y - 1);
  }
  for (int i = 0; i < W; i++) {
    int x1, y1, x2, y2;
    cin >> x1 >> y1 >> x2 >> y2;
    int step = x1 < x2 ? Y : x1 > x2 ? -Y : 0;
    step += y1 < y2 ? 1 : y1 > y2 ? -1 : 0;
    int numWalls = max(abs(x1 - x2), abs(y1 - y2)) + 1;
    for (int j = 0, n = (x1 - 1) * Y + y1 - 1; j < numWalls; j++, n += step) {
      tiles[n] = -1;
    }
  }
  for (int leak: leaks) {
    setWet(leak, 1);
  }
  while (!queue.empty()) {
    int head = queue.front();
    queue.pop_front();
    int tile = tiles[head] + 1;
    if (tile > T) {
      break;
    }
    int x = head / Y;
    int y = head % Y;
    if (x > 0) {
      setWet(head - Y, tile);
    }
    if (x + 1 < X) {
      setWet(head + Y, tile);
    }
    if (y > 0) {
      setWet(head - 1, tile);
    }
    if (y + 1 < Y) {
      setWet(head + 1, tile);
    }
  }
  cout << numWets << '\n';
  return true;
}

int main() {
  istream::sync_with_stdio(false);
  while (compute());
  cout << flush;
  quick_exit(0);
}
