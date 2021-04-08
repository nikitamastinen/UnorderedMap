#include <list>
#include <vector>
#include <type_traits>
#include <iostream>


template<typename T, typename Allocator = std::allocator<T>>
class List {
  class Node {
  public:
    Node* next;
    Node* prev;
    T value;

    explicit Node(): next(nullptr), prev(nullptr), value(T()) {}
    explicit Node(const T& value): next(nullptr), prev(nullptr), value(value) {}
  };

  Node* head;
  size_t length;
  using NAllocator = typename Allocator::template rebind<Node>::other;
  NAllocator allocator;
  Allocator t_allocator;
public:
  Node* insert(Node* node, const T& value) {
    Node* ins = std::allocator_traits<NAllocator>::allocate(allocator, 1);
    std::allocator_traits<NAllocator>::construct(allocator, ins, value);
    ins->next = node->next;
    ins->prev = node;
    node->next->prev = ins;
    node->next = ins;
    ++length;
    return ins;
  }

  Node* erase(Node* node) {
    if (node == head) {
      return head;
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    Node* result = node->next;
    std::allocator_traits<NAllocator>::destroy(allocator, node);
    std::allocator_traits<NAllocator>::deallocate(allocator, node, 1);
    --length;
    return result;
  }

  void no_allocator_swap(List& other) {
    std::swap(length, other.length);
    std::swap(head, other.head);
  }
public:
  explicit List(const Allocator& t_allocator = Allocator()):
      length(0), t_allocator(t_allocator) {
    typename Allocator::template rebind<Node>::other n_allocator;
    allocator = n_allocator;
    head = std::allocator_traits<NAllocator>::allocate(allocator, 1);
    head->next = head;
    head->prev = head;
  }

  explicit List(
      size_t count,
      const T& value,
      const Allocator& t_allocator = Allocator()):
      length(count), t_allocator(t_allocator) {
    typename Allocator::template rebind<Node>::other n_allocator;
    allocator = n_allocator;
    head = std::allocator_traits<NAllocator>::allocate(allocator, 1);
    head->next = head;
    head->prev = head;
    Node* copy_before = head;
    Node* copy_after = head;
    for (size_t i = 0; i < count; i++) {
      copy_after = std::allocator_traits<NAllocator>::allocate(allocator, 1);
      std::allocator_traits<NAllocator>::construct(allocator, copy_after, value);
      copy_before->next = copy_after;
      copy_after->prev = copy_before;
      copy_before = copy_after;
    }
    copy_after->next = head;
    head->prev = copy_after;
  }

  explicit List(
      size_t count,
      const Allocator& t_allocator = Allocator()):
      length(count), t_allocator(t_allocator) {
    typename Allocator::template rebind<Node>::other n_allocator;
    allocator = n_allocator;
    head = std::allocator_traits<NAllocator>::allocate(allocator, 1);
    head->next = head;
    head->prev = head;
    Node* copy_before = head;
    Node* copy_after = head;
    for (size_t i = 0; i < count; i++) {
      copy_after = std::allocator_traits<NAllocator>::allocate(allocator, 1);
      std::allocator_traits<NAllocator>::construct(allocator, copy_after);
      copy_before->next = copy_after;
      copy_after->prev = copy_before;
      copy_before = copy_after;
    }
    copy_after->next = head;
    head->prev = copy_after;
  }

  List(const List& other): length(other.length) {
    t_allocator = std::allocator_traits<Allocator>::
    select_on_container_copy_construction(other.t_allocator);
    allocator = std::allocator_traits<NAllocator>::
    select_on_container_copy_construction(other.allocator);
    head = std::allocator_traits<NAllocator>::allocate(allocator, 1);
    head->next = head;
    head->prev = head;
    Node* copy_before = head;
    Node* copy_after = head;
    Node* other_head = other.head;
    for (size_t i = 0; i < length; i++) {
      other_head = other_head->next;
      copy_after = std::allocator_traits<NAllocator>::allocate(allocator, 1);
      std::allocator_traits<NAllocator>::
      construct(allocator, copy_after, other_head->value);
      copy_before->next = copy_after;
      copy_after->prev = copy_before;
      copy_before = copy_after;
    }
    copy_after->next = head;
    head->prev = copy_after;
  }

  void clear() {
    while (begin() != end()) {
      erase(begin());
    }
  }

  List(List&& other) noexcept : length(other.length) {
    t_allocator = std::move(other.t_allocator);
    allocator = std::move(other.allocator);
    auto new_head = std::allocator_traits<NAllocator>::allocate(allocator, 1);
    new_head->next = new_head;
    new_head->prev = new_head;
    other.length = 0;
    head = other.head;
    other.head = new_head;
  }

  List& operator=(const List& other) {
    if (this == &other) {
      return *this;
    }
    List copy = other;
    if (std::allocator_traits<Allocator>::
    propagate_on_container_copy_assignment::value) {
      t_allocator = other.t_allocator;
      allocator = other.allocator;
    }
    no_allocator_swap(copy);
    return *this;
  }

  List& operator=(List&& other) noexcept {
    if (this == &other) {
      return *this;
    }
    List copy = std::move(other);
    if (std::allocator_traits<Allocator>::
    propagate_on_container_move_assignment::value) {
      t_allocator = std::move(other.t_allocator);
      allocator = std::move(other.allocator);
    }
    no_allocator_swap(copy);
    return *this;
  }

  ~List() {
    Node* copy_before = head->next;
    for (size_t i = 0; i < length; i++) {
      auto copy_after = copy_before->next;
      std::allocator_traits<NAllocator>::destroy(allocator, copy_before);
      std::allocator_traits<NAllocator>::deallocate(allocator, copy_before, 1);
      copy_before = copy_after;
    }
    std::allocator_traits<NAllocator>::deallocate(allocator, head, 1);
  }

  Allocator get_allocator() const {
    return t_allocator;
  }

  template<bool IsConst>
  class iterator_impl {
  public:
    std::conditional_t<IsConst, const Node*, Node*> it;
    friend class List;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = std::conditional_t<IsConst, const T*, T*>;;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;

    iterator_impl(Node* node) {
      it = node;
    }

    //iterator_impl(const iterator_impl& other) = default;

    operator iterator_impl<true>() {
      return iterator_impl<true>(it);
    }

    std::conditional_t<IsConst, const_reference, reference>  operator*() const {
      return it->value;
    }

    std::conditional_t<IsConst, const pointer , pointer> operator->() const {
      return &(it->value);
    }

    iterator_impl& operator++() {
      it = it->next;
      return *this;
    }

    iterator_impl operator++(int) {
      auto copy = *this;
      it = it->next;
      return copy;
    }

    iterator_impl& operator--() {
      it = it->prev;
      return *this;
    }

    iterator_impl operator--(int) {
      auto copy = *this;
      it = it->prev();
      return copy;
    }

    bool operator==(const iterator_impl& other) const {
      return it == other.it;
    }

    bool operator!=(const iterator_impl& other) const {
      return !(*this == other);
    }
  };

  using const_iterator = iterator_impl<true>;
  using iterator = iterator_impl<false>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  size_t size() const {
    return length;
  }

  iterator begin() const {
    iterator it(head->next);
    return it;
  }

  const_iterator cbegin() const {
    const_iterator it(head->next);
    return it;
  }

  reverse_iterator rbegin() const {
    reverse_iterator it(head);
    return it;
  }

  const_reverse_iterator crbegin() const {
    const_reverse_iterator it(head);
    return it;
  }

  iterator end() const {
    iterator it(head);
    return it;
  }

  const_iterator cend() const {
    const_iterator it(head->next);
    return it;
  }

  reverse_iterator rend() const {
    reverse_iterator it(head->next);
    return it;
  }

  const_reverse_iterator crend() const {
    const_reverse_iterator it(head->next);
    return it;
  }

  void push_front(const T& value) {
    insert(head, value);
  }

  void push_back(const T& value) {
    insert(head->prev, value);
  }

  void pop_front() {
    erase(head->next);
  }

  void pop_back() {
    erase(head->prev);
  }

  iterator insert(const_iterator it, const T& value) {
    return iterator(insert(const_cast<Node*>((--it).it), value));
  }

  iterator erase(const_iterator it) {
    return iterator(erase(const_cast<Node*>(it.it)));
  }
};

template<
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename Equal = std::equal_to<Key>,
    typename Allocator = std::allocator<std::pair<const Key, Value>>
>
class UnorderedMap {
public:
  using NodeType = std::pair<const Key, Value>;
  using ListIterator = typename List<NodeType*>::iterator;
  using ConstListIterator = typename List<NodeType*>::const_iterator;
  template<bool IsConst>
  class iterator_impl {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = NodeType;
    using difference_type = size_t;
    using pointer = typename std::conditional_t<IsConst, const NodeType*, NodeType*>;
    using reference = typename std::conditional_t<IsConst, const NodeType&, NodeType&>;
    using list_iterator =
    typename std::conditional_t<IsConst, ConstListIterator, ListIterator>;

