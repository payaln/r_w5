#include <cstdint>
#include <algorithm>

using namespace std;


template <typename T>
class SimpleVector {
public:
  SimpleVector() = default;
  explicit SimpleVector(size_t size) : data(new T[size]), _size(size), capacity(size) {}
  ~SimpleVector() {
      delete[] data;
  }
  SimpleVector(SimpleVector& other)
        : data(new T[other.Capacity()])
        , _size(other.Size())
        , capacity(other.Capacity()){
      move(other.begin(), other.end(), data);
  }

  T& operator[](size_t index) {
      return (data[index]);
  }

  T* begin() {
      return data;
  }
  T* end() {
      return data + _size;
  }

  size_t Size() const {
      return _size;
  }
  size_t Capacity() const {
      return capacity;
  }
  void PushBack(const T& value) {
      if (_size == capacity) {
          Alloc();
      }
      data[_size++] = move(value);
  }

    void PushBack(T&& value) {
        if (_size == capacity) {
            Alloc();
        }
        data[_size++] = move(value);
    }


private:
  T* data = nullptr;
  size_t _size = 0;
  size_t capacity = 0;

  void Alloc() {
      if (capacity > 0) {
          capacity *= 2;
      } else {
          capacity = 1;
      }
      T* new_t = new T[capacity];
      move(begin(), end(), new_t);
      delete[] data;
      data = new_t;
  }

};
