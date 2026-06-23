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

  template<typename T, bool ThreadSafe> bool nullable<T, ThreadSafe>::isnull() const
    { return _ptr.isnull(); }

  template<typename T, bool ThreadSafe> nullable<T, ThreadSafe>::operator bool() const
    { return not _ptr.isnull(); }

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

};

#endif //__LANGLIBS_NULLABLE_IMPL__
