#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>

using namespace std;

double square(double x) {
  return x * x;
}

double g(int64_t n, int64_t i, int m) {
  double base = 2 * (square((i - 1) / (double)n) + square((n - i) / (double)n)) - 1;
  double p = pow(abs(base), m);
  return m % 2 == 0 || base >= 0 ? p : -p;
}

double f(int64_t n, int m) {
  double total = 0;
  int64_t left = 1, right = n;
  for (; right > left; ++left, --right) {
    total += g(n, left, m);
    if ((left & (left - 1)) == 0) {
      cout << "INFO left=" << left << " total=" << total << endl;
    }
  }
  if (left == right) {
    total += 0.5 * g(n, left, m);
  }
  return total + 0.5 * n;
}

int main() {
  cout << fixed << setprecision(10);
  cout << f(3, 1) << endl;
  cout << f(3, 2) << endl;
  cout << f(10, 4) << endl;
  cout << f(100, 10) << endl;
  cout << f(10000000000LL, 4000) << endl;
}
