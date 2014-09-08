#include <algorithm>
#include <iostream>
using namespace std;

typedef int Counts[10];
typedef int Digits[9];

int powerOfTen[10];
int borrow[10];

void init() {
  for (int i = 1, n = 1; i <= 9; i++, n *= 10) {
    powerOfTen[i] = n;
    borrow[i] = n + borrow[i - 1];
  }
}

int compute_digits(int n, Digits digits) {
  int length = 0;
  while (n > 0) {
    digits[length++] = n % 10;
    n /= 10;
  }
  return length;
}

void calculate(int n, Counts counts) {
  fill_n(counts, 10, 0);
  Digits digits;
  int length = compute_digits(n, digits);
  for (int i = length - 1; i >= 0; i--) {
    int d = digits[i];
    int q = powerOfTen[i + 1];
    int r = d * i * powerOfTen[i];
    n -= d * q;
    for (int j = 0; j < d; j++) {
      counts[j] += r + q;
    }
    counts[d] += r + n + 1;
    for (int j = d + 1; j < 10; j++) {
      counts[j] += r;
    }
  }
  counts[0] -= borrow[length];
}

int main() {
  init();
  istream::sync_with_stdio(false);
  int m, n;
  Counts countsN, countsM;
  while ((cin >> m >> n) && (m != 0 || n != 0)) {
    calculate(n, countsN);
    calculate(m - 1, countsM);
    for (int i = 0; i < 10; i++) {
      if (i > 0) {
        cout << ' ';
      }
      cout << (countsN[i] - countsM[i]);
    }
    cout << '\n';
  }
}
