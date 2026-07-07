#include <unittest>
#include <collections>

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
        expect<bool>((*iter2).isnull()).istrue();
        expect<int>((*iter2).or_else(-1)).is(-1);
      }
    )

    .test(
      "Test list iterator forward iteration",
      __testfunc__ {
        list<int> lst;
        list<int>::iterator iter = lst.begin();
        expect<bool>(iter.has_next()).istrue();
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
        expect<function_block>(__testfunc__ {*iter--;}).throws(typeid(iterator_position_error));

        expect<int>(*iter).is(2);
        expect<int>(*(iter--)).is(2);
        expect<int>(*iter).is(1);
        expect<function_block>(__testfunc__ {iter--;}).throws(typeid(iterator_position_error));

        iter = lst.end();
        expect<int>(*(--iter)).is(2);
        expect<int>(*iter).is(2);
        expect<int>(*(--iter)).is(1);

        iter = lst.end();
        int i = 0;
        for (; iter.has_previous(); iter--, i++) {}
        expect<int>(i).is(lst.length());
        expect<int>(*iter).is(1);
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
        expect<bool>(lst.begin() == iter1).istrue();
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
        expect<function_block>(__testfunc__{lst.remove(iter);}).throws(typeid(iterator_position_error));
        expect<int>(lst.length()).is(0);
        iter.add(1);
        expect<int>(lst.length()).is(1);
        expect<int>(lst.remove(iter)).is(1);
        expect<int>(lst.length()).is(0);

        iter = lst.begin();
        iter.add(1).add(2).add(3);
        expect<int>(lst.length()).is(3);
        iter = lst.begin();

        expect<int>(lst.remove(iter)).is(1);
        expect<int>(lst.length()).is(2);
        expect<int>(*iter).is(2);
        expect<bool>(iter == lst.begin()).istrue();
        expect<int>(*lst.begin()).is(2);

        expect<int>(lst.remove(iter)).is(2);
        expect<int>(lst.length()).is(1);
        expect<int>(*iter).is(3);

        expect<int>(lst.remove(iter)).is(3);
        expect<int>(lst.length()).is(0);

        lst.begin().add(1).add(2);
        expect<int>(lst.length()).is(2);
        iter = lst.end();
        expect<function_block>(__testfunc__{lst.remove(iter);}).throws(typeid(iterator_position_error));

        --iter;
        expect<int>(lst.remove(iter)).is(2);
        expect<int>(lst.length()).is(1);
        expect<int>(*iter).is(1);

        expect<int>(lst.remove(iter)).is(1);
        expect<int>(lst.length()).is(0);

        lst.begin().add(10).add(20).add(30);
        iter = ++ lst.begin();
        expect<int>(lst.remove(iter)).is(20);
        expect<int>(lst.length()).is(2);
        iter = lst.begin();
        expect<int>(*iter++).is(10);
        expect<int>(*iter).is(30);
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

