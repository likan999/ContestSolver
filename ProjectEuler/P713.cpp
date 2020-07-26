#include <cstdint>
#include <iostream>

using namespace std;

uint64_t cliqueSize(uint64_t n) {
  return n * (n - 1) / 2;
}

uint64_t T(uint64_t N, uint64_t m) {
  // Reference: Turan's theorem
  // https://en.wikipedia.org/wiki/Tur%C3%A1n%27s_theorem
  uint64_t a = N / (m - 1), b = N % (m - 1);
  return cliqueSize(a + 1) * b + cliqueSize(a) * (m - 1 - b);
}

uint64_t L(uint64_t N) {
  uint64_t total = 0;
  for (uint64_t m = 2; m <= N; ++m) {
    total += T(N, m);
  }
  return total;
}

int main() {
  cout << L(1000) << endl;
  cout << L(10000000) << endl;
}