    list_iterator it;
    reference operator*() const {
      return **it;
    }

    pointer operator->() const {
      return *it;
    }

    list_iterator get() {
      return it;
    }

    iterator_impl(const list_iterator& node): it(node) {}

    operator iterator_impl<true>() {
      return iterator_impl<true>(it);
    }

    iterator_impl& operator++() {
      ++it;
      return *this;
    }

    iterator_impl& operator--() {
      --it;
      return *this;
    }

    iterator_impl operator++(int) {
      auto copy = *this;
      ++it;
      return copy;
    }

    bool operator==(const iterator_impl& other) const {
      return it == other.it;
    }

    bool operator!=(const iterator_impl& other) const {
      return it != other.it;
    }
  };


  using iterator = iterator_impl<false>;
  using const_iterator = iterator_impl<true>;

  std::vector<ListIterator> hash_array;
  Hash hash_function;
  //using Alloc = typename Allocator::template rebind<NodeType*>::other;
  Allocator t_alloc;
  List<NodeType*> elements;
  Equal equal_key;


  float current_max_load_factor = 0.75;

  UnorderedMap() {
    hash_array.resize(1, elements.end());
  }

  UnorderedMap(const UnorderedMap& other):
      hash_function(other.hash_function),
      t_alloc(
          std::allocator_traits<Allocator>::select_on_container_copy_construction(other.t_alloc)
      ),
      equal_key(other.equal_key),
      current_max_load_factor(other.current_max_load_factor) {
    hash_array.resize(1, elements.end());
    for (auto it : other) {
      insert(it);
    }
  }

