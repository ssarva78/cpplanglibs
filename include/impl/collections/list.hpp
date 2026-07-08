#ifndef __LANGLIBS_COLLECTIONS_LIST_IMPL__
#define __LANGLIBS_COLLECTIONS_LIST_IMPL__

namespace lang::collections {
  template<typename T, bool ThreadSafe> class linkedlistnode {
    public:
      linkedlistnode() :
        _val(pointer<T, ThreadSafe>::null()),
        _next(pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>::null()),
        _previous(pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>::null())
        {}
      explicit linkedlistnode(const pointer<T, ThreadSafe>& ptr) :
        _val(ptr),
        _next(pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>::null()),
        _previous(pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>::null())
        {}

      const pointer<T, ThreadSafe>& operator*() const { return _val; }
      pointer<T, ThreadSafe>& operator*() { return _val; }

      const pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& next() const
        { return _next; }
      pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& next()
        { return _next; }

      const pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& previous() const
        { return _previous; }
      pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& previous()
        { return _previous; }

      static void link(
          pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& left,
          pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& right) {
        (*right).previous() = left;
        if ((*left).next()) {
          (*right).next() = (*left).next();
          (*(*left).next()).previous() = right;
        }
        (*left).next() = right;
      }

      pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe> unlink() {
        auto to_return = next();
        (*next()).previous() = previous();
        if (previous()) {
          (*previous()).next() = next();
        }
        next() = previous() = pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>::null();
        return to_return;
      }

    private:
      pointer<T, ThreadSafe> _val;
      pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe> _next, _previous;
  };

  template<typename T, bool ThreadSafe> struct linkedlistentry {
    pointer<linkedlistcapacity<T, ThreadSafe>, ThreadSafe> _capacity;
    pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe> _last, _first;
    linkedlistentry() : _capacity(), _last(), _first(_last) {}
    ~linkedlistentry() {
      (*_first).next() = pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>::null();
      (*_last).previous() = pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>::null();
    }
  };

  template<typename T, bool ThreadSafe> struct linkedlistcapacity {
    using list_size_t =
      std::conditional_t<ThreadSafe, std::atomic<long long>, long long>;
    list_size_t _len, _max;
    linkedlistcapacity() : _len(0), _max(0) {}
  };

  // iterator methods
  template<typename T, bool ThreadSafe>
  list<T, ThreadSafe>::iterator::iterator(
      const pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& node,
      const pointer<linkedlistentry<T, ThreadSafe>, ThreadSafe>& entry) :
      _current(node), _entry(entry)
    {}

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator&
  list<T, ThreadSafe>::iterator::add(const pointer<T, ThreadSafe>& ptr) {
    pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe> newnode(ptr);
    if (has_next()) {
      linkedlistnode<T, ThreadSafe>::link(_current, newnode);
    } else if (not has_previous()) {
      // no item present in the list; only terminating node exists
      linkedlistnode<T, ThreadSafe>::link(newnode, _current);
      (*_entry)._first = newnode;
    } else {
      // attempting to add after terminating node
      throw iterator_position_error();
    }
    ++(*(*_entry)._capacity)._len;
    _current = newnode;
    return *this;
  }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator&
  list<T, ThreadSafe>::iterator::add(std::initializer_list<T> lst) {
    for (auto item : lst) {
      add(item);
    }
    return *this;
  }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator&
  list<T, ThreadSafe>::iterator::add(const list<T, false>& lst) {
    for (auto item : lst) {
      add(item);
    }
    return *this;
  }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator&
  list<T, ThreadSafe>::iterator::add(const list<T, true>& lst) {
    for (auto item : lst) {
      add(item);
    }
    return *this;
  }

  template<typename T, bool ThreadSafe> nullable<T, ThreadSafe>
  list<T, ThreadSafe>::iterator::remove() {
    nullable<T, ThreadSafe> return_value = operator*();
    auto node_after_delete = (*_current).unlink();
    if (_current == (*_entry)._first) {
      (*_entry)._first = node_after_delete;
    }
    _current = node_after_delete;
    --(*(*_entry)._capacity)._len;
    return return_value;
  }

  template<typename T, bool ThreadSafe> const nullable<T, ThreadSafe>
  list<T, ThreadSafe>::iterator::operator*() const {
    if (not has_next())
      throw iterator_position_error();
    return nullable<T, ThreadSafe>(**_current);
  }

  template<typename T, bool ThreadSafe> nullable<T, ThreadSafe>
  list<T, ThreadSafe>::iterator::operator*() {
    if (not has_next())
      throw iterator_position_error();
    return nullable<T, ThreadSafe>(**_current);
  }

  template<typename T, bool ThreadSafe> bool
  list<T, ThreadSafe>::iterator::has_next() const
    { return not (*_current).next().isnull(); }

  template<typename T, bool ThreadSafe> bool
  list<T, ThreadSafe>::iterator::has_previous() const
    { return not (*_current).previous().isnull(); }

  template<typename T, bool ThreadSafe> 
  list<T, ThreadSafe>::iterator::operator bool() const
    { return has_next(); }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator
  list<T, ThreadSafe>::iterator::operator++(int) {
    if (not has_next())
      throw iterator_position_error();
    list<T, ThreadSafe>::iterator iter = *this;
    _current = (*_current).next();
    return iter;
  }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator&
  list<T, ThreadSafe>::iterator::operator++() {
    if (not has_next())
      throw iterator_position_error();
    _current = (*_current).next();
    return *this;
  }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator
  list<T, ThreadSafe>::iterator::operator--(int) {
    if (not has_previous())
      throw iterator_position_error();
    list<T, ThreadSafe>::iterator iter = *this;
    _current = (*_current).previous();
    return iter;
  }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator&
  list<T, ThreadSafe>::iterator::operator--() {
    if (not has_previous())
      throw iterator_position_error();
    _current = (*_current).previous();
    return *this;
  }

  template<typename T, bool ThreadSafe> bool
  list<T, ThreadSafe>::iterator::operator==
      (const list<T, ThreadSafe>::iterator& iter) const
    { return _current == iter._current; }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator&
  list<T, ThreadSafe>::iterator::current(
      const pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& ptr) {
    _current = ptr;
    return *this;
  }

  iterator_position_error::iterator_position_error() :
    std::runtime_error(typeutil::classname(typeid(iterator_position_error))) {}

  // list methods
  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::list() : _entry() {}

  template<typename T, bool ThreadSafe>
  list<T, ThreadSafe>::iterator list<T, ThreadSafe>::begin() const
    { return list<T, ThreadSafe>::iterator((*_entry)._first, _entry); }

  template<typename T, bool ThreadSafe>
  list<T, ThreadSafe>::iterator list<T, ThreadSafe>::end() const
    { return list<T, ThreadSafe>::iterator((*_entry)._last, _entry); }

  template<typename T, bool ThreadSafe> long long list<T, ThreadSafe>::length() const
    { return (*(*_entry)._capacity)._len; }
}

#endif //__LANGLIBS_COLLECTIONS_LIST_IMPL__
