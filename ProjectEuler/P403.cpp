#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

int S = 0;
int U[24] = { 0 };
int V[24] = { 0 };
int W[24] = { 0 };
const int twoPowerNine = 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
const int fivePowerNine = 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5;

int64_t mul64(int x, int y) {
  return static_cast<int64_t>(x) * static_cast<int64_t>(y);
}

int gcd(int a, int b) {
  if (a < b) swap(a, b);
  while (b > 0) {
    int r = a % b;
    a = b;
    b = r;
  }
  return a;
}

int reciprocal(int n, int base) {
  n %= base;
  int prev = 1;
  int current = n;
  while (current != 1) {
    int next = mul64(current, n) % base;
    prev = current;
    current = next;
  }
  return prev;
}

int fastmod(int x, int p) {
  return x >= p ? x - p : x;
}

int compute(int a, int b, int c) {
  return gcd(24, gcd(6 * a + 12, gcd(2 * b + 2, 1 + a + b + c)));
}

void init() {
  for (int i = 1; i <= 24; i++) {
    for (int j = 1; j <= 24; j++) {
      for (int k = 1; k <= 24; k++) {
        int x = compute(i, j, k);
        int y = compute(j, k, i);
        int z = compute(k, i, j);
        S += x;
        for (int m = k; m < 24; m++) {
          U[m] += x + y + z;
        }
        for (int m = max(j, k); m < 24; m++) {
          V[m] += x + y + z;
        }
        for (int m = max(max(i, j), k); m < 24; m++) {
          W[m] += x;
        }
      }
    }
  }
}

int64_t computeS(int64_t N, int64_t p) {
  int64_t n = N / 24 % p;
  int64_t m = N % 24;
  return (((S % p * n + U[m]) % p * n + V[m]) % p * n + W[m]) % p;
}

vector<int64_t> computeFibonacci(int64_t p) {
  int64_t m = 24 * p;
  int64_t prev = 1;
  int64_t current = 0;
  vector<int64_t> values;
  do {
    values.emplace_back(computeS(current, p));
    int64_t next = fastmod(prev + current, m);
    prev = current;
    current = next;
  } while (!(prev == 1 && current == 0));
  return values;
}

int64_t compute(int64_t n, int64_t p) {
  auto fibonacci = computeFibonacci(p);
  auto t = fibonacci.size();
  cout << t << endl;
  int64_t q = n / t % p;
  auto r = n % t;
  int64_t total = 0, residual = 0;
  for (size_t i = 0; i < t; i++) {
    total = fastmod(total + fibonacci[i], p);
    if (i == r) residual = total;
  }
  return (total * q + residual + p - 2) % p;
}

int64_t compute(int64_t n) {
  int64_t x = compute(n, twoPowerNine);
  int64_t y = compute(n, fivePowerNine);
  return (x * reciprocal(fivePowerNine, twoPowerNine) * fivePowerNine + y * reciprocal(twoPowerNine, fivePowerNine) * twoPowerNine) % 1000000000;
}

int main() {
  init();
  cout << compute(1234567890123L) << endl;
  // cout << computeS(0, 10000) << endl;
}
