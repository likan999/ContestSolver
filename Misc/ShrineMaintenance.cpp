#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

const double PI = 3.1415926535897932384626;

vector<double> chordLengthCache;
vector<double> partialSums;
vector<double> distances;

template <typename Vector>
void initializeVector(Vector& v, size_t N) {
  v.clear();
  v.resize(N);
}

double getChordLength(int i, int N) {
  if (chordLengthCache[i] != 0) {
    return chordLengthCache[i];
  }
  return chordLengthCache[i] = 2 * sin(PI * i / N);
}

void compute(const double* partialSums, int n, int W) {
  distances.resize(n);
  for (int i = 0; i < n; i++) {
    distances[i] = partialSums[i] - partialSums[0];
  }
  for (int w = 1; w < W; w++) {
    int j = n - 1;
    for (int i = n - 1; i >= w; i--) {
      double best = numeric_limits<double>::max();
      double current;
      int k;
      for (k = j; k >= w && (current = max(distances[k - 1], partialSums[i] - partialSums[k])) <= best; k--) {
        best = current;
      }
      j = k + 1;
      distances[i] = best;
    }
  }
}

bool compute() {
  int W, N, D;
  cin >> W >> N >> D;
  if (W == 0) {
    return false;
  }
  vector<bool> isSacred(N);
  for (int i = 0; i < D; i++) {
    int divider;
    cin >> divider;
    if (!isSacred[divider]) {
      for (int j = 0; j < N; j += divider) {
        isSacred[j] = true;
      }
    }
  }
  vector<int> dividers;
  for (int i = 0; i < N; i++) {
    if (isSacred[i]) {
      dividers.emplace_back(i);
    }
  }
  int n = dividers.size();
  if (n <= W) {
    cout << 2000.0 << '\n';
    return true;
  }
  chordLengthCache.clear();
  chordLengthCache.resize(N / 2 + 1);
  partialSums.clear();
  partialSums.reserve(n * 2);
  partialSums.emplace_back(0);
  double sum = 0;
  for (int i = 0; i < n - 1; i++) {
    sum += getChordLength(dividers[i + 1] - dividers[i], N);
    partialSums.emplace_back(sum);
  }
  sum += getChordLength(N - dividers.back(), N);
  partialSums.emplace_back(sum);
  for (int i = 0; i < n - 1; i++) {
    sum += getChordLength(dividers[i + 1] - dividers[i], N);
    partialSums.emplace_back(sum);
  }
  double best = numeric_limits<double>::max();
  for (int i = 0; i < n && best >= partialSums[i]; i++) {
    compute(&partialSums[i], n, W);
    best = min(best, distances[n - 1]);
  }
  cout << (2 + best) * 1000 << '\n';
  return true;
}

int main() {
  istream::sync_with_stdio(false);
  cout << setprecision(1) << fixed;
  while (compute());
  cout << flush;
  quick_exit(0);
}
