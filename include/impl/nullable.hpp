#ifndef __LANGLIBS_NULLABLE_IMPL__
#define __LANGLIBS_NULLABLE_IMPL__

namespace lang {
  template<typename T> nullable<T>::nullable() : _ptr(pointer<T>::null()) {}

  template<typename T> nullable<T>::nullable(const pointer<T>& ptr) : _ptr(ptr) {}

  template<typename T> const T& nullable<T>::operator *() const
    { return *_ptr; }

  template<typename T> T& nullable<T>::operator *()
    { return *_ptr; }

  template<typename T> bool nullable<T>::isnull() const
    { return _ptr.isnull(); }

  template<typename T> nullable<T>::operator bool() const
    { return not _ptr.isnull(); }

  template<typename T> T nullable<T>::or_else(const T& value) const
    { return isnull()? value : *_ptr; }

  template<typename T> T nullable<T>::or_else(std::function<T()> func) const
    { return isnull()? func() : *_ptr; }

  template<typename T> const pointer<T>& nullable<T>::operator &() const
    { return _ptr; }

  template<typename T> pointer<T>& nullable<T>::operator &()
    { return _ptr; }

};

#endif //__LANGLIBS_NULLABLE_IMPL__
