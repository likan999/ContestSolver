#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

vector<int> computePrimes(int M) {
  vector<int> primes;
  vector<bool> isComposite(M + 1, false);
  for (int i = 2; i * i <= M; i++) {
    if (!isComposite[i]) {
      for (int j = 2 * i; j <= M; j += i) {
        isComposite[j] = true;
      }
    }
  }
  for (int i = 2; i <= M; i++) {
    if (!isComposite[i]) {
      primes.emplace_back(i);
    }
  }
  return primes;
}

vector<int> decomposite(int m, const vector<int>& primes) {
  vector<int> ret;
  for (int prime: primes) {
    if (prime * prime > m) {
      break;
    }
    if (m % prime == 0) {
      ret.emplace_back(prime);
      while (m % prime == 0) m /= prime;
    }
  }
  if (m > 1) {
    ret.emplace_back(m);
  }
  return ret;
}

int64_t mul64(int x, int y) {
  return static_cast<int>(x) * static_cast<int>(y);
}

int64_t compute(int64_t n) {
  int M = floor(sqrt(sqrt(static_cast<double>(n))));
  vector<int> primes = computePrimes(M);
  vector<bool> hasCommonFactor(M + 1, false);
  int64_t total = 0;
  for (int m = 1; m <= M; m++) {
    fill(hasCommonFactor.begin() + 1, hasCommonFactor.begin() + m, false);
    auto factors = decomposite(m, primes);
    for (int prime: factors) {
      for (int i = prime; i <= m; i += prime) {
        hasCommonFactor[i] = true;
      }
    }
    for (int j = m - 1; j * 2 > m; j -= 2) {
      if (!hasCommonFactor[j]) {
        int64_t x = mul64(m, m) - mul64(j, j) + mul64(m, j);
        int64_t y = 4 * mul64(m, j) + mul64(j, j) - mul64(m, m);
        if (x % 5 != 0 || y % 5 != 0) {
          total += n / (x * y);
        }
      }
    }
    for (int j = m % 2 == 0 ? 1 : 2; j * 3 < m; j += 2) {
      if (!hasCommonFactor[j]) {
        int64_t x = mul64(m, m) - mul64(j, j) - mul64(m, j);
        int64_t y = mul64(m, m) - mul64(j, j) + 4 * mul64(m, j);
        if (x % 5 != 0 || y % 5 != 0) {
          total += n / (x * y);
        }
      }
    }
  }
  return total;
}

int main() {
  cout << compute(1000) << endl;
  cout << compute(10000) << endl;
  cout << compute(1000000) << endl;
  cout << compute(100000000000000000) << endl;
}
