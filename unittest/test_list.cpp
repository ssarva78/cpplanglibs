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
        expect<bool>(iter.has_next()).isfalse();
        expect<bool>(iter.has_previous()).isfalse();

        iter.add(1);
        expect<bool>(iter.has_next()).isfalse();
        expect<bool>(iter.has_previous()).isfalse();

        iter.add(2);
        expect<bool>(iter.has_next()).isfalse();
        expect<bool>(iter.has_previous()).istrue();

        iter = lst.begin();
        expect<bool>(iter.has_next()).istrue();
        expect<bool>(iter.has_previous()).isfalse();

        expect<int>(*(iter++)).is(1);
        expect<int>(*iter).is(2);

        iter = lst.begin();
        int i = 0;
        for (; iter.has_next(); iter++, i++) {}
        expect<int>(i).is(lst.length() - 1);

        iter = lst.begin();
        expect<int>(*(++iter)).is(2);
        expect<int>(*iter).is(2);

        iter = lst.begin();
        i = 0;
        for (; iter.has_next(); ++iter, i++) {}
        expect<int>(i).is(lst.length() - 1);

        expect<function_block>(__testfunc__ {iter++;}).throws(typeid(iterator_position_error));
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

