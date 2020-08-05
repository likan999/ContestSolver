#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <utility>

using namespace std;

template <typename Int, Int Mod, typename Intermediate = Int>
class ModuloRing {
  static_assert(std::numeric_limits<Int>::is_integer);
  static_assert(std::numeric_limits<Int>::is_signed);
  static_assert(std::numeric_limits<Int>::max() / (Mod - 1) >= 2);
  static_assert(std::numeric_limits<Intermediate>::max() / (Mod - 1) >=
                Mod - 1);

 public:
  explicit ModuloRing(Int value = Int()) : value_(normalize(value)) {}

  Int value() const { return value_; }

#define DEFINE_OP_FROM_ASSIGN_(op, op_assign)        \
  ModuloRing operator op(ModuloRing another) const { \
    ModuloRing out = *this;                          \
    out op_assign another;                           \
    return out;                                      \
  }
  DEFINE_OP_FROM_ASSIGN_(+, +=)
  DEFINE_OP_FROM_ASSIGN_(-, -=)
  DEFINE_OP_FROM_ASSIGN_(*, *=)
#undef DEFINE_OP_FROM_ASSIGN_

  ModuloRing& operator+=(ModuloRing another) {
    value_ += another.value_;
    if (value_ >= Mod) value_ -= Mod;
    return *this;
  }

  ModuloRing operator-=(ModuloRing another) {
    value_ -= another.value_;
    if (value_ < 0) value_ += Mod;
    return *this;
  }

  ModuloRing operator*=(ModuloRing another) {
    value_ = static_cast<Intermediate>(value_) *
             static_cast<Intermediate>(another.value_) % Mod;
    return *this;
  }

 private:
  static Int normalize(Int value) {
    if (value >= 0) {
      if (value >= Mod) {
        value -= Mod;
        if (value >= Mod) {
          value %= Mod;
        }
      }
    } else {
      value += Mod;
      if (value < 0) {
        value %= Mod;
        if (value < 0) {
          value += Mod;
        }
      }
    }
    return value;
  }

  Int value_;
};

template <typename T, std::size_t N, std::size_t M>
class Matrix {
 public:
  explicit Matrix() = default;
  explicit Matrix(const std::array<std::array<T, M>, N>& values)
      : values_(values) {}

  const std::array<T, M>& operator[](std::size_t idx) const {
    return values_[idx];
  }
  std::array<T, M>& operator[](std::size_t idx) { return values_[idx]; }

#define DEFINE_ELEMENT_WISE_OP_(op, op_assign)                                \
  template <typename U>                                                       \
  auto operator op(const Matrix<U, N, M>& another) const {                    \
    Matrix<decltype(this->values_[0][0] op another.values_[0][0]), N, M> out; \
    for (std::size_t i = 0; i < N; ++i) {                                     \
      for (std::size_t j = 0; j < M; ++j) {                                   \
        out.values_[i][j] = values_[i][j] op another.values_[i][j];           \
      }                                                                       \
    }                                                                         \
    return out;                                                               \
  }                                                                           \
                                                                              \
  template <typename U>                                                       \
  Matrix& operator op_assign(const Matrix<U, N, M>& another) {                \
    for (std::size_t i = 0; i < N; ++i) {                                     \
      for (std::size_t j = 0; j < M; ++j) {                                   \
        values_[i][j] op_assign another.values_[i][j];                        \
      }                                                                       \
    }                                                                         \
    return *this;                                                             \
  }

  DEFINE_ELEMENT_WISE_OP_(+, +=)
  DEFINE_ELEMENT_WISE_OP_(-, -=)
#undef DEFINE_ELEMENT_WISE_OP_

  template <typename U, std::size_t L>
  auto operator*(const Matrix<U, M, L>& another) const {
    using R = decltype(this->values_[0][0] * another[0][0]);
    Matrix<R, N, L> out;
    for (std::size_t i = 0; i < N; ++i) {
      for (std::size_t j = 0; j < L; ++j) {
        out[i][j] = R();
        for (std::size_t k = 0; k < M; ++k) {
          out[i][j] += values_[i][k] * another[k][j];
        }
      }
    }
    return out;
  }

 private:
  std::array<std::array<T, M>, N> values_;
};

template <typename T>
T FastPower(const T& t, uint64_t n, const T& one = T(1)) {
  T out = (n % 2 != 0) ? t : one;
  n /= 2;
  if (n > 0) {
    T sqrt = FastPower(t, n, one);
    out = out * sqrt * sqrt;
  }
  return out;
}

namespace {
constexpr int64_t kMod = 999999937;
using Ring = ModuloRing<int64_t, kMod>;

Ring f(Ring a, Ring b, uint64_t n) {
  // b = ceil(sqrt(a))
  // c_n = (b+sqrt(a))^n + (b-sqrt(a))^n is integer
  // c_0 = 2, c_1 = 2*b
  // c_{n+2} = 2*b*c_{n+1} - (b^2-a)*c_n
  // C_n = [c_{n+1}; c_n]
  // C_{n+1} = [2*b, a-b^2; 1, 0] * C_n
  // C_n = [2*b, a-b^2; 1, 0]^n * [2*b; 2]
  Ring zero{0}, one{1}, two{2};
  Matrix<Ring, 2, 2> m{{{
      {{b + b, a - b * b}},
      {{one, zero}},
  }}};
  Matrix<Ring, 2, 2> identity{{{
      {{one, zero}},
      {{zero, one}},
  }}};
  auto p = FastPower(m, n, identity);
  Matrix<Ring, 2, 1> init{{{
      {{b + b}},
      {{two}},
  }}};
  auto c = p * init;
  return c[1][0] - one;
}

int64_t g(int64_t n) {
  Ring out{0};
  for (int64_t i = 1, s = 1, sq = 1; i <= n; ++i) {
    Ring rs(s);
    auto exponent = i * i;
    if (i == sq) {
      out += FastPower(rs + rs, exponent);
      sq += 2 * (s++) + 1;
    } else {
      out += f(Ring{i}, rs, exponent);
    }
  }
  return out.value();
}

}  // namespace

int main() {
  cout << g(1000) << endl;
  cout << g(5000000) << endl;
}