  UnorderedMap(UnorderedMap&& other):
      hash_array(std::move(other.hash_array)),
      hash_function(std::move(other.hash_function)),
      t_alloc(std::move(other.t_alloc)),
      elements(std::move(other.elements)),
      equal_key(std::move(other.equal_key)),
      current_max_load_factor(std::move(other.current_max_load_factor))
  {}

  void clear_list_elements() {
    if (elements.size() == 0) return;
    for (iterator it = elements.begin(); it != elements.end(); ++it) {
      std::allocator_traits<Allocator>::destroy(t_alloc, &(*it));
      std::allocator_traits<Allocator>::deallocate(t_alloc, &(*it), 1);
    }
    elements.clear();
  }

  void swap_and_kill(UnorderedMap&& other) {
    hash_array = std::move(other.hash_array);
    hash_function = std::move(other.hash_function);
    clear_list_elements();
    elements = std::move(other.elements);
    equal_key = std::move(other.equal_key);
    current_max_load_factor = std::move(other.current_max_load_factor);
  }

  UnorderedMap& operator=(const UnorderedMap& other) {
    if (this == &other) {
      return *this;
    }
    UnorderedMap copy = other;
    if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
      t_alloc = other.t_alloc;
    }
    swap_and_kill(std::move(copy));
    return *this;
  }

  UnorderedMap& operator=(UnorderedMap&& other) noexcept {
    if (this == &other) {
      return *this;
    }
    if (std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
      t_alloc = std::move(other.t_alloc);
    }
    swap_and_kill(std::move(other));
    return *this;
  }

  ~UnorderedMap() {
    clear_list_elements();
  }

  size_t size() const {
    return elements.size();
  }

  size_t get_hash(const Key& key) const {
    return hash_function(key) % hash_array.size();
  }

  float load_factor() const {
    return static_cast<float>(elements.size()) / hash_array.size();
  }

  float load_factor_after_insert() const {
    return static_cast<float>(elements.size() + 1) / hash_array.size();
  }

  void max_load_factor(float value) {
    current_max_load_factor = value;
  }

  float max_load_factor() {
    return current_max_load_factor;
  }

  void update() {
    if (load_factor_after_insert() > current_max_load_factor) {
      reserve(hash_array.size() * 2 + 1);
    }
  }

  void reserve(size_t count) {
    if (count > hash_array.size()) {
      rehash(static_cast<size_t>(static_cast<float>(count) / max_load_factor() + 1));
    }
  }

