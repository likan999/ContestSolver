#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

namespace std {
template <>
class hash<pair<int, int>> {
 public:
    size_t operator()(const pair<int, int>& p) const {
      return (((uint64_t)p.first) << 32) | p.second;
    }
};
}

const int N = 10000000;

vector<int> computePrimes();
bitset<N + 1> computeIsPrime();
unordered_map<int, vector<int>> computeNeighbors();


const vector<int> primes = computePrimes();
const bitset<N + 1> isPrime_ = computeIsPrime();
const unordered_map<int, vector<int>> neighbors_ = computeNeighbors();
const vector<int> emptyVector_;

vector<int> computePrimes() {
  vector<int> primes;
  bitset<N + 1> isPrime("11");
  isPrime.flip();

  bool sieve = true;
  for (int i = 2; i <= N; i++) {
    if (isPrime[i]) {
      primes.push_back(i);
      if (sieve) {
        if (i * i <= N) {
          for (int j = 2 * i; j <= N; j += i) {
            isPrime[j] = false;
          }
        } else {
          sieve= false;
        }
      }
    }
  }
  return primes;
}

bitset<N + 1> computeIsPrime() {
  bitset<N + 1> isPrime;
  for (int prime: primes) {
    isPrime[prime] = true;
  }
  return isPrime;
}

bool isPrime(int n) {
  return n <= N && isPrime_[n];
}

unordered_map<int, vector<int>> computeNeighbors() {
  unordered_map<int, vector<int>> neighbors;
  for (int prime: primes) {
    string s = to_string(prime);
    for (size_t i = 0; i < s.length(); i++) {
      char original = s[i];
      for (char c = i == 0 ? '1' : '0'; c <= '9'; c++) {
        if (c != original) {
          s[i] = c;
          int another = stoi(s);
          if (isPrime(another)) {
            neighbors[prime].push_back(another);
          }
        }
      }
      s[i] = original;
    }
    s = "1" + s;
    for (char c = '1'; c <= '9'; c++) {
      s[0] = c;
      int another = stoi(s);
      if (isPrime(another)) {
        neighbors[prime].push_back(another);
        neighbors[another].push_back(prime);
      }
    }
  }
  for (auto& p: neighbors) {
    sort(p.second.begin(), p.second.end());
  }
  return neighbors;
}

const vector<int>& neighbors(int n) {
  auto it = neighbors_.find(n);
  if (it != neighbors_.end()) {
    return it->second;
  } else {
    return emptyVector_;
  }
}

int64_t compute(int n) {
  vector<int> unreachables;
  bitset<N + 1> isReachable;
  isReachable[2] = true;
  for (size_t i = 1; i < primes.size() && primes[i] <= n; i++) {
    int p = primes[i];
    bool reachable = false;
    for (int neighbor: neighbors(p)) {
      if (neighbor >= p) {
        break;
      }
      if (isReachable[neighbor]) {
        reachable = true;
        break;
      }
    }
    if (reachable) {
      isReachable[p] = true;
      queue<int> q;
      q.push(p);
      while (!q.empty()) {
        int current = q.front();
        q.pop();
        for (int neighbor: neighbors(current)) {
          if (neighbor >= p) {
            break;
          }
          if (!isReachable[neighbor]) {
            isReachable[neighbor] = true;
            q.push(neighbor);
          }
        }
      }
    } else {
      unreachables.push_back(p);
    }
  }
  return accumulate(unreachables.begin(), unreachables.end(), 0LL);
}

int main() {
  cout << compute(1000) << endl;
  cout << compute(10000) << endl;
  cout << compute(10000000) << endl;
}
