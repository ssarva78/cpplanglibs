#ifndef __LANGLIBS_COLLECTIONS_LIST_IMPL__
#define __LANGLIBS_COLLECTIONS_LIST_IMPL__

namespace lang::collections {
  template<typename T, bool ThreadSafe> class linkedlistnode {
    public:
      linkedlistnode() :
        _val(pointer<T, ThreadSafe>::null()),
        _next(pointer<linkedlistnode<T, ThreadSafe>>::null()),
        _previous(pointer<linkedlistnode<T, ThreadSafe>>::null())
        {}
      explicit linkedlistnode(const pointer<T>& ptr) :
        _val(ptr),
        _next(pointer<linkedlistnode<T, ThreadSafe>>::null()),
        _previous(pointer<linkedlistnode<T, ThreadSafe>>::null())
        {}

      const pointer<T>& operator*() const { return _val; }
      pointer<T>& operator*() { return _val; }

      const pointer<linkedlistnode<T, ThreadSafe>>& next() const
        { return _next; }
      pointer<linkedlistnode<T, ThreadSafe>>& next()
        { return _next; }

      const pointer<linkedlistnode<T, ThreadSafe>>& previous() const
        { return _previous; }
      pointer<linkedlistnode<T, ThreadSafe>>& previous()
        { return _previous; }

      static pointer<linkedlistnode<T, ThreadSafe>>& link(
          pointer<linkedlistnode<T, ThreadSafe>>& prev,
          pointer<linkedlistnode<T, ThreadSafe>>& new_node) {
        if (not prev) {
          return new_node;
        }
        (*new_node).previous() = prev;
        pointer<linkedlistnode<T, ThreadSafe>> &nxt = (*prev).next();
        (*new_node).next() = nxt;
        if (nxt) {
          (*nxt).previous() = new_node;
        }
        (*prev).next() = new_node;
        return new_node;
      }

      pointer<linkedlistnode<T, ThreadSafe>>& unlink() {
        pointer<linkedlistnode<T, ThreadSafe>> &nxt = next();
        pointer<linkedlistnode<T, ThreadSafe>> &prev = previous();
        if (nxt) {
          (*nxt).previous() = prev;
        }
        if (prev) {
          (*prev).next() = nxt;
        }
        next() = previous() = pointer<linkedlistnode<T, ThreadSafe>>::null();
        return prev;
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
  list<T, ThreadSafe>::iterator::operator*() const
    { return nullable<T, ThreadSafe>(**_current); }

  template<typename T, bool ThreadSafe> nullable<T, ThreadSafe>
  list<T, ThreadSafe>::iterator::operator*()
    { return nullable<T, ThreadSafe>(**_current); }

  template<typename T, bool ThreadSafe> bool
  list<T, ThreadSafe>::iterator::has_next() const {
    const pointer<linkedlistnode<T, ThreadSafe>, ThreadSafe>& next = (*_current).next();
    return not next.isnull() and not (*next).next().isnull();
  }

  template<typename T, bool ThreadSafe> bool
  list<T, ThreadSafe>::iterator::has_previous() const
    { return not (*_current).previous().isnull(); }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator
  list<T, ThreadSafe>::iterator::operator++(int) {
    list<T, ThreadSafe>::iterator iter = *this;
    _current = (*_current).next();
    return iter;
  }

  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::iterator&
  list<T, ThreadSafe>::iterator::operator++() {
    _current = (*_current).next();
    return *this;
  }

  iterator_position_error::iterator_position_error() :
    std::runtime_error(typeutil::classname(typeid(iterator_position_error))) {}

  // list methods
  template<typename T, bool ThreadSafe> list<T, ThreadSafe>::list() : _entry() {}

  template<typename T, bool ThreadSafe>
  list<T, ThreadSafe>::iterator list<T, ThreadSafe>::begin() const {
    return list<T, ThreadSafe>::iterator((*_entry)._first, (*_entry)._capacity);
  }

  template<typename T, bool ThreadSafe> long long list<T, ThreadSafe>::length() const
    { return (*(*_entry)._capacity)._len; }
}

#endif //__LANGLIBS_COLLECTIONS_LIST_IMPL__
