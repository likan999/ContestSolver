#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

vector<int> primesUnder(int n) {
  vector<int> primes;
  vector<bool> isPrime(n + 1, true);
  for (int i = 2; i <= n; i++) {
    if (isPrime[i]) {
      primes.emplace_back(i);
      if (i * i <= n) {
        for (int j = 2 * i; j <= n; j += i) {
          isPrime[j] = 0;
        }
      }
    }
  }
  return primes;
}

int powmod(int base, int exponent, int m) {
  if (exponent == 0) {
    return 1;
  } else {
    int x = __builtin_clz(exponent);
    if (!__builtin_expect(x > 0, 1)) {
      cerr << "exponent is negative" << endl;
      exit(1);
    }
    int64_t mod = 1;
    for (int mask = 1 << (31 - x); mask > 0; mask >>= 1) {
      mod = mod * mod % m;
      if (exponent & mask) {
        mod = mod * base % m;
      }
    }
    return mod;
  }
}

int compute(int n, int m) {
  vector<int> primes = primesUnder(n);
  int64_t mod = 1;
  for (int prime: primes) {
    int exponent = 0;
    for (int i = n / prime; i > 0; exponent += i, i /= prime);
    mod = (1 + powmod(prime, 2 * exponent, m)) * mod % m;
  }
  return mod;
}

int main() {
  cout << compute(4, 1000000009) << endl;
  cout << compute(4, 19) << endl;
  cout << compute(100000000, 1000000009) << endl;
}
