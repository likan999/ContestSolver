#include <algorithm>
#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

using namespace std;

typedef complex<double> Complex;

const Complex I = { 0, 1 };
const Complex s = 0.8 + 0.6*I;
const Complex r1 = 0.8 * s;
const Complex r2 = 1.0 - r1;
const Complex p1 = (0.5 + 0.5 * I) * (I + r1);
const Complex p2 = (0.5 + 0.5 * I) * (1.0 + r2 * I);
const double bound = max(abs(p1) / (1 - abs(r1)), abs(p2) / (1 - abs(r2)));

struct Interval {
  Complex z;
  double base;
  double upper;
  double lower;
  bool valid = true;

  Interval(const Complex& z, double base) : z(z), base(base) {
    double e = abs(z) * bound;
    upper = base + e;
    lower = base - e;
  }
};

struct UpperCompare {
  bool operator()(const Interval* left, const Interval* right) {
    return left->upper < right->upper;
  }
};

struct LowerCompare {
  bool operator()(const Interval* left, const Interval* right) {
    return left->lower < right->lower;
  }
};

pair<double, double> f(const Complex& z, double eps) {
  priority_queue<Interval*, vector<Interval*>, UpperCompare> upperQueue;
  priority_queue<Interval*, vector<Interval*>, LowerCompare> lowerQueue;
  auto interval = new Interval(z, 0);
  upperQueue.push(interval);
  lowerQueue.push(interval);
  double constant = 0;
  double upper = max(constant, upperQueue.top()->upper);
  double lower = max(constant, lowerQueue.top()->lower);
  while (upper - lower >= eps) {
    auto interval = upperQueue.top();
    upperQueue.pop();
    interval->valid = false;
    constant = max(constant, interval->base);
    auto left = new Interval(interval->z * r1, interval->base + real(interval->z * p1));
    auto right = new Interval(interval->z * r2, interval->base + real(interval->z * p2));
    upperQueue.push(left);
    upperQueue.push(right);
    lowerQueue.push(left);
    lowerQueue.push(right);
    while (!lowerQueue.top()->valid) {
      delete lowerQueue.top();
      lowerQueue.pop();
    }
    upper = max(constant, upperQueue.top()->upper);
    lower = max(constant, lowerQueue.top()->lower);
  }
  while (!lowerQueue.empty()) {
    delete lowerQueue.top();
    lowerQueue.pop();
  }
  return make_pair(lower, upper);
}

int main() {
  const double eps = 1e-13;
  auto right = f({1,0}, eps);
  auto left = f({-1,0}, eps);
  auto up = f({0,-1}, eps);
  auto down = f({0,1}, eps);
  cout << setprecision(16);
  cout << "[" << right.first << ", " << right.second << "]" << endl;
  cout << "[" << left.first << ", " << left.second << "]" << endl;
  cout << "[" << up.first << ", " << up.second << "]" << endl;
  cout << "[" << down.first << ", " << down.second << "]" << endl;
  double maxS = (max(0.5, right.second) + max(0.5, left.second)) * (max(0.5, up.second) + max(0.5, down.second));
  double minS = (max(0.5, right.first) + max(0.5, left.first)) * (max(0.5, up.first) + max(0.5, down.first));
  cout << "[" << minS << ", " << maxS << "]" << endl;
}
