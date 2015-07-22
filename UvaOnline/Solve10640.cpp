#include <iostream>
using namespace std;

int solve(int a, int b) {
  if (3*a <= b) return -1;
  if (a >= b) return 1;
  int x = 8*a;
  int y = 3*a-b;
  int s = (x-1)/y+1;
  if (s%2 && (s*s-1)*y < s*x) ++s;
  return s - 3;
}

int main() {
  istream::sync_with_stdio(true);
  int t;
  cin >> t;
  for (int i = 1; i <= t; i++) {
    int a, b;
    cin >> a >> b;
    cout << "Case " << i << ": " << solve(a, b) << '\n';
  }
}
