#ifndef __LANGLIBS_TYPEUTIL_IMPL__
#define __LANGLIBS_TYPEUTIL_IMPL__

#include <string>
#include <typeinfo>
#include <iostream>

#ifdef __GNUG__
#include<cxxabi.h>
#endif

namespace lang {
  namespace typeutil {

    template<typename T> std::string classname(const T& o)
      { return classname(typeid(o)); }

    std::string classname(const std::type_info& t) {
      #ifdef __GNUG__
        int status;
        char *nm = abi::__cxa_demangle(t.name(), NULL, NULL, &status);
        std::string c_nm(status == 0? nm : t.name());
        free(nm);
        return c_nm;
      #else
        return std::string(t.name());
      #endif
    }

  }; //namespace typeutil
}; //namespace lang

#endif //__LANGLIBS_TYPEUTIL_IMPL__

