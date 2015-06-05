#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

uint64_t compute(const vector<bool>& sequence, int delay) {
  vector<size_t> zeroPos(1), onePos(1);
  size_t n = sequence.size();
  for (size_t i = 0; i < n; i++) {
    (sequence[i] ? onePos : zeroPos).emplace_back(i);
  }

  size_t numZeros = zeroPos.size() - 1, numOnes = onePos.size() - 1;
  vector<uint64_t> results(numOnes + 1, 1);
  for (size_t i = 1; i <= numZeros; i++) {
    size_t zero = zeroPos[i];
    uint64_t result = 1;
    for (size_t j = 1; j <= numOnes; j++) {
      size_t one = onePos[j];
      if (one < zero) {
        result = results[j] + (one + delay < zero ? 0 : result);
      } else {
        result = result + (zero + delay < one ? 0 : results[j]);
      }
      results[j] = result;
    }
  }
  return results.back();
}

int main() {
  int delay;
  cin >> delay;
  string s;
  cin >> s;
  vector<bool> sequence(s.size());
  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == '1') {
      sequence[i] = true;
    } else if (s[i] != '0') {
      throw runtime_error("Invalid sequence");
    }
  }
  cout << compute(sequence, delay) << endl;
}
