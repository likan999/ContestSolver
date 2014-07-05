#include <cstdint>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include <gmpxx.h>

using namespace std;

const bool bools[] = { false, true };

mpz_class compute(const mpz_class& N) {
  map<pair<mpz_class, mpz_class>, mpz_class> found;
  vector<pair<const pair<mpz_class, mpz_class>, mpz_class>*> pending;
  for (mpz_class x = 1, S = 9; S <= N; ++x, S = x * (1 + 8 * x * x)) {
    auto res = found.insert(make_pair(make_pair(x, 4 * x * x), S));
    pending.emplace_back(&*res.first);
  }

  while (!pending.empty()) {
    auto p = pending.back();
    pending.pop_back();
    const auto& S = p->second;
    for (auto swap: bools) {
      auto x = swap ? &p->first.second : &p->first.first;
      auto y = swap ? &p->first.first : &p->first.second;
      mpz_class xy = (*x) * (*y) * 2;
      for (auto negate: bools) {
        auto m = S;
        if (negate) m -= xy;
        else m += xy;
        mpz_class nx = *x;
        mpz_class ny = 4 * m * (*x);
        if (negate) ny -= (*y);
        else ny += (*y);
        if (nx > 0 && ny > 0) {
          mpz_class nS = 2 * nx * ny + m;
          if (nS <= N) {
            auto res = found.insert(make_pair(nx <= ny ? make_pair(nx, ny) : make_pair(ny, nx), nS));
            if (res.second) {
              pending.emplace_back(&*res.first);
            }
          }
        }
      }
    }
  }

  mpz_class sum = 0;
  for (const auto& p: found) {
    sum += p.second;
  }
  return sum;
}

int main(int argc, char* argv[]) {
  mpz_class N(10000000000);
  if (argc == 2) {
    if (N.set_str(argv[1], 10) == -1) {
      cerr << argv[1] << " is not a valid integer" << endl;
      return 1;
    }
  }
  cout << compute(N).get_str() << endl;
}
