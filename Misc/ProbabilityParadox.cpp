#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>
using namespace std;

class Fraction {
 public:
  Fraction(uint32_t n = 0) : Fraction(n, 1, false) {}

  Fraction(uint32_t n, uint32_t d) {
    if (d == 0) {
      throw invalid_argument("Denominator is zero");
    }
    tie(numerator_, denominator_) = normalize(n, d);
  }

  Fraction(const Fraction&) = default;
  Fraction& operator=(const Fraction&) = default;

  friend Fraction operator+(const Fraction& a, const Fraction& b) {
    auto p = normalize(add(mul(a.numerator_, b.denominator_), mul(b.numerator_, a.denominator_)), mul(a.denominator_, b.denominator_));
    return Fraction(safe_cast<uint32_t>(p.first), safe_cast<uint32_t>(p.second), false);
  }

  friend Fraction operator-(const Fraction& a, const Fraction& b) {
    uint64_t p = mul(a.numerator_, b.denominator_);
    uint64_t q = mul(b.numerator_, a.denominator_);
    if (p < q) {
      throw range_error("Negative result when doing subtraction");
    }
    auto r = normalize(p - q, mul(a.denominator_, b.denominator_));
    return Fraction(safe_cast<uint32_t>(r.first), safe_cast<uint32_t>(r.second), false);
  }

  friend Fraction operator*(const Fraction& a, const Fraction& b) {
    auto p = normalize(mul(a.numerator_, b.numerator_), mul(a.denominator_, b.denominator_));
    return Fraction(safe_cast<uint32_t>(p.first), safe_cast<uint32_t>(p.second), false);
  }

  friend Fraction operator/(const Fraction& a, const Fraction& b) {
    if (b.numerator_ == 0) {
      throw invalid_argument("Divide by zero");
    }
    auto p = normalize(mul(a.numerator_, b.denominator_), mul(a.denominator_, b.numerator_));
    return Fraction(safe_cast<uint32_t>(p.first), safe_cast<uint32_t>(p.second), false);
  }

  friend bool operator==(const Fraction& a, const Fraction& b) {
    return a.numerator_ == b.numerator_ && a.denominator_ == b.denominator_;
  }

  friend bool operator!=(const Fraction& a, const Fraction& b) {
    return !(a == b);
  }

  friend ostream& operator<<(ostream& os, const Fraction& f) {
    os << f.numerator_;
    if (f.numerator_ != 0 && f.denominator_ != 1) {
      os << '/' << f.denominator_;
    }
    return os;
  }

 private:
  Fraction(uint32_t n, uint32_t d, bool /*dummy*/) : numerator_(n), denominator_(d) {
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
      throw overflow_error("Overflow when doing addition");
    }
    return a + b;
  }

  static uint64_t mul(uint32_t a, uint32_t b) {
    return static_cast<uint64_t>(a) * static_cast<uint64_t>(b);
  }

 private:
  uint32_t numerator_;
  uint32_t denominator_;
};

template <typename Value>
class IntMap {
 private:
  vector<pair<int, Value>> keyValuePairs_;
  unique_ptr<int[]> keyToIndex_;
  int bufSize_ = 0;

 public:
  void init(int maxSize) {
    if (keyToIndex_ && bufSize_ < maxSize) {
      keyToIndex_.reset();
    }
    if (!keyToIndex_) {
      bufSize_ = max(bufSize_ * 2, maxSize);
      keyToIndex_.reset(new int[bufSize_]);
    }
    keyValuePairs_.clear();
  }

  bool put(int key, const Value& value) {
    if (key < 0 || key >= bufSize_) {
      throw out_of_range("Key out of range.");
    }
    int pos = keyToIndex_[key];
    if (pos >= 0 && pos < static_cast<int>(keyValuePairs_.size()) && keyValuePairs_[pos].first == key) {
      return false;
    }
    keyToIndex_[key] = keyValuePairs_.size();
    keyValuePairs_.emplace_back(key, value);
    return true;
  }

  bool lookup(int key, Value* value) const {
    if (key < 0 || key >= bufSize_) {
      throw out_of_range("Key out of range.");
    }
    int pos = keyToIndex_[key];
    if (pos >= 0 && pos < static_cast<int>(keyValuePairs_.size())) {
      const auto& keyValuePair = keyValuePairs_[pos];
      if (keyValuePair.first == key) {
        *value = keyValuePair.second;
        return true;
      }
    }
    return false;
  }

