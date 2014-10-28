#include <algorithm>
#include <cstdlib>
#include <iostream>

using namespace std;

struct Factors {
  int size = 0;
  int factors[4];
};

Factors factorsTable[1001];
int minSteps[1001];

void init() {
  for (int i = 2; i <= 1000; i++) {
    if (factorsTable[i].size == 0) {
      for (int j = 2 * i; j <= 1000; j += i) {
        Factors& factors = factorsTable[j];
        factors.factors[factors.size++] = i;
      }
    }
  }
}

int compute(int s, int t) {
  if (s == t) {
    return 0;
  } else if (s > t || t == s + 1) {
    return -1;
  } else if (factorsTable[s].size == 0) {
    return -1;
  }
  minSteps[s] = 0;
  fill(minSteps + s + 1, minSteps + t + 1, -1);
  for (int i = s; i < t; i++) {
    int steps = minSteps[i] + 1;
    if (steps == 0) {
      continue;
    }
    const Factors& factors = factorsTable[i];
    for (int j = 0; j < factors.size; j++) {
      int k = i + factors.factors[j];
      if (k > t) {
        break;
      }
      if (minSteps[k] == -1 || minSteps[k] > steps) {
        minSteps[k] = steps;
      }
    }
  }
  return minSteps[t];
}

int main() {
  istream::sync_with_stdio(false);
  init();
  int s, t;
  for (int i = 1; (cin >> s >> t) && s != 0 && t != 0; i++) {
    cout << "Case " << i << ": " << compute(s, t) << '\n';
  }
  cout << flush;
  quick_exit(0);
}
