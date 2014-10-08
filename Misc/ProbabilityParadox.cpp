#include <algorithm>
#include <iostream>
#include <limits>
#include <list>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

class Fraction {
 public:
  Fraction(int n = 0) : Fraction(n < 0, abs(n), 1) {}

  Fraction(int n, int d) {
    if (d == 0) {
      throw invalid_argument("Denominator is zero");
    }
    negative_ = (n != 0) && ((n >= 0) ^ (d >= 0));
    tie(numerator_, denominator_) = normalize(abs(n), abs(d));
  }

  Fraction(const Fraction&) = default;
  Fraction& operator=(const Fraction&) = default;

  friend Fraction operator+(const Fraction& a, const Fraction& b) {
    if (a.negative_ == b.negative_) {
      auto p = add(a.numerator_, a.denominator_, b.numerator_, b.denominator_);
      return Fraction(a.negative_, p.first, p.second);
    } else {
      auto p = subtract(a.numerator_, a.denominator_, b.numerator_, b.denominator_);
      return Fraction(a.negative_ ^ get<2>(p), get<0>(p), get<1>(p));
    }
  }

  friend Fraction operator-(const Fraction& a, const Fraction& b) {
    if (a.negative_ == b.negative_) {
      auto p = subtract(a.numerator_, a.denominator_, b.numerator_, b.denominator_);
      return Fraction(a.negative_ ^ get<2>(p), get<0>(p), get<1>(p));
    } else {
      auto p = add(a.numerator_, a.denominator_, b.numerator_, b.denominator_);
      return Fraction(a.negative_, p.first, p.second);
    }
  }

  friend Fraction operator*(const Fraction& a, const Fraction& b) {
    auto p = normalize(mul(a.numerator_, b.numerator_), mul(a.denominator_, b.denominator_));
    return Fraction(a.negative_ ^ b.negative_, safe_cast<uint32_t>(p.first), safe_cast<uint32_t>(p.second));
  }

  friend Fraction operator/(const Fraction& a, const Fraction& b) {
    if (b.numerator_ == 0) {
      throw invalid_argument("Divide by zero");
    }
    auto p = normalize(mul(a.numerator_, b.denominator_), mul(a.denominator_, b.numerator_));
    return Fraction(a.negative_ ^ b.negative_, safe_cast<uint32_t>(p.first), safe_cast<uint32_t>(p.second));
  }

  friend bool operator==(const Fraction& a, const Fraction& b) {
    return a.negative_ == b.negative_ && a.numerator_ == b.numerator_ && a.denominator_ == b.denominator_;
  }

  friend bool operator!=(const Fraction& a, const Fraction& b) {
    return !(a == b);
  }

  friend ostream& operator<<(ostream& os, const Fraction& f) {
    if (f.negative_) {
      os << '-';
    }
    os << f.numerator_;
    if (f.numerator_ != 0 && f.denominator_ != 1) {
      os << '/' << f.denominator_;
    }
    return os;
  }

 private:
  Fraction(bool negative, uint32_t n, uint32_t d) : negative_(n != 0 && negative), numerator_(n), denominator_(d) {
  }

  template <typename U, typename T>
  static U safe_cast(T t) {
    if (t > numeric_limits<U>::max()) {
      throw range_error("Out of range when doing type cast");
    }
    return static_cast<U>(t);
  }

  template <typename T>
  static pair<T, T> normalize(T numerator, T denominator) {
    T n = numerator, d = denominator;
    while (n != 0) {
      tie(n, d) = make_pair(d % n, n);
    }
    return { numerator / d, denominator / d };
  }

  template <typename T>
  static T add(T a, T b) {
    if (numeric_limits<T>::max() - a < b) {
      throw overflow_error("Overflows when doing addition");
    }
    return a + b;
  }

  template <typename T>
  static pair<T, bool> subtract(T a, T b) {
    bool negative = a < b;
    return { negative ? b - a : a - b, negative };
  }

  static uint64_t mul(uint32_t a, uint32_t b) {
    return static_cast<uint64_t>(a) * static_cast<uint64_t>(b);
  }

