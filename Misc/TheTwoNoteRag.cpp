#include <cstdint>
#include <iostream>
using namespace std;

int64_t powersOfFive[21];
int64_t powersOfTwo[21];
int64_t remainders[21];
int baseSolution[5] = { 0, 0, 1, 3, 2 };
int inverse[5] = { 0, 1, 3, 2, 4 };

void init() {
  int64_t powerOfFive = 1, powerOfTwo = 1, remainder = 0;
  for (int i = 0; i <= 20; i++, powerOfTwo *= 2, powerOfFive *= 5) {
    powersOfTwo[i] = powerOfTwo;
    powersOfFive[i] = powerOfFive;
    remainders[i] = remainder;
    if (remainder % 2 == 0) {
      remainder = remainder / 2 + powerOfFive;
    } else {
      remainder = (remainder + powerOfFive) / 2;
    }
  }
}

int64_t multiplyModulo(int64_t x, int64_t y, int64_t n) {
  return static_cast<__int128>(x) * static_cast<__int128>(y) % n;
}

int64_t powerModulo(int base, int64_t e, int64_t n) {
  int64_t result = 1;
  int64_t mask = static_cast<int64_t>(1) << 62;
  for (; mask != 0 && (e & mask) == 0; mask >>= 1);
  for (; mask != 0; mask >>= 1) {
    result = multiplyModulo(result, result, n);
    if (e & mask) {
      result = multiplyModulo(base, result, n);
    }
  }
  return result;
}

int64_t compute(int k, int64_t remainder) {
  if (k == 1) {
    return baseSolution[remainder % 5];
  }
  int64_t n = compute(k - 1, remainder % powersOfFive[k - 1]);
  int r = (remainder - powerModulo(2, n, powersOfFive[k])) / powersOfFive[k - 1];
  if (r < 0) r += 5;
  int c = (3 << (n % 4)) % 5;
  int m = r * inverse[c] % 5;
  return m * 4 * powersOfFive[k - 2] + n;
}

int main() {
  istream::sync_with_stdio(false);
  init();
  int n;
  cin >> n;
  for (int i = 1; i <= n; i++) {
    int k;
    cin >> k;
    cout << i << ' ' << k << ' ' << k + compute(k, remainders[k]) << '\n';
  }
}