  void rehash(size_t count) {
    hash_array.clear();
    List<NodeType*> copy = std::move(elements);
    hash_array.resize(count, elements.end());
    for (ListIterator it = copy.begin(); it != copy.end(); ++it) {
      ListIterator& elem = hash_array[get_hash((*it)->first)];
      if (elem == elements.end()) {
        elem = elements.insert(elements.cend(), *it);
      } else {
        elem = elements.insert(elem, *it);
      }
    }
  }

  template<class... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    NodeType* mover = std::allocator_traits<Allocator>::allocate(t_alloc, 1);
    std::allocator_traits<Allocator>::construct(t_alloc, mover, std::forward<Args>(args)...);
    iterator result = find(mover->first);
    if (result != elements.end()) {
      std::allocator_traits<Allocator>::destroy(t_alloc, mover);
      std::allocator_traits<Allocator>::deallocate(t_alloc, mover, 1);
      return {result, false};
    }
    update();
    ListIterator& elem = hash_array[get_hash(mover->first)];
    if (elem ==  elements.end()) {
      elements.insert(elements.cend(), mover);
    } else {
      elements.insert(elem, mover);
    }
    return {elem, true};
  }

  std::pair<iterator, bool> insert(const NodeType& value) {
    iterator result = find(value.first);
    if (result != elements.end()) {
      return {result, false};
    }
    update();
    ListIterator& elem = hash_array[get_hash(value.first)];
    NodeType* copy = std::allocator_traits<Allocator>::allocate(t_alloc, 1);
    std::allocator_traits<Allocator>::construct(t_alloc, copy, value);
    if (elem == elements.end()) {
      elem = elements.insert(elements.end(), copy);
    } else {
      elem = elements.insert(elem, copy);
    }
    return {elem, true};
  }

  template<typename NodePair>
  std::pair<iterator, bool> insert(NodePair&& value) {
    iterator result = find(value.first);
    if (result != iterator(elements.end())) {
      return {result, false};
    }
    update();
    ListIterator& elem = hash_array[get_hash(value.first)];
    NodeType* mover = std::allocator_traits<Allocator>::allocate(t_alloc, 1);
    std::allocator_traits<Allocator>::construct(t_alloc, mover, std::forward<NodePair>(value));
    if (elem ==  elements.end()) {
      elem = elements.insert(elements.end(), mover);
    } else {
      elem = elements.insert(elem, mover);
    }
    return {elem, true};
  }

  template<typename Input>
  void insert(Input first, Input last) {
    for (; first != last; insert(*first++));
  }

  size_t erase(const Key& key) {
    iterator it = find(key);
    if (it == elements.end()) {
      return 0;
    }
    erase(it);
    return 1;
  }

  iterator erase(const_iterator it) {
    size_t hash = get_hash(it->first);
    if (const_iterator(hash_array[hash]) != it) {
      return elements.erase(it.it);
    }
    auto nit = elements.erase(it.it);
    hash_array[hash] = (
        nit != elements.end() && get_hash((*nit)->first) == hash ? nit : elements.end()
    );
    return nit;
  }

  iterator erase(const_iterator first, const_iterator last) {
    const_iterator stop = last;
    --stop;
    for (const_iterator it = first; it != last; it = erase(it)) {
      if (it == stop) {
        return erase(it);
      }
    }
    return end();
  }

  iterator find(const Key& key) {
    size_t hash = get_hash(key);
    iterator it = hash_array[hash];
    //iterator it(elements.begin());
    while (it != elements.end() && get_hash(it->first) == hash) {
      if (equal_key(it->first, key)) {
        return it;
      }
      ++it;
    }
    return elements.end();
  }

  Value& at(const Key& key) {
    size_t hash = get_hash(key);
    ListIterator it = hash_array[hash];
    while (it != elements.end() && get_hash((*it)->first) == hash) {
      if (equal_key((*it)->first, key)) {
        return (*it)->second;
      }
      ++it;
    }
    throw std::out_of_range("Target element doesn't exists");
  }

  Value& operator[](const Key& key) {
    try {
      return at(key);
    } catch(std::out_of_range&) {
      iterator it = insert({key, Value()}).first;
      return it->second;
    }
  }

  iterator begin() {
    return elements.begin();
  }

  const_iterator begin() const {
    return elements.cbegin();
  }

  iterator end() {
    return elements.end();
  }

  const_iterator end() const {
    return elements.cend();
  }

  const_iterator cbegin() const {
    return elements.cbegin();
  }

  const_iterator cend() const {
    return elements.cend();
  }
};