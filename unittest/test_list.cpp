#include <unittest>
#include <collections>

using namespace cppunittest;
using namespace lang;
using namespace lang::collections;

/*
struct pointer_memory_trace {
  char st_type;
  long long st_memory_location;
  size_t st_alloc_size;
  std::string st_type_name;
};

pointer_memory_trace parse(const std::string& str) {
  std::istringstream ss(str);
  pointer_memory_trace trace;
  std::getline(ss, trace.st_type, '/');
  std::string addr;
  std::getline(ss, addr, '/');
  trace.st_memory_location = std::stoll(addr);
  std::string sz;
  std::getline(ss, sz, '/');
}
*/

int run_unittest() {
  unittest ut("Test list", __FILE__);

  ut
    /*
    .test(
      "Test list add to iterator",
      __testfunc__ {
        list<int> lst;
        list<int>::iterator iter = lst.begin();
        iter.add(5);
        expect<long long>(lst.length()).is(1);
        expect<int>(*iter).is(5);
        iter.add(6);
        expect<long long>(lst.length()).is(2);
        expect<int>(*iter).is(6);
        expect<int>(*lst.begin()).is(5);
      }
    )

    .test(
      "Test list iterator reference operator",
      __testfunc__ {
        list<int> lst;
        list<int>::iterator iter = lst.begin();
        iter.add(5).add(6);
        nullable<int> item1 = *lst.begin();
        *item1 = 1;
        expect<int>(*lst.begin()).is(1);
        **lst.begin() = 2;
        expect<int>(*lst.begin()).is(2);

        list<int> lst2;
        list<int>::iterator iter2 = lst2.begin();
        expect<function_block>(__testfunc__{auto x = *iter2;}).throws(typeid(iterator_position_error));
      }
    )

    .test(
      "Test list iterator forward iteration",
      __testfunc__ {
        list<int> lst;
        list<int>::iterator iter = lst.begin();
        expect<bool>(iter.has_next()).isfalse();
        expect<bool>(iter.has_previous()).isfalse();

        iter.add(1);
        expect<bool>(iter.has_next()).istrue();
        expect<bool>(iter.has_previous()).isfalse();

        iter.add(2);
        expect<bool>(iter.has_next()).istrue();
        expect<bool>(iter.has_previous()).istrue();

        iter = lst.begin();
        expect<bool>(iter.has_previous()).isfalse();

        expect<int>(*(iter++)).is(1);
        expect<int>(*iter).is(2);

        iter = lst.begin();
        int i = 0;
        // cppcheck-suppress postfixOperator
        for (; iter.has_next(); iter++, i++) {}
        expect<int>(i).is(lst.length());

        iter = lst.begin();
        expect<int>(*(++iter)).is(2);
        expect<int>(*iter).is(2);

        iter = lst.begin();
        i = 0;
        for (; iter.has_next(); ++iter, i++) {}
        expect<int>(i).is(lst.length());

        expect<function_block>(__testfunc__ {*iter;}).throws(typeid(iterator_position_error));
        // cppcheck-suppress postfixOperator
        expect<function_block>(__testfunc__ {iter++;}).throws(typeid(iterator_position_error));
        expect<function_block>(__testfunc__ {++iter;}).throws(typeid(iterator_position_error));
      }
    )

    .test(
      "Test list iterator reverse iteration",
      __testfunc__ {
        list<int> lst;
        list<int>::iterator iter = lst.begin();
        iter.add(1).add(2);

        iter = lst.end();
        expect<bool>(iter.has_previous()).istrue();
        expect<bool>(iter.has_next()).isfalse();
        expect<bool>(iter).isfalse();
        expect<function_block>(__testfunc__ {*iter;}).throws(typeid(iterator_position_error));
        expect<function_block>(__testfunc__ {*(iter--);}).throws(typeid(iterator_position_error));

        expect<int>(*iter).is(2);
        expect<int>(*(iter--)).is(2);
        expect<int>(*iter).is(1);
        // cppcheck-suppress postfixOperator
        expect<function_block>(__testfunc__ {iter--;}).throws(typeid(iterator_position_error));

        iter = lst.end();
        expect<int>(*(--iter)).is(2);
        expect<int>(*iter).is(2);
        expect<int>(*(--iter)).is(1);

        iter = lst.end();
        int i = 0;
        // cppcheck-suppress postfixOperator
        for (; iter.has_previous(); iter--, i++) {}
        expect<int>(i).is(lst.length());
        expect<int>(*iter).is(1);
        // cppcheck-suppress postfixOperator
        expect<function_block>(__testfunc__ {iter--;}).throws(typeid(iterator_position_error));
        expect<function_block>(__testfunc__ {--iter;}).throws(typeid(iterator_position_error));

        iter = lst.end();
        i = 0;
        for (; iter.has_previous(); --iter, i++) {}
        expect<int>(i).is(lst.length());
        expect<int>(*iter).is(1);
      }
    )

    .test(
      "Test range iteration",
      __testfunc__ {
        list<int> lst;
        lst.begin().add(1).add(2).add(3);

        list<int>::iterator iter1 = lst.begin();
        // cppcheck-suppress knownConditionTrueFalse
        expect<bool>(lst.begin() == iter1).istrue();
        // cppcheck-suppress knownConditionTrueFalse
        expect<bool>(lst.begin() != iter1).isfalse();

        ++iter1;
        expect<int>(*iter1).is(2);
        expect<bool>(lst.begin() == iter1).isfalse();
        expect<bool>(lst.begin() != iter1).istrue();
        expect<bool>(lst.begin() == lst.end()).isfalse();
        expect<bool>(lst.begin() != lst.end()).istrue();

        ++iter1; ++iter1;
        expect<bool>(lst.end() == iter1).istrue();
        expect<bool>(lst.end() != iter1).isfalse();

        --iter1;
        iter1.add(pointer<int>::null()).add(5);

        int i = 0;
        for (auto v : lst) {
          expect<int>(v.or_else(-1)).is(++i==4? -1: i);
        }
      }
    )

    .test(
      "Add at terminal node",
      __testfunc__ {
        list<int> lst;
        lst.begin().add(1);
        list<int>::iterator iter = lst.end();
        expect<function_block>(__testfunc__ {iter.add(2);}).throws(typeid(iterator_position_error));
      }
    )

    .test(
      "Remove from iterator",
      __testfunc__ {
        list<int> lst;
        list<int>::iterator iter = lst.begin();
        expect<function_block>(__testfunc__{iter.remove();}).throws(typeid(iterator_position_error));
        expect<int>(lst.length()).is(0);
        iter.add(1);
        expect<int>(lst.length()).is(1);
        expect<int>(iter.remove()).is(1);
        expect<int>(lst.length()).is(0);

        iter = lst.begin();
        iter.add(1).add(2).add(3);
        expect<int>(lst.length()).is(3);
        iter = lst.begin();

        expect<int>(iter.remove()).is(1);
        expect<int>(lst.length()).is(2);
        expect<int>(*iter).is(2);
        expect<bool>(iter == lst.begin()).istrue();
        expect<int>(*lst.begin()).is(2);

        expect<int>(iter.remove()).is(2);
        expect<int>(lst.length()).is(1);
        expect<int>(*iter).is(3);

        expect<int>(iter.remove()).is(3);
        expect<int>(lst.length()).is(0);

        lst.begin().add(1).add(2);
        expect<int>(lst.length()).is(2);
        iter = lst.end();
        expect<function_block>(__testfunc__{iter.remove();}).throws(typeid(iterator_position_error));

        --iter;
        expect<int>(iter.remove()).is(2);
        expect<int>(lst.length()).is(1);
        expect<int>(*--iter).is(1);

        expect<int>(iter.remove()).is(1);
        expect<int>(lst.length()).is(0);

        lst.begin().add(10).add(20).add(30);
        iter = ++ lst.begin();
        expect<int>(*iter).is(20);
        expect<int>(iter.remove()).is(20);
        expect<int>(lst.length()).is(2);
        iter = lst.begin();
        expect<int>(*iter++).is(10);
        expect<int>(*iter).is(30);
      }
    )

    .test(
      "Test add initializer list to iterator",
      __testfunc__ {
        list<int> lst;
        auto iter = lst.begin();
        iter.add({1,2,3});
        expect<long long>(lst.length()).is(3);
        expect<int>(*iter).is(3);
        int i = 1;
        for (auto item : lst) {
          expect<int>(item).is(i++);
        }
      }
    )

    .test(
      "Test add from another list to iterator",
      __testfunc__ {
        list<int> lst;
        list<int> lst1;
        lst1.begin().add({1,2,3});
        concurrent_list<int> lst2;
        lst2.begin().add({4,5,6});

        lst.begin().add(lst1).add(lst2);
        expect<long long>(lst.length()).is(6);
        int i = 1;
        for (auto item : lst) {
          expect<int>(item).is(i++);
        }

        auto it = lst1.begin();
        **it = 9;
        expect<int>(*lst1.begin()).is(9);
        expect<int>(*lst.begin()).is(9);

        auto it2 = lst2.begin();
        **it2 = 8;
        expect<int>(*lst2.begin()).is(8);
        it = lst.begin();
        ++it;++it;++it;
        expect<int>(*it).is(4); //since lst and lst2 are different types,
                                // only value, not reference, is pushed to lst from lst2
      }
    )

    .test(
      "Test clear and validate list is useable after clear",
      __testfunc__ {
        list<int> lst;
        lst.clear();
        lst.begin().add(1).add(2);
        expect<long long>(lst.length()).is(2);
        lst.clear();
        expect<long long>(lst.length()).is(0);
        lst.begin().add(1).add(2);
        int i = 1;
        for (auto item : lst) {
          expect<int>(item).is(i++);
        }
      }
    )
    */

    .test(
      "Add before current node in iterator",
      __testfunc__ {
        list<int> lst;
        lst.begin().add_before(0);
        expect<long long>(lst.length()).is(1);
        auto iter = lst.begin();
        expect<int>(*iter).is(0);
        lst.end().add_before(3);
        expect<long long>(lst.length()).is(2);
        expect<int>(*--lst.end()).is(3);
        lst.begin().add(1).add(2);
        expect<long long>(lst.length()).is(4);
        int i = 0;
        for(auto item : lst) {
          expect<int>(*item).is(i++);
        }
      }
    )

    .test(
      "Test memory leak",
      __testfunc__ {
        std::stringstream ss;
        pointer<int>::enable_memory_trace(ss);
        {
          list<int> lst;
          auto iter = lst.begin();
          iter.add(1).add(3).add(4);
          iter = lst.begin();
          iter.add(2);
          //while (iter.has_next()) iter.remove();
          //iter.add(3);
        }
        pointer<int>::disable_memory_trace();
        std::cout << ss.str() << std::endl;
      }
    )

    ;

  return ut.error() + ut.failure();
}

int main() {
  try {
    return run_unittest();
  } catch(...) {
    return -1;
  }
}

