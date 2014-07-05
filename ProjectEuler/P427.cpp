#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include <omp.h>

using namespace std;

int inverse(int n, int m) {
  n %= m;
  // Invariant a = n * x (mod m), b = n * y (mod m)
  int64_t x = 0, a = m;
  int64_t y = 1, b = n;
  while (b > 1) {
    int64_t p = a / b;
    int64_t q = a % b;
    // q = a - b * p = (x - y * p) * n (mod m)
    int64_t z = (x - y * p) % m;
    x = y, a = b;
    y = z, b = q;
  }
  return y < 0 ? y + m : y;
}

int64_t mul64(int x, int y) {
  return static_cast<int64_t>(x) * static_cast<int64_t>(y);
}

int compute(int k, int n, int m, int* S) {
  fill(&S[1], &S[k + 1], 0);
  S[0] = 1;
  int current = 0, previous = k;
  for (int i = 1; i <= n; i++) {
    previous = current;
    current = current < k ? current + 1 : 0;
    S[current] = (mul64(n, S[previous]) - mul64(n - 1, S[current])) % m;
  }
  return S[current] - S[previous];
}

int compute(int n, int m) {
  vector<unique_ptr<int[]>> S;
  int64_t total = 0;
  #pragma omp parallel shared(S, total)
  {
    #pragma omp master
    {
      // Pre-allocated array S
      int numThreads = omp_get_num_threads();
      S.resize(numThreads);
      for (auto& s: S) {
        s.reset(new int[n + 1]);
      }
    }
    #pragma omp barrier
    #pragma omp for reduction(+: total)
    for (int k = 1; k < n; k++) {
      int threadId = omp_get_thread_num();
      total += compute(k, n, m, S[threadId].get());
      if (!(k & (k - 1))) {
        #pragma omp critical
        {
          cout << "Finished " << k << endl;
        }
      }
    }
  }
  total = (mul64(n, compute(n, n, m, S[0].get())) - total) % m;
  total = mul64(inverse(n - 1, m), mul64(n, total) % m) % m;
  return total < 0 ? total + m : total;
}

int main() {
  int M = 1000000009;
  cout << compute(3, M) << endl;
  cout << compute(7, M) << endl;
  cout << compute(11, M) << endl;
  cout << compute(7500000, M) << endl;
}
