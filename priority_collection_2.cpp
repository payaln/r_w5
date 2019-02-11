#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <deque>

using namespace std;

struct Priority {
    Priority() {
        static size_t cnt = 0;
        id = cnt++;
    }

    size_t pr = 0;
    size_t id;
};

//template <typename T>
//struct Item {
//    Priority p;
//    T t;
//    Item() = default;
//    Item(T&& other, const Priority& pr) : t(move(other)), p(pr){
//
//    }
//
//    Item(Item&& other) = default;
//    Item& operator = (Item&& other) = default;
//};

bool operator < (const Priority& l, const Priority& r) {
    if (l.pr == r.pr) {
        return l.id < r.id;
    } else {
        return l.pr < r.pr;
    }
}

template <typename T>
class PriorityCollection {
public:

  using Id = size_t;

  PriorityCollection() {
      data.resize(2000000);
  }

  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object) {
      Priority p;
      data[p.id] = move(object);
      priority[p] = p.id;
      re_priority[p.id] = p;
      return p.id;
  }

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)

  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
      move(range_begin, range_end, )
      while (range_begin != range_end) {
          Priority p;
          data[p.id] = move(*range_begin++);
          priority[p] = p.id;
          re_priority[p.id] = p;
          *(ids_begin++) = p.id;
      }

  }

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const {
      return (re_priority.count(id) > 0);
  }

  // Получить объект по идентификатору
  const T& Get(Id id) const {
      return data.at(id);
  }

  // Увеличить приоритет объекта на 1
  void Promote(Id id) {
      Priority p = re_priority[id];
      priority.erase(p);
      ++p.pr;
      priority[p] = id;
      re_priority[id] = p;
  }

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T&, int> GetMax() const {
      auto [k, v] = *priority.rbegin();
      return {move(data.at(v)), k.pr};
  }

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, int> PopMax() {
      auto [k, v] = *priority.rbegin();
      priority.erase(k);
      re_priority.erase(v);

      return {move(data.at(v)), k.pr};
  }

private:
    vector<T> data;
//    set<T> data;
    map<Priority, Id> priority;
    map<Id , Priority> re_priority;
};


class StringNonCopyable : public string {
public:
  using string::string;  // Позволяет использовать конструкторы строки
  StringNonCopyable(const StringNonCopyable&) = delete;
  StringNonCopyable(StringNonCopyable&&) = default;
  StringNonCopyable& operator=(const StringNonCopyable&) = delete;
  StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
  PriorityCollection<StringNonCopyable> strings;
  const auto white_id = strings.Add("white");
  const auto yellow_id = strings.Add("yellow");
  const auto red_id = strings.Add("red");

  vector<StringNonCopyable> vs;
  vs.emplace_back("qwerty");
  vs.emplace_back("aaaaaa");
  vector<size_t> ids;

    strings.Add<typename vector<StringNonCopyable>::iterator, back_insert_iterator<vector<size_t >>>
          (vs.begin(), vs.end(), back_inserter(ids));

    strings.Promote(ids[0]);
    strings.Promote(ids[0]);

  strings.Promote(yellow_id);
  for (int i = 0; i < 2; ++i) {
    strings.Promote(red_id);
  }
  strings.Promote(yellow_id);

    {
        const auto item = strings.GetMax();

        ASSERT_EQUAL(item.first, "qwerty");
        ASSERT_EQUAL(item.second, 2);
        const auto ite1m = strings.PopMax();
    }

  {
    const auto item = strings.PopMax();

    ASSERT_EQUAL(item.first, "red");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "yellow");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "aaaaaa");
    ASSERT_EQUAL(item.second, 0);
  }
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoCopy);
  return 0;
}
