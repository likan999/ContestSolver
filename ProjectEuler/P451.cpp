#include <algorithm>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;

vector<int> computeFirstDividor(int N);
vector<int> computePrimes(const vector<int>& firstDividor);

int N = 20000000;
const vector<int> FirstDividor = computeFirstDividor(N);

vector<int> computeFirstDividor(int N) {
  vector<int> firstDividor(N + 1);
  iota(firstDividor.begin(), firstDividor.end(), 0);
  for (int i = 2, square = 4; square <= N; square += 1 + 2 * i++) {
    if (firstDividor[i] != i) {
      continue;
    }
    for (int j = i * 2; j <= N; j += i) {
      if (firstDividor[j] == j) {
        firstDividor[j] = i;
      }
    }
  }
  return firstDividor;
}

void factor(int n, int& factorsOfTwo, vector<int>& coprimes) {
  for (factorsOfTwo = 0; n % 2 == 0; ++factorsOfTwo, n /= 2);
  coprimes.clear();
  int lastDividor = 0;
  while (n > 1) {
    int dividor = FirstDividor[n];
    if (dividor != lastDividor) {
      coprimes.emplace_back(dividor);
      lastDividor = dividor;
    } else {
      coprimes.back() *= dividor;
    }
    n /= dividor;
  }
}

int64_t mul64(int m, int n) {
  return static_cast<int64_t>(m) * static_cast<int64_t>(n);
}

template <typename T>
int modulo(T m, int n) {
  m %= n;
  return m < 0 ? m + n : m;
}

template <typename T>
void modulo(vector<T>& ms, int n) {
  for (auto& m : ms) {
    m = modulo(m, n);
  }
}

int inversion(int m, int n) {
  int t0 = 0, r0 = n;
  int t1 = 1, r1 = m;
  while (r1 != 1) {
    tie(t0, r0, t1, r1) = make_tuple(t1, r1, (t0 - mul64(r0 / r1, t1)) % n, r0 % r1);
  }
  return t1 < 0 ? t1 + n : t1;
}

void solveOdd(int base, const vector<int>& coprimes, vector<int>& solutions) {
  solutions = { 0 };
  for (int p : coprimes) {
    int part = mul64(inversion(base / p % p, p), base / p) % base;
    size_t size = solutions.size();
    solutions.resize(2 * size);
    for (size_t i = 0; i < size; i++) {
      tie(solutions[i], solutions[i + size]) = make_pair(solutions[i] - part, solutions[i] + part);
    }
  }
  modulo(solutions, base);
}

int solve(int odd, int factorsOfTwo, const vector<int>& coprimes, vector<int>& solutions) {
  bool hasOdd = !coprimes.empty();
  if (hasOdd) {
    solveOdd(odd, coprimes, solutions);
  }
  bool hasEven = factorsOfTwo > 0;
  int evenBase = 1;
  if (hasEven) {
    evenBase = factorsOfTwo > 3 ? 1 << (factorsOfTwo - 1) : 2;
  }
  if (!hasOdd) {
    return evenBase == 2 ? 3 : evenBase - 1;
  }
  int base = evenBase * odd;
  if (hasEven) {
    if (evenBase != 2) {
      size_t size = solutions.size();
      solutions.resize(size * 2);
      for (size_t i = 0; i < size; i++) {
        int64_t a = mul64(inversion(evenBase, odd), evenBase) % base;
        int64_t b = mul64(inversion(odd, evenBase), odd) % base;
        tie(solutions[i], solutions[i + size]) = make_pair((a * solutions[i] - b) % base, (a * solutions[i] + b) % base);
      }
    } else {
      for (auto& solution: solutions) {
        if (solution % 2 == 0) {
          solution += odd;
        }
      }
    }
  }
  modulo(solutions, base);
  sort(solutions.begin(), solutions.end());
  return solutions[1];
}

int main() {
  vector<int> coprimes;
  int factorsOfTwo;
  vector<int> solutions;
  int64_t sum = 0;
  for (int i = 3; i <= N; i++) {
    factor(i, factorsOfTwo, coprimes);
    int n = solve(i >> factorsOfTwo, factorsOfTwo, coprimes, solutions);
    sum += i - n;
    if (mul64(n, n) % i != 1) {
      cerr << "solve(" << i << ")=" << n << endl;
    }
  }
  cout << sum << endl;
}
