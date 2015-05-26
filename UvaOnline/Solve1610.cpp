#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int MAX_LENGTH = 30;

string compute(const string& a, string& b) {
  if (b.back() == 'A') {
    b.pop_back();
  } else {
    --b.back();
    b.append(MAX_LENGTH - b.size(), 'Z');
  }

  size_t p = 0;
  for (; p < a.size() && p < b.size() && a[p] == b[p]; p++);
  string s = a.substr(0, p);
  if (p != a.size()) {
    s += a[p];
    if (p + 1 < a.size()) {
      ++s[p];
    }
  }
  return s;
}

int main() {
  while (true) {
    int n;
    cin >> n;
    if (n == 0) break;

    vector<string> names(n);
    for (int i = 0; i < n; i++) {
      cin >> names[i];
    }
    sort(names.begin(), names.end());
    cout << compute(names[n / 2 - 1], names[n / 2]) << endl;
  }
}
