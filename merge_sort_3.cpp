#include "test_runner.h"
#include <algorithm>
#include <memory>
#include <vector>
#include <iterator>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {


    size_t size = range_end - range_begin;

    if (size < 2) {
        return;
    }
    vector<typename RandomIt::value_type> v = {make_move_iterator(range_begin), make_move_iterator(range_end)};

    MergeSort<RandomIt>(v.begin(), v.begin() + size / 3);
    MergeSort<RandomIt>(v.begin() + size / 3, v.begin() + 2 * size / 3);
    MergeSort<RandomIt>(v.begin() + 2 * size / 3, v.end());

    vector<typename RandomIt::value_type> tmp;
    tmp.reserve(2 * size / 3);
    merge(make_move_iterator(v.begin()), make_move_iterator(v.begin() + size / 3),
            make_move_iterator(v.begin() + size / 3), make_move_iterator(v.begin() + 2 * size / 3), back_inserter(tmp));
    merge(make_move_iterator(tmp.begin()), make_move_iterator(tmp.end()),
          make_move_iterator(v.begin() + 2 * size / 3), make_move_iterator(v.end()), range_begin);
}

void TestIntVector() {
  vector<int> numbers = {6, 1, 3, 9, 1, 9, 8, 12, 1};
  MergeSort(begin(numbers), end(numbers));
  ASSERT(is_sorted(begin(numbers), end(numbers)));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestIntVector);
  return 0;
}
