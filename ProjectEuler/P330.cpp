#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

const int N = 77777777;

int64_t normalize(int64_t n) {
  n %= N;
  if (n < 0) n += N;
  return n;
}

int compute(int n) {
  vector<int64_t> multiplier = { 1 };
  for (int64_t i = n, m = 1; m != 0 && i >= 1; i--) {
    m = normalize(m * i);
    multiplier.emplace_back(m);
  }
  vector<int64_t> v(138, 0);
  v[1] = 1;
  int64_t sum = multiplier.size() > n ? multiplier[n] : 0;
  for (int i = 1; i <= n; i++) {
    int64_t S = 0;
    for (int j = 137; j > 0; j--) {
      v[j] = normalize(2 * (j - 1) * v[j - 1] - j * v[j]);
      S = normalize(S + v[j]);
    }
    if (i + multiplier.size() > n) {
      sum = normalize(sum + multiplier[n - i] * S);
    }
  }
  return sum;
}

int main() {
  // for (int i = 0; i <= 10; i++) {
  //   cout << i << ' ' << compute(i) << endl;
  // }
  for (int i = 1; i <= 1000000000; i *= 10) {
    cout << i << ' ' << compute(i) << endl;
  }
}