  static pair<uint32_t, uint32_t> add(uint32_t n1, uint32_t d1, uint32_t n2, uint32_t d2) {
    auto p = normalize(add(mul(n1, d2), mul(n2, d1)), mul(d1, d2));
    return { safe_cast<uint32_t>(p.first), safe_cast<uint32_t>(p.second) };
  }

  static tuple<uint32_t, uint32_t, bool> subtract(uint32_t n1, uint32_t d1, uint32_t n2, uint32_t d2) {
    uint64_t a = mul(n1, d2);
    uint64_t b = mul(n2, d1);
    auto p = subtract(a, b);
    auto q = normalize(p.first, mul(d1, d2));
    return make_tuple(safe_cast<uint32_t>(q.first), safe_cast<uint32_t>(q.second), p.second);
  }

 private:
  bool negative_;
  uint32_t numerator_;
  uint32_t denominator_;
};

int sizeOfPattern(int pattern) {
  return sizeof(unsigned int) * 8 - 1 - __builtin_clz(pattern);
}

Fraction compute(int patternA, int patternB) {
  int n = sizeOfPattern(patternA);
  unordered_map<int, int> patternToIndex;
  patternToIndex[1] = 1;
  for (int i = n - 1; i > 0; i--) {
    patternToIndex.emplace(patternA >> i, patternToIndex.size() + 1);
    patternToIndex.emplace(patternB >> i, patternToIndex.size() + 1);
  }
  int m = patternToIndex.size();
  vector<list<pair<int, Fraction>>> coefficients(m + 1);
  for (const auto& p : patternToIndex) {
    int pattern = p.first;
    auto& coeff = coefficients[p.second];
    for (int d = 0; d <= 1; d++) {
      int newPattern = (pattern << 1) + d;
      if (newPattern == patternA) {
        coeff.emplace_back(0, Fraction(1, 2));
      } else if (newPattern != patternB) {
        int size = sizeOfPattern(newPattern);
        for (int i = size; i >= 0; i--) {
          auto q = patternToIndex.find((newPattern & ((1 << i) - 1)) | (1 << i));
          if (q != patternToIndex.end()) {
            coeff.emplace_back(q->second, Fraction(1, 2));
            break;
          }
        }
      }
    }
    coeff.sort([] (const pair<int, Fraction>& left, const pair<int, Fraction>& right) { return left.first < right.first; });
  }
  for (int i = m; i > 0; i--) {
    auto& coeff = coefficients[i];
    if (!coeff.empty() && coeff.back().first == i) {
      Fraction divider = 1 - coeff.back().second;
      coeff.pop_back();
      for (auto& p : coeff) {
        p.second = p.second / divider;
      }
    }
    for (int j = 1; j < i; j++) {
      auto& c = coefficients[j];
      if (!c.empty() && c.back().first == i) {
        Fraction ci = c.back().second;
        c.pop_back();
        auto it1 = c.begin();
        auto it2 = coeff.begin();
        while (it1 != c.end() && it2 != coeff.end()) {
          if (it1->first == it2->first) {
            it1->second = it1->second + ci * it2->second;
            ++it1;
            ++it2;
          } else if (it1->first < it2->first) {
            ++it1;
          } else {
            c.emplace(it1, it2->first, ci * it2->second);
            ++it2;
          }
        }
        for (; it2 != coeff.end(); ++it2) {
          c.emplace(it1, it2->first, ci * it2->second);
        }
      }
    }
  }
  if (coefficients[1].empty() || coefficients[1].front().first != 0) {
    return 0;
  } else {
    return coefficients[1].front().second;
  }
}

int convert(const string& s) {
  int n = 1;
  for (char c : s) {
    n = (n << 1) + (c == 'T');
  }
  return n;
}

int main() {
  string patternA, patternB;
  while (true) {
    cin >> patternA;
    if (patternA == "$") {
      break;
    }
    cin >> patternB;
    cout << compute(convert(patternA), convert(patternB)) << endl;
  }
}
