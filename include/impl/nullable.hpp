#ifndef __LANGLIBS_NULLABLE_IMPL__
#define __LANGLIBS_NULLABLE_IMPL__

namespace lang {
  template<typename T, bool ThreadSafe>
  nullable<T, ThreadSafe>::nullable() : _ptr(pointer<T, ThreadSafe>::null()) {}

  template<typename T, bool ThreadSafe>
  nullable<T, ThreadSafe>::nullable(const pointer<T, ThreadSafe>& ptr) : _ptr(ptr) {}

  template<typename T, bool ThreadSafe>
  const T& nullable<T, ThreadSafe>::operator *() const
    { return *_ptr; }

  template<typename T, bool ThreadSafe> T& nullable<T, ThreadSafe>::operator *()
    { return *_ptr; }

  template<typename T, bool ThreadSafe> nullable<T, ThreadSafe>::operator T() const
    { return *_ptr; }

  template<typename T, bool ThreadSafe> bool nullable<T, ThreadSafe>::isnull() const
    { return _ptr.isnull(); }

  template<typename T, bool ThreadSafe>
  T nullable<T, ThreadSafe>::or_else(const T& value) const
    { return isnull()? value : *_ptr; }

  template<typename T, bool ThreadSafe>
  T nullable<T, ThreadSafe>::or_else(std::function<T()> func) const
    { return isnull()? func() : *_ptr; }

  template<typename T, bool ThreadSafe> template<typename E, typename... EArgs>
  T nullable<T, ThreadSafe>::or_throw(EArgs... args) {
    if (not isnull())
      return *_ptr;
    throw E(args...);
  }

  template<typename T, bool ThreadSafe> template<typename F>
  auto nullable<T, ThreadSafe>::map(F func) const {
    static_assert(std::is_invocable_v<F, T&>,
        "Argument must be function");
    typename std::invoke_result<F, T&>::type t;
    static_assert(is_nullable_type<decltype(t)>::value,
        "Callable return type should be nullable");
    if (isnull()) {
      return t;
    }
    return func(operator*());
  }

};

#endif //__LANGLIBS_NULLABLE_IMPL__
