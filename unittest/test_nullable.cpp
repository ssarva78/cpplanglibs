#include <unittest>
#include <nullable>

using namespace cppunittest;
using namespace lang;

class mynullabletestclass {
  public:
    explicit mynullabletestclass(const std::string& s) : _str(s) {}
    const std::string& strval() const { return _str; }
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
      "Validate bool operator overload for isnull",
      __testfunc__ {
        nullable<mynullabletestclass> opt_null;
        expect<bool>(opt_null).isfalse();
        nullable<mynullabletestclass> opt2(pointer<mynullabletestclass>("hello"));
        expect<bool>(opt2).istrue();
      }
    )

    .test(
      "Get the pointer from nullable",
      __testfunc__ {
        nullable<int> opt1(5);
        // cppcheck-suppress redundantPointerOp
        expect<int>(*&opt1).is(5);

        pointer<int> p = &opt1;
        *p = 20;
        expect<int>(*opt1).is(20);

        const nullable<int> opt2(5);
        // cppcheck-suppress redundantPointerOp
        expect<int>(*&opt2).is(5);

        nullable<int> opt_null;
        expect<bool>(&opt_null).isfalse();
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
      "Validate assignment and move to nullable",
      __testfunc__ {
        nullable<int> opt1(5);
        nullable<int> opt2;
        opt2 = opt1;
        expect<int>(*opt2).is(5);
        *opt1 = 10;
        expect<int>(*opt2).is(10);

        nullable<int> opt3;
        opt3 = std::move(opt1);
        expect<int>(*opt3).is(10);
        expect<int>(*opt2).is(10);
        expect<bool>(opt1).isfalse();
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

