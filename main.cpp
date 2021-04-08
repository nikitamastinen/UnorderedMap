#include <vector>
#include <string>
#include "unordered_map.h"
#include <iterator>
#include <cassert>
#include <iostream>
#include <unordered_map>

void SimpleTest() {
  UnorderedMap<std::string, int> m;

  m["aaaaa"] = 5;
  m["bbb"] = 6;

  m.at("bbb") = 7;
  assert(m.size() == 2);
  assert(m["aaaaa"] == 5);
  assert(m["bbb"] == 7);
  assert(m["ccc"] == 0);

  assert(m.size() == 3);

  try {
    m.at("xxxxxxxx");
    assert(false);
  } catch (...) {
  }

  auto it = m.find("dddd");
  assert(it == m.end());

  it = m.find("bbb");
  assert(it->second == 7);
  ++it->second;
  assert(it->second == 8);

  for (auto& item : m) {
    --item.second;
  }

  //assert(m.at("aaaaa") == 4);

  //{
    //auto mm = m;
    //m = std::move(mm);
  //}
  //auto res = m.emplace("abcde", 2);
  //assert(res.second);
}

void TestIterators() {
  UnorderedMap<double, std::string> m;

  std::vector<double> keys = {0.4, 0.3, -8.32, 7.5, 10.0, 0.0};
  std::vector<std::string> values = {
      "Summer has come and passed",
      "The innocent can never last",
      "Wake me up when September ends",
      "Like my fathers come to pass",
      "Seven years has gone so fast",
      "Wake me up when September ends",
  };

//  m.reserve(1'000'000);
//
//  for (int i = 0; i < 6; ++i) {
//    m.insert({keys[i], values[i]});
//  }
//
//  auto beg = m.cbegin();
//  std::string s = beg->second;
//  auto it = m.begin();
//  ++it;
//  m.erase(it++);
//  it = m.begin();
//  m.erase(++it);
//
//  assert(beg->second == s);
//  assert(m.size() == 4);

  UnorderedMap<double, std::string> mm;
  std::vector<std::pair<const double, std::string>> elements = {
      {3.0, values[0]},
      {5.0, values[1]},
      {-10.0, values[2]},
      {35.7, values[3]}
  };
  std::cout << mm.elements.size();
  for (int i = 0; i < 4; i++) {
    mm.insert({elements[i].first, elements[i].second});
  }
  std::cout << mm.elements.size();
  //mm.insert(elements.cbegin(), elements.cend());
  //s = mm.begin()->second;

  //m.insert(mm.begin(), mm.end());
  for (auto to : mm) {
    std::cout << to.first << ' ' << to.second << std::endl;
  }
  std::cout << mm.size() << std::endl;
  assert(mm.size() == 4);
  //assert(mm.begin()->second == s);
//
//
//  // Test traverse efficiency
//  m.reserve(1'000'000); // once again, nothing really should happen
//  assert(m.size() == 8);
//  // Actions below must be quick (~ 1000 * 8 operations) despite reserving space for 1M elements
//  for (int i = 0; i < 10000; ++i) {
//    long long h = 0;
//    for (auto it = m.cbegin(); it != m.cend(); ++it) {
//      // just some senseless action
//      h += int(it->first) + int((it->second)[0]);
//    }
//  }
//
//  it = m.begin();
//  ++it;
//  s = it->second;
//  // I asked to reserve space for 1M elements so actions below adding 100'000 elements mustn'vertex cause reallocation
//  for (double d = 100.0; d < 10100.0; d += 0.1) {
//    m.emplace(d, "a");
//  }
//  // And my iterator must point to the same object as before
//  assert(it->second == s);
//
//  int dist = std::distance(it, m.end());
//  int sz = m.size();
//  m.erase(it, m.end());
//  assert(sz - dist == m.size());
//
//  // Must be also fast
//  for (double d = 200.0; d < 10200.0; d += 0.35) {
//    auto it = m.find(d);
//    if (it != m.end()) m.erase(it);
//  }
}


