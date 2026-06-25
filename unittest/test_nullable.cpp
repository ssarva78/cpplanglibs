#include <unittest>
#include <nullable>

using namespace cppunittest;
using namespace lang;

class mynullabletestclass {
  public:
    explicit mynullabletestclass(const std::string& s) : _str(s) {}
    const std::string& strval() const { return _str; }
    void strval(const std::string& s) { _str = s; }
  private:
    std::string _str;
};

int run_unittest() {
  unittest ut("Test nullable", __FILE__);

  ut
    .test(
      "Create a null valued nullable",
      __testfunc__ {
        nullable<mynullabletestclass> opt;
        expect<bool>(opt.isnull()).istrue();
      }
    )

    .test(
      "Create nullable with non-null object",
      __testfunc__ {
        nullable<mynullabletestclass> opt(pointer<mynullabletestclass>("hello"));
        expect<std::string>((*opt).strval()).is("hello");

        const nullable<mynullabletestclass> opt2(pointer<mynullabletestclass>("hello"));
        expect<std::string>((*opt2).strval()).is("hello");

        concurrent_nullable<int> copt(5);
        expect<int>(*copt).is(5);

        nullable<mynullabletestclass> opt3(mynullabletestclass("hello world"));
        expect<std::string>((*opt3).strval()).is("hello world");
      }
    )

    .test(
      "Return the value or a default value from nullable",
      __testfunc__ {
        nullable<mynullabletestclass> opt_null;
        expect<std::string>(opt_null.or_else(mynullabletestclass("default value")).strval()).is("default value");

        mynullabletestclass mc("another value");
        expect<std::string>(opt_null.or_else(mc).strval()).is("another value");

        nullable<mynullabletestclass> opt2(pointer<mynullabletestclass>("hello"));
        expect<std::string>(opt2.or_else(mc).strval()).is("hello");
      }
    )

    .test(
      "Return the value or the output of a getter function",
      __testfunc__ {
        nullable<mynullabletestclass> opt_null;
        auto getter = [&]() -> mynullabletestclass { return mynullabletestclass("default value"); };
        expect<std::string>(opt_null.or_else(getter).strval()).is("default value");

        nullable<mynullabletestclass> opt2("hello"); //implicit constructor to pointer
        expect<std::string>(opt2.or_else(getter).strval()).is("hello");
      }
    )

    .test(
      "Validate cast operator",
      __testfunc__ {
        nullable<mynullabletestclass> opt("hello");
        mynullabletestclass mc("world");
        mc = opt;
        expect<std::string>(mc.strval()).is("hello");
        mc.strval("hello world");
        expect<std::string>((*opt).strval()).is("hello");
        // mynullabletestclass &mc2 = opt; // this throws compilation error
                                           // because cast returns const reference
        const mynullabletestclass &mc2 = opt;
        expect<std::string>(mc2.strval()).is("hello");
        auto fn = [&](const mynullabletestclass& c) -> std::string
            { return c.strval(); };
        expect<std::string>(fn(opt)).is("hello");
      }
    )

    .test(
      "Throws null pointer error on retrieving value from empty nullable",
      __testfunc__ {
        nullable<int> opt_null;
        expect<function_block>(__testfunc__{*opt_null;}).throws(
          typeid(lang::nullpointer_error));
      }
    )

    .test(
      "Throw exception when nullable has no value",
      __testfunc__ {
        nullable<int> opt_null;
        expect<function_block>(__testfunc__{opt_null.or_throw<std::runtime_error>("nullable is null");})
            .throws(typeid(std::runtime_error), "nullable is null");

        nullable<int> opt2(4);
        expect<int>(opt2.or_throw<std::exception>()).is(4);
      }
    )

    .test(
      "Transform value in nullable with a call to the function",
      __testfunc__ {
        nullable<std::string> opt("hello");
        auto fn2 = [&](const std::string& s) -> nullable<int> { return nullable<int>(s.length()); };
        expect<int>(*opt.map(fn2)).is(std::string("hello").length());

        nullable<std::string> opt2;
        expect<bool>(opt2.map(fn2).isnull()).istrue();
        expect<int>(opt2.map(fn2).or_else(-1)).is(-1);
      }
    )

    .test(
      "Validate assignment and move to nullable",
      __testfunc__ {
        nullable<int> opt1(5);
        nullable<int> opt2;
        opt2 = opt1;
        expect<int>(opt2).is(5);
        *opt1 = 10;
        expect<int>(opt2).is(10);

        nullable<int> opt3;
        opt3 = std::move(opt1);
        expect<int>(opt3).is(10);
        expect<int>(opt2).is(10);
        expect<bool>(opt1.isnull()).istrue();
      }
    )

    .test(
      "Validate comparison operators",
      __testfunc__ {
        nullable<int> opt1(5), opt2(6), opt3(5);
        expect<bool>(opt1 == opt3).istrue();
        expect<bool>(opt1 == 5).istrue();
        expect<bool>(5 == opt1).istrue();
        expect<bool>(opt1 != opt2).istrue();
        expect<bool>(opt1 != 1).istrue();
        expect<bool>(1 != opt1 ).istrue();
        expect<bool>(opt1 < opt2).istrue();
        expect<bool>(opt1 < 8).istrue();
        expect<bool>(1 < opt2).istrue();
        expect<bool>(opt1 <= opt2).istrue();
        expect<bool>(opt1 <= 8).istrue();
        expect<bool>(1 <= opt2).istrue();
        expect<bool>(opt2 > opt3).istrue();
        expect<bool>(opt3 > 1).istrue();
        expect<bool>(8 > opt3).istrue();
        expect<bool>(opt2 >= opt3).istrue();
        expect<bool>(opt3 >= 1).istrue();
        expect<bool>(8 >= opt3).istrue();
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

