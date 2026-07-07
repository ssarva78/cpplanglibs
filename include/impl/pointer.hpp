#ifndef __LANGLIBS_POINTER_IMPL__
#define __LANGLIBS_POINTER_IMPL__

#include<typeutil>
#include<atomic>

namespace lang {
  /** @cond */
  template <typename T, bool ThreadSafe> class pointernode {
    public:
      explicit pointernode(T* ptr): _ptr(ptr), _refcnt(1) {}
      ~pointernode() {
        -- _refcnt;
        if (_refcnt == 0) {
          delete _ptr;
        }
      }
      long long reference_count() const
        { return _refcnt; }
      void increment_reference()
        { ++ _refcnt; }
      T* reference_pointer() const
        { return _ptr; }
    private:
      T* _ptr;
      using reference_count_t
          = std::conditional_t<ThreadSafe, std::atomic<long long>, long long>;
      reference_count_t _refcnt;
      pointernode(const pointernode<T, ThreadSafe>&) = delete;
      pointernode<T, ThreadSafe>& operator=(const pointernode<T, ThreadSafe>&) = delete;
  };
  /** @endcond */

  template<typename T, bool ThreadSafe> template <typename... TArgs>
  pointer<T, ThreadSafe>::pointer(TArgs... args)
      : _val(new pointernode<T, ThreadSafe>(new T(args...))) {}

  template<typename T, bool ThreadSafe>
  pointer<T, ThreadSafe>::pointer(const pointer<T, ThreadSafe>& right)
      : _val(right._val) {
    if (not right.isnull()) {
      _val -> increment_reference();
    }
  }

  template<typename T, bool ThreadSafe>
  pointer<T, ThreadSafe>::pointer(pointer<T, ThreadSafe>&& rvalue)
      : _val(rvalue._val)
    { rvalue._val = nullptr; }

  template<typename T, bool ThreadSafe>
  void pointer<T, ThreadSafe>::delete_reference() {
    if (not isnull()) {
      _val -> ~pointernode();
    }
  }

  template<typename T, bool ThreadSafe> pointer<T, ThreadSafe>::~pointer() {
    delete_reference();
    _val = nullptr;
  }

  template<typename T, bool ThreadSafe>
  pointer<T, ThreadSafe>& pointer<T, ThreadSafe>::operator =
      (const pointer<T, ThreadSafe>& right) {
    delete_reference();
    _val = right._val;
    if (not right.isnull()) {
      _val -> increment_reference();
    }
    return *this;
  }

  template<typename T, bool ThreadSafe>
  pointer<T, ThreadSafe>& pointer<T, ThreadSafe>::operator =
      (pointer<T, ThreadSafe>&& rvalue) {
    delete_reference();
    _val = rvalue._val;
    rvalue._val = nullptr;
    return *this;
  }

  template<typename T, bool ThreadSafe>
  const T& pointer<T, ThreadSafe>::operator *() const {
    if (isnull())
      throw nullpointer_error();
    return *(_val -> reference_pointer());
  }

  template<typename T, bool ThreadSafe>
  T& pointer<T, ThreadSafe>::operator *() {
    if (isnull())
      throw nullpointer_error();
    return *(_val -> reference_pointer());
  }

  template<typename T, bool ThreadSafe>
  long long pointer<T, ThreadSafe>::references() const {
    if (isnull())
      throw nullpointer_error();
    return _val -> reference_count();
  }

  template<typename T, bool ThreadSafe>
  bool pointer<T, ThreadSafe>::isnull() const
    { return _val == nullptr; }

  template<typename T, bool ThreadSafe>
  const pointer<T, ThreadSafe> pointer<T, ThreadSafe>::null()
    { return pointer<T, ThreadSafe>((const void*)nullptr); }

  template<typename T, bool ThreadSafe>
  pointer<T, ThreadSafe>::operator bool() const
    { return not isnull(); }

  template<typename T, bool ThreadSafe>
  bool pointer<T, ThreadSafe>::operator ==
      (const pointer<T, ThreadSafe>& right) const {
    return reinterpret_cast<long long>(_val)
        == reinterpret_cast<long long>(right._val);
  }

  template<typename T, bool ThreadSafe> template<typename K, typename... KArgs>
  pointer<T, ThreadSafe> pointer<T, ThreadSafe>::dynamic(KArgs... args) {
    static_assert(std::is_base_of<T, K>::value, "Not a derived class");
    return pointer<T, ThreadSafe>(new pointernode<T, ThreadSafe>((T*)new K(args...)));
  }

  template<typename T, bool ThreadSafe> template<bool NewThreadSafety>
  pointer<T, NewThreadSafety> pointer<T, ThreadSafe>::clone() const {
    static_assert(std::is_copy_constructible<T>::value
        and std::is_copy_assignable<T>::value,
      "Template type should have copy constructor and copy assignment");
    return pointer<T, NewThreadSafety>(*_val -> reference_pointer());
  }

  nullpointer_error::nullpointer_error() :
    std::runtime_error(typeutil::classname(typeid(nullpointer_error))) {}
}

#endif //__LANGLIBS_POINTER_IMPL__