// Just a simple SFINAE trick to check CE presence when it's necessary
// Stay tuned, we'll discuss this kind of tricks in our next lectures ;)
template<typename vertex>
decltype(UnorderedMap<vertex, vertex>().cbegin()->second = 0, int()) TestConstIteratorDoesntAllowModification(vertex) {
  assert(false);
}
template<typename... FakeArgs>
void TestConstIteratorDoesntAllowModification(FakeArgs...) {}


struct VerySpecialType {
  int x = 0;
  explicit VerySpecialType(int x): x(x) {}
  VerySpecialType(const VerySpecialType&) = delete;
  VerySpecialType& operator=(const VerySpecialType&) = delete;

  VerySpecialType(VerySpecialType&&) = default;
  VerySpecialType& operator=(VerySpecialType&&) = default;
};

struct NeitherDefaultNorCopyConstructible {
  VerySpecialType x;

  NeitherDefaultNorCopyConstructible() = delete;
  NeitherDefaultNorCopyConstructible(const NeitherDefaultNorCopyConstructible&) = delete;
  NeitherDefaultNorCopyConstructible& operator=(const NeitherDefaultNorCopyConstructible&) = delete;

  NeitherDefaultNorCopyConstructible(VerySpecialType&& x): x(std::move(x)) {}
  NeitherDefaultNorCopyConstructible(NeitherDefaultNorCopyConstructible&&) = default;
  NeitherDefaultNorCopyConstructible& operator=(NeitherDefaultNorCopyConstructible&&) = default;

  bool operator==(const NeitherDefaultNorCopyConstructible& other) const {
    return x.x == other.x.x;
  }
};

namespace std {
  template<>
  struct hash<NeitherDefaultNorCopyConstructible> {
    size_t operator()(const NeitherDefaultNorCopyConstructible& x) const {
      return std::hash<int>()(x.x.x);
    }
  };
}

