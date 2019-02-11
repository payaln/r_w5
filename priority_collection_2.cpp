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

using namespace std;

template <typename T>
class Object {
public:

    Object() = default;


    explicit Object(T&& _obj) : obj(move(_obj)) {
        static size_t cnt = 0;
        id = cnt++;
    }

    Object(const Object& object)
            : obj(move(object.obj))
            , id(object.getId())
            , pr(object.pr) {}

    void Up() {
        ++pr;
    }

    Object& operator = (Object&& object) {
        obj = move(object.obj);
        id = object.getId();
        pr = object.pr;
        return *this;
    }

    size_t getId() {
        return id;
    }

    const size_t getPriority() const {
        return pr;
    }

    const T& getObj() const {
        return obj;
    }

    T obj;
private:
    size_t pr = 0;
    size_t id;

};


template <typename T>
class PriorityCollection {
public:

  using Id = size_t;

  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object) {
      Object<T> obj(move(object));
      size_t id = obj.getId();
      data[id] = move(obj);
      priority[0].insert(id);
      return id;
  }

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)

  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
      while (range_begin != range_end) {
          *(++ids_begin) = Add(move((*range_begin++)));
      }

  }

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const {
      return (data.count(id) > 0);
  }

  // Получить объект по идентификатору
  const T& Get(Id id) const {
      return data.at(id).getObj();
  }

  // Увеличить приоритет объекта на 1
  void Promote(Id id) {
      if (priority[data[id].getPriority()].size() < 2) {
          priority.erase(data[id].getPriority());
      } else {
          priority[data[id].getPriority()].erase(priority[data[id].getPriority()].find(id));
      }

      data[id].Up();
      priority[data[id].getPriority()].insert(id);
  }

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T&, int> GetMax() const {
      auto p = priority.rbegin()->second;
      size_t id = *p.rbegin();
      const T& t = data.at(id).getObj();
      int pr = data.at(id).getPriority();
      return make_pair(cref(t), pr);
  }

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, int> PopMax() {
      auto [k, p] = *priority.rbegin();
      size_t id = *p.rbegin();
      if (p.size() < 2) {
          priority.erase(k);
      } else {
          priority[k].erase(id);
      }
      T t = move(data.at(id).obj);
      size_t pr = data[id].getPriority();
      data.erase(id);
      return {move(t), pr};
  }

private:
    map<size_t, Object<T>> data;
    map<size_t, set<size_t>> priority;
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

  vector<string> vs{"aaaa", "bbbb", "qwerty"};
  vector<size_t> ids;

  strings.Add<typename vector<string>::iterator, back_insert_iterator<vector<size_t>>>(vs.begin(), vs.end(), back_inserter(ids));

  for (auto i : ids) {
      cout << i << endl;
  }

  strings.Promote(yellow_id);
  for (int i = 0; i < 2; ++i) {
    strings.Promote(red_id);
  }
  strings.Promote(yellow_id);
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
    ASSERT_EQUAL(item.first, "white");
    ASSERT_EQUAL(item.second, 0);
  }
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoCopy);
  return 0;
}