  int size() const {
    return keyValuePairs_.size();
  }

  auto begin() const -> decltype(this->keyValuePairs_.begin()) {
    return keyValuePairs_.begin();
  }

  auto end() const -> decltype(this->keyValuePairs_.end()) {
    return keyValuePairs_.end();
  }
};

int sizeOfPattern(int pattern) {
  return sizeof(unsigned int) * 8 - 1 - __builtin_clz(pattern);
}

thread_local vector<vector<pair<int, Fraction>>> coefficients;
thread_local vector<pair<int, Fraction>> merged;
thread_local IntMap<int> patternToIndex;

Fraction compute(int patternA, int patternB) {
  if (patternA == patternB) {
    return Fraction(1, 2);
  }
  int n = sizeOfPattern(patternA);
  patternToIndex.init(2 << n);
  patternToIndex.put(1, 1);
  for (int i = n - 1; i > 0; i--) {
    patternToIndex.put(patternA >> i, static_cast<int>(patternToIndex.size() + 1));
    patternToIndex.put(patternB >> i, static_cast<int>(patternToIndex.size() + 1));
  }
  int m = patternToIndex.size();
  if (coefficients.size() < static_cast<size_t>(m + 1)) {
    coefficients.resize(m + 1);
  }
  for (const auto& p : patternToIndex) {
    int pattern = p.first;
    auto& coeff = coefficients[p.second];
    coeff.clear();
    for (int d = 0; d <= 1; d++) {
      int newPattern = (pattern << 1) + d;
      if (newPattern == patternA) {
        coeff.emplace_back(0, Fraction(1, 2));
      } else if (newPattern != patternB) {
        int size = sizeOfPattern(newPattern);
        for (int i = size; i >= 0; i--) {
          int index;
          if (patternToIndex.lookup((newPattern & ((1 << i) - 1)) | (1 << i), &index)) {
            coeff.emplace_back(index, Fraction(1, 2));
            break;
          }
        }
      }
    }
    sort(coeff.begin(), coeff.end(), [] (const pair<int, Fraction>& left, const pair<int, Fraction>& right) { return left.first < right.first; });
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
        merged.clear();
        auto it1 = c.begin();
        auto it2 = coeff.begin();
        while (it1 != c.end() && it2 != coeff.end()) {
          if (it1->first == it2->first) {
            merged.emplace_back(it1->first, it1->second + ci * it2->second);
            ++it1;
            ++it2;
          } else if (it1->first < it2->first) {
            merged.emplace_back(*it1);
            ++it1;
          } else {
            merged.emplace_back(it2->first, ci * it2->second);
            ++it2;
          }
        }
        for (; it1 != c.end(); ++it1) {
          merged.emplace_back(*it1);
        }
        for (; it2 != coeff.end(); ++it2) {
          merged.emplace_back(it2->first, ci * it2->second);
        }
        swap(c, merged);
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

string convert(int pattern) {
  int n = sizeOfPattern(pattern);
  string s;
  for (int i = n - 1; i >= 0; i--) {
    s += (pattern & (1 << i)) ? 'T' : 'H';
  }
  return s;
}

int main(int argc, char* argv[]) {
#ifndef COMPUTE_ALL_PAIRS
  string patternA, patternB;
  while (true) {
    cin >> patternA;
    if (patternA == "$") {
      return 0;
    }
    cin >> patternB;
    cout << compute(convert(patternA), convert(patternB)) << endl;
  }
#else
  if (argc != 2) {
    cerr << "Help: " << argv[0] << " n" << endl;
    return 1;
  }
  int n = atoi(argv[1]);
  if (n <= 0) {
    cerr << "n must be positive" << endl;
    return 1;
  }
  int m = 1 << n;
  for (int i = m; i < 2 * m; i++) {
    cout << ',' << convert(i);
  }
  cout << endl;
  vector<string> results(m);
  #pragma omp parallel for schedule(dynamic, 1)
  for (int i = m; i < 2 * m; i++) {
    ostringstream oss;
    oss << convert(i);
    for (int j = m; j < 2 * m; j++) {
      oss << ',' << compute(i, j);
    }
    results[i - m] = oss.str();
  }
  for (int i = 0; i < m; i++) {
    cout << results[i] << endl;
  }
#endif
}