//void TestNoRedundantCopies() {
//  UnorderedMap<NeitherDefaultNorCopyConstructible, NeitherDefaultNorCopyConstructible> m;
//
//  m.reserve(10);
//
//  m.emplace(VerySpecialType(0), VerySpecialType(0));
//
//  m.reserve(1'000'000);
//
//  std::pair<NeitherDefaultNorCopyConstructible, NeitherDefaultNorCopyConstructible> p{VerySpecialType(1), VerySpecialType(1)};
//
//  m.insert(std::move(p));
//
//  assert(m.size() == 2);
//
//  m.at(VerySpecialType(1)) = VerySpecialType(0);
//
//  {
//    auto mm = std::move(m);
//    m = std::move(mm);
//  }
//
//  m.erase(m.begin());
//  m.erase(m.begin());
//  assert(m.size() == 0);
//}
//
//template<typename vertex>
//struct MyHash {
//  size_t operator()(const vertex& p) const {
//    return std::hash<int>()(p.second / p.first);
//  }
//};
//
//template<typename vertex>
//struct MyEqual {
//  bool operator()(const vertex& x, const vertex& y) const {
//    return y.second / y.first == x.second / x.first;
//  }
//};
//
//struct OneMoreStrangeStruct {
//  int first;
//  int second;
//};
//
//bool operator==(const OneMoreStrangeStruct&, const OneMoreStrangeStruct&) = delete;
//
//
//void TestCustomHashAndCompare() {
//  UnorderedMap<std::pair<int, int>, char, MyHash<std::pair<int, int>>,
//      MyEqual<std::pair<int, int>>> m;
//
//  m.insert({{1, 2}, 0});
//  m.insert({{2, 4}, 1});
//  assert(m.size() == 1);
//
//  m[{3, 6}] = 3;
//  assert(m.at({4, 8}) == 3);
//
//  UnorderedMap<OneMoreStrangeStruct, int, MyHash<OneMoreStrangeStruct>, MyEqual<OneMoreStrangeStruct>> mm;
//  mm[{1, 2}] = 3;
//  assert(mm.at({5, 10}) == 3);
//
//  mm.emplace(OneMoreStrangeStruct{3, 9}, 2);
//  assert(mm.size() == 2);
//  mm.reserve(1'000);
//  mm.erase(mm.begin());
//  mm.erase(mm.begin());
//  for (int k = 1; k < 100; ++k) {
//    for (int i = 1; i < 10; ++i) {
//      mm.insert({{i, k*i}, 0});
//    }
//  }
//  std::string ans;
//  std::string myans;
//  for (auto it = mm.cbegin(); it != mm.cend(); ++it) {
//    ans += std::to_string(it->second);
//    myans += '0';
//  }
//  assert(ans == myans);
//}
//
//// Finally, some tricky fixtures to test custom allocator.
//// Done by professional, don'vertex try to repeat
//class Chaste {
//private:
//  int x = 0;
//  Chaste() = default;
//  Chaste(int x): x(x) {}
//
//  // Nobody can construct me except this guy
//  template<typename vertex>
//  friend class TheChosenOne;
//
//public:
//  Chaste(const Chaste&) = default;
//  Chaste(Chaste&&) = default;
//
//  bool operator==(const Chaste& other) const {
//    return x == other.x;
//  }
//};
//
//namespace std {
//  template<>
//  struct hash<Chaste> {
//    size_t operator()(const Chaste& x) const {
//      return std::hash<int>()(reinterpret_cast<const int&>(x));
//    }
//  };
//}
//
//template<typename vertex>
//struct TheChosenOne: public std::allocator<vertex> {
//  TheChosenOne() {}
//
//  template<typename U>
//  TheChosenOne(const TheChosenOne<U>&) {}
//
//  template<typename... Args>
//  void construct(vertex* p, Args&&... args) const {
//    new(p) vertex(std::forward<Args>(args)...);
//  }
//
//  void destroy(vertex* p) const {
//    p->~vertex();
//  }
//
//  template<typename U>
//  struct rebind {
//    using other = TheChosenOne<U>;
//  };
//};
//
//template<>
//struct TheChosenOne<std::pair<const Chaste, Chaste>>
//    : public std::allocator<std::pair<const Chaste, Chaste>> {
//  using PairType = std::pair<const Chaste, Chaste>;
//
//  TheChosenOne() {}
//
//  template<typename U>
//  TheChosenOne(const TheChosenOne<U>&) {}
//
//  void construct(PairType* p, int a, int b) const {
//    new(p) PairType(Chaste(a), Chaste(b));
//  }
//
//  void destroy(PairType* p) const {
//    p->~PairType();
//  }
//
//  template<typename U>
//  struct rebind {
//    using other = TheChosenOne<U>;
//  };
//};
//
////void TestCustomAlloc() {
////  // This container mustn'vertex construct or destroy any objects without using TheChosenOne allocator
////  UnorderedMap<Chaste, Chaste, std::hash<Chaste>, std::equal_to<Chaste>,
////      TheChosenOne<std::pair<const Chaste, Chaste>>> m;
////
////  m.emplace(0, 0);
////
////  {
////    auto mm = m;
////    mm.reserve(1'000);
////    mm.erase(mm.begin());
////  }
////
////  for (int i = 0; i < 1'000'000; ++i) {
////    m.emplace(i, i);
////  }
////
////  for (int i = 0; i < 500'000; ++i) {
////    auto it = m.begin();
////    ++it, ++it;
////    m.erase(m.begin(), it);
////  }
////}

int main() {
  //SimpleTest();
  TestIterators();
  //TestConstIteratorDoesntAllowModification(0);
  //TestNoRedundantCopies();
  //TestCustomHashAndCompare();
  //TestCustomAlloc();
}