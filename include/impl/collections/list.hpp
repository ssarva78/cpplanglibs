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

      static pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& link(
          pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& prev,
          pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& new_node) {
        if (not prev) {
          return new_node;
        }
        (*new_node).previous() = prev;
        pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe> &nxt = (*prev).next();
        (*new_node).next() = nxt;
        if (nxt) {
          (*nxt).previous() = new_node;
        }
        (*prev).next() = new_node;
        return new_node;
      }

      pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe> unlink() {
        auto to_return = previous()? previous() : next();
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
    pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe> _first, _last;
    linkedlistentry() : _capacity(), _first(), _last() {
      (*_first).next() = _last;
      (*_last).previous() = _first;
    }
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
      const pointer<linkedlistcapacity<T, ThreadSafe>, ThreadSafe>& capacity) :
      _current(node), _capacity(capacity)
    {}

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator&
  list<T, ThreadSafe>::iterator::add(const pointer<T, ThreadSafe>& ptr) {
    if (not has_next())
      throw iterator_position_error();
    if ((*_capacity)._len == 0) {
      **_current = ptr;
    } else {
      pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe> newnode(ptr);
      linkedlistnode<T, ThreadSafe>::link(_current, newnode);
      _current = newnode;
    }
    ++ (*_capacity)._len;
    return *this;
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
    { return list<T, ThreadSafe>::iterator((*_entry)._first, (*_entry)._capacity); }

  template<typename T, bool ThreadSafe>
  list<T, ThreadSafe>::iterator list<T, ThreadSafe>::end() const
    { return list<T, ThreadSafe>::iterator((*_entry)._last, (*_entry)._capacity); }

  template<typename T, bool ThreadSafe> nullable<T, ThreadSafe>
  list<T, ThreadSafe>::remove(list<T, ThreadSafe>::iterator& iter) {
    if (length() == 0) {
      throw iterator_position_error();
    }
    nullable<T, ThreadSafe> return_value = *iter;
    if (length() == 1) {
      pointer<T, ThreadSafe>& val = **(*_entry)._first;
      val = pointer<T, ThreadSafe>::null();
    } else {
      auto node_after_delete = (*iter._current).unlink();
      if (iter == begin()) {
        (*_entry)._first = node_after_delete;
      }
      iter.current(node_after_delete);
    }
    --(*(*_entry)._capacity)._len;
    return return_value;
  }

  template<typename T, bool ThreadSafe> long long list<T, ThreadSafe>::length() const
    { return (*(*_entry)._capacity)._len; }
}

#endif //__LANGLIBS_COLLECTIONS_LIST_IMPL__
