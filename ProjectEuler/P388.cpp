#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <unordered_map>
#include <utility>

#include <gmpxx.h>

using namespace std;

const int64_t N = 10000000000;
int smallCacheSize = (int) sqrt(N);
mpz_class* smallCache = new mpz_class[smallCacheSize + 1];
unordered_map<int64_t, mpz_class> bigCache;

void int64ToMpz(mpz_class& v, int64_t i) {
  if (i < (1L<<32)) {
    v = (uint32_t)i;
  } else {
    char buf[25];
    sprintf(buf, "%lld", i);
    v.set_str(buf, 10);
  }
}

void computeSmallCache() {
  for (int n = 1; n <= smallCacheSize; n++) {
    mpz_class& sum = smallCache[n];
    sum = n + 1;
    sum = sum * sum * sum - 1;
    int m = (int)sqrt(2 * n);
    for (int i = 2; i <= m; i++) {
      sum -= smallCache[n / i];
    }
    for (int i = n / m; i > 0; i--) {
      int count = n / i - max(m, n / (i + 1));
      if (count > 0) {
        sum -= smallCache[i] * count;
      }
    }
  }
}

const mpz_class& compute(int64_t n) {
  if (n <= smallCacheSize) {
    return smallCache[n];
  }
  auto result = bigCache.insert(make_pair(n, mpz_class()));
  if (result.second) {
    mpz_class& sum = result.first->second;
    int64ToMpz(sum, n + 1);
    sum = sum * sum * sum - 1;
    int m = (int) sqrt(2 * n);
    for (int i = 2; i <= m; i++) {
      sum -= compute(n / i);
    }
    for (int i = n / (m + 1); i > 0; i--) {
      int64_t count = n / i - max<int64_t>(m, n / (i + 1));
      if (count > 0) {
        mpz_class c;
        int64ToMpz(c, count);
        sum -= compute(i) * c;
      }
    }
  }
  return result.first->second;
}

int main() {
  computeSmallCache();
  cout << compute(N).get_str() << endl;
}
