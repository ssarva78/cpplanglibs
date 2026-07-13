#include <unittest>
#include <collections>
#include <thread>

using namespace cppunittest;
using namespace lang;
using namespace lang::collections;

int run_unittest() {
  unittest ut("Test list", __FILE__);

  ut
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
        expect<int>(i).is(lst.length()+1);
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
        expect<int>(i).is(lst.length()+1);

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
        expect<int>(i).is(lst.length()+1);
      }
    )

    .test(
      "Add before current node in iterator",
      __testfunc__ {
        list<int> lst;
        lst.begin().add_before(0);
        expect<bool>(lst.begin() == lst.end()).isfalse();
        expect<long long>(lst.length()).is(1);
        auto iter = lst.begin();
        expect<int>(*iter).is(0);

        lst.end().add_before(3);
        expect<long long>(lst.length()).is(2);
        ++iter;
        expect<int>(*iter).is(3);

        expect<int>(*--lst.end()).is(3);
        lst.begin().add(1).add(2);
        expect<long long>(lst.length()).is(4);
        int i = 0;
        for(auto item : lst) {
          expect<int>(*item).is(i++);
        }
        expect<int>(i).is(lst.length());
      }
    )

    .test(
      "Test memory leak",
      __testfunc__ {
        std::stringstream ss;
        pointer_memory_trace::enable(ss);
        {
          list<int> lst;
          lst.begin().add_before(0);
          lst.end().add_before(3);
          lst.begin().add(1).add(2);
          auto iter = lst.end();
          --iter;
          iter.remove();
          --iter;
          iter.add(3);
          auto iter2 = iter--;
          expect<long long>(lst.length()).is(4);
          int i = 0;
          for(auto item : lst) {
            expect<int>(*item).is(i++);
          }
          expect<int>(i).is(4);
        }
        pointer_memory_trace::disable();
        std::string tok;
        list<pointer_memory_trace> constructors, destructors;
        auto iter_constr = constructors.begin();
        auto iter_destr = destructors.begin();
        while(std::getline(ss, tok)) {
          pointer_memory_trace trace(tok);
          if (trace.st_type == 'C') iter_constr.add(trace);
          else iter_destr.add(trace);
        }
        expect<long long>(constructors.length()).is(13);
        expect<long long>(constructors.length()).is(destructors.length());
        for (auto item_c : constructors) {
          bool found = false;
          for (auto item_d : destructors) {
            if ((*item_c).st_memory_location == (*item_d).st_memory_location) {
              found = true;
              break;
            }
          }
          expect<bool>(found).istrue(); //matching destructor is found
        }
      }
    )

    .test(
      "Test concurrent add",
      __testfunc__ {
        concurrent_list<int> clist;
        auto iter = clist.begin();
        for (int i = 0; i < 100000; i++) {
          iter.add(i);
        }

        auto additems = [&](int start) -> void {
          auto it = --clist.end();
          for (int i = 0; i < 50000; i++)
            it.add(start+i);
        };
        auto run1 = [&]() -> void {additems(100000);};
        auto run2 = [&]() -> void {additems(150000);};
        std::thread t1(run1), t2(run2);
        t1.join();
        t2.join();
        std::cout << clist.length() << std::endl;
        expect<long long>(clist.length()).is(200000);

        auto lst1 = clist.filter([&](const concurrent_nullable<int>& v) -> bool {
          return ((int)v < 150000);
        });
        auto lst2 = clist.filter([&](const concurrent_nullable<int>& v) -> bool {
          return ((int)v >= 150000);
        });
        int i = 0;
        for(auto item : lst1) {
          expect<int>(*item).is(i++);
        }
        i = 150000;
        for(auto item : lst2) {
          expect<int>(*item).is(i++);
        }
      }
    )

    .test(
      "Test concurrent remove",
      __testfunc__ {
        concurrent_list<int> clist;
        auto iter = clist.begin();
        for (int i = 0; i < 100000; i++) {
          iter.add(i);
        }

        auto pop = [&]() -> void {
          for (int i = 0; i < 40000; i++)
            (clist.begin()).remove();
        };
        std::thread t1(pop), t2(pop);
        t1.join();
        t2.join();
        expect<long long>(clist.length()).is(20000);

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

