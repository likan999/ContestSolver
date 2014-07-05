#include <cstdint>
#include <iostream>
#include <utility>

using namespace std;

int* M1 = new int[1024*1024];
int* M2 = new int[1024*1024];

int getM1(int m, int v) {
  return M1[(m<<10) + v];
}

int getM2(int m, int v) {
  return M2[(m<<10) + v];
}

void setM1(int m, int v, int value) {
  M1[(m<<10) + v] = value;
}

void setM2(int m, int v, int value) {
  M2[(m<<10) + v] = value;
}

int compute(int n) {
  int step = n + 1;
  for (int s = 1; s <= step; s++) {
    for (int m = 1; m <= s; m++) {
      for (int v = 0; v <= n; v++) {
        int value = getM2(m - 1, v) + s - m + 1;
        if (value >= step) value = 0;
        value = getM1(m - 1, value);
        setM2(m, v, value);
      }
    }
    int value = getM2(s, 0);
    int v;
    for (v = 1; v <= n && getM2(s, v) == value; v++);
    if (v > n) return value;
    swap(M1, M2);
  }
  return getM1(step, 0);
}

int main() {
  int64_t sum = 0;
  for (int v = 0; v <= 1000; v++) {
    setM1(0, v, v);
    setM2(0, v, v);
  }
  for (int n = 1; n <= 1000; n++) {
    int Mn = compute(n);
    //cout << n << ' ' << Mn << endl;
    sum += Mn * Mn * Mn;
  }
  cout << "Sum = " << sum << endl;
}
