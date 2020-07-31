#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
using namespace std;

template <typename Int>
struct Complex {
  using value_type = Int;

  Int x = 0;
  Int y = 0;

  constexpr static Complex<Int> one() {
    return {1};
  }

  constexpr Complex<Int> conjugate() const {
    return {x, -y};
  }

  constexpr Complex<Int> scale(Int m) const {
    return {x * m, y * m};
  }

  constexpr Int absSq() const {
    return x * x + y * y;
  }
};

template <typename Int>
Complex<Int> operator*(const Complex<Int>& lhs, const Complex<Int>& rhs) {
  return {lhs.x * rhs.x - lhs.y * rhs.y, lhs.x * rhs.y + lhs.y * rhs.x};
}

template <int N, typename Int = int8_t>
using Exponents = array<Int, N>;

template <typename Exp, typename F>
void forEachExponent(const Exp& exp, F f) {
  uint64_t n = 1;
  for (const auto e : exp) n *= (e + 1);
  for (auto cur = exp; n > 0; --n) {
    f(cur);
    for (int i = 0; i < exp.size(); ++i) {
      if (cur[i] == 0) {
        cur[i] = exp[i];
      } else {
        --cur[i];
        break;
      }
    }
  }
}


template <typename PrimesArray, typename Exp>
uint64_t numPythagoreanQuadrilaterals(const PrimesArray& primes, Exp exp) {
  using ComplexT = typename PrimesArray::value_type;
  using Int = typename ComplexT::value_type;
  constexpr auto N = primes.size();
  static_assert(exp.size() == N);

  uint64_t n = 1;
  for (const auto e : exp) n *= (e + 1);
  uint64_t m = 4 * n;
  uint64_t out = n * (2 * n - 1) * (4 * n - 3);
  array<ComplexT, N> squares;
  array<vector<ComplexT>, N> square_powers;
  for (int i = 0; i < N; ++i) {
    auto sq = squares[i] = primes[i] * primes[i];
    auto& powers = square_powers[i];
    powers.emplace_back(ComplexT::one());
    for (int j = 1; j <= exp[i]; ++j) {
      powers.emplace_back(powers.back() * sq);
    }
    auto norm_sq = primes[i].absSq();
    Int multiplier = 1;
    for (auto it = powers.rbegin() + 1; it != powers.rend(); ++it) {
      (*it) = it->scale(multiplier *= norm_sq);
    }
  }
  forEachExponent(exp, [&](const auto& sum_exp) {
    uint64_t multiplier = 1;
    for (int i = 0; i < N; ++i) {
      if (sum_exp[i] != exp[i]) multiplier *= 2;
    }
    vector<Int> xs, ys;
    forEachExponent(sum_exp, [&](const auto& cur_exp) {
      bool all_zeros = true;
      auto value = ComplexT::one();
      for (int i = 0; i < N; ++i) {
        auto e = 2 * cur_exp[i] - sum_exp[i];
        if (all_zeros && e < 0) return;
        if (e > 0) all_zeros = false;
        const auto& powers = square_powers[i];
        value = value * (e < 0 ? powers[-e].conjugate() : powers[e]);
      }
      ys.emplace_back(value.y);
      if (!all_zeros) {
        xs.emplace_back(value.x);
        xs.emplace_back(-value.x);
        ys.emplace_back(-value.y);
      }
    });
    auto count = [](vector<Int>& v) {
      sort(v.begin(), v.end());
      uint64_t pairs = 0;
      for (auto lit = v.begin(); lit != v.end();) {
        auto x = *(lit++);
        auto rit = lower_bound(lit, v.end(), -x);
        uint64_t c = rit - lit;
        if (c == 0) break;
        pairs += c;
      }
      return 2 * pairs + lower_bound(v.begin(), v.end(), 0) - v.begin();
    };
    out += 4 * multiplier * (count(xs) + count(ys));
  });
  return out;
}

template <typename PrimesArray, typename Exp>
uint64_t totalPythagoreanQuadrilateralsForDividers(const PrimesArray& primes, Exp exp) {
  uint64_t total = 0;
  forEachExponent(exp, [&](const auto& cur_exp) {
    total += numPythagoreanQuadrilaterals(primes, cur_exp);
  });
  return total;
}

constexpr Complex<int64_t> kComplex5{2, 1};
constexpr Complex<int64_t> kComplex13{3, 2};
constexpr Complex<int64_t> kComplex17{4, 1};
constexpr Complex<int64_t> kComplex29{5, 2};
constexpr Complex<int64_t> kComplex37{6, 1};
constexpr Complex<int64_t> kComplex41{5, 4};
constexpr Complex<int64_t> kComplex53{7, 2};
constexpr Complex<int64_t> kComplex61{6, 5};

int main() {
  cout << numPythagoreanQuadrilaterals(array{kComplex5}, Exponents<1>{0}) << endl;
  cout << numPythagoreanQuadrilaterals(array{kComplex5}, Exponents<1>{1}) << endl;
  cout << numPythagoreanQuadrilaterals(array{kComplex5}, Exponents<1>{2}) << endl;
  cout << totalPythagoreanQuadrilateralsForDividers(array{kComplex5, kComplex13}, array<int8_t, 2>{2, 1}) << endl;
  cout << totalPythagoreanQuadrilateralsForDividers(
      array{kComplex5, kComplex13, kComplex17, kComplex29, kComplex37, kComplex41, kComplex53, kComplex61},
      array<int8_t, 8>{6, 3, 2, 1, 1, 1, 1, 1}) << endl;
}
