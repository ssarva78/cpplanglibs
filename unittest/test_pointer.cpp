#include <unittest>
#include <pointer>
#include <string>
#include <thread>
#include <chrono>

using namespace cppunittest;
using namespace lang;

// resumeservice@naukiri.com

class myclass {
  public:
    myclass(const std::string& s, int n) : _m_str(s), _m_n(n) { }
    myclass() : _m_str("default string value"), _m_n(0) {}
    const std::string& strval() const { return _m_str; }
    int intval() const { return _m_n; }
    virtual std::string getname() { return "myclass"; }
    virtual ~myclass() {}
  private:
    std::string _m_str;
    int _m_n;
};

class myderivedclass : public myclass {
  public:
    myderivedclass(const std::string& s, int n): myclass(s, n) {}
    myderivedclass() : myclass() {}
    std::string getname() override { return "myderivedclass"; }
};

class myabstract {
  public:
    virtual std::string getname() = 0;
    virtual ~myabstract() {}
};

class myimplementation : public myabstract {
  public:
    virtual std::string getname() override { return "myimplementation"; }
};

int run_unittest() {
  unittest ut("Test pointer", __FILE__);

  ut
    .test(
      "Instatiate pointer with class constructor arguments",
      __testfunc__ {
        pointer<myclass> p("hello", 5);
        expect<std::string>((*p).strval()).is("hello");
        expect<int>((*p).intval()).is(5);
      }
    )

    .test(
      "Instatiate pointer with default class constructor",
      __testfunc__ {
        pointer<myclass> p;
        // cppcheck-suppress uninitvar
        expect<std::string>((*p).strval()).is("default string value");
      }
    )

    .test(
      "Validate reference count after copy constructor",
      __testfunc__ {
        pointer<myclass> p1("hello", 10);
        expect<long long>(p1.references()).is(1);
        pointer<myclass> p2(p1);
        expect<std::string>((*p2).strval()).is("hello");
        expect<long long>(p1.references()).is(2);
        expect<long long>(p2.references()).is(2);
      }
    )

    .test(
      "Validate reference count after destructor",
      __testfunc__ {
        pointer<myclass> p1("hello", 10);
        pointer<myclass> p2(p1);
        expect<long long>(p1.references()).is(2);
        p2.~pointer<myclass>();
        expect<long long>(p1.references()).is(1);
      }
    )

    .test(
      "Validate the underlying object is destructed when there is no more reference",
      __testfunc__ {
        pointer<myclass> p1("hello", 1);
        p1.~pointer<myclass>();
        expect<bool>(p1.isnull()).istrue();
      }
    )

    .test(
      "Validate rvalue pointer after move constructor",
      __testfunc__ {
        pointer<myclass> p1("hello", 5);
        pointer<myclass> p2(std::move(p1));
        expect<bool>(p1.isnull()).istrue();
        expect<int>((*p2).intval()).is(5);
      }
    )

    .test(
      "Validate the deferenced object is mutable",
      __testfunc__ {
        pointer<int> p(5);
        *p = 6;
        int i = *p;
        expect<int>(i).is(6);
      }
    )

    .test(
      "Validate deferencing null pointer throws exception",
      __testfunc__ {
        pointer<int> p(pointer<int>::null());
        expect<function_block>(__testfunc__{*p;}).throws(typeid(nullpointer_error)); //uses T& operator*()
        expect<function_block>(__testfunc__{*pointer<int>::null();}).throws(typeid(nullpointer_error)); // uses const T& operator*() const
        expect<function_block>(__testfunc__{p.references();}).throws(typeid(nullpointer_error));
      }
    )

    .test(
      "Assign a pointer to another and validate reference count",
      __testfunc__ {
        pointer<myclass> p1("hello", 25);
        pointer<myclass> p2;
        pointer<myclass> p3(p2);
        expect<long long>(p2.references()).is(2);
        p2 = p1;
        expect<long long>(p1.references()).is(2);
        expect<long long>(p3.references()).is(1);
        expect<std::string>((*p2).strval()).is("hello");
      }
    )

    .test(
      "Move a pointer and validate reference count",
      __testfunc__ {
        pointer<myclass> p1("hello", 25);
        pointer<myclass> p2;
        p2 = std::move(p1);
        expect<bool>(p1).isfalse();
        expect<std::string>((*p2).strval()).is("hello");
        expect<long long>(p2.references()).is(1);
      }
    )

    .test(
      "Check if pointer is null using bool operator",
      __testfunc__ {
        pointer<int> p(2);
        expect<bool>(p).istrue();
        expect<bool>(pointer<int>::null()).isfalse();
      }
    )

    .test(
      "Validate equal and unequal comparison operators",
      __testfunc__ {
        pointer<int> p1(2);
        pointer<int> p2(p1);
        pointer<int> p3(2);
        expect<bool>(p1 == p2).istrue();
        expect<bool>(p1 != p3).istrue();
      }
    )

    .test(
      "Validate dynamic binding",
      __testfunc__ {
        pointer<myclass> pb1 = pointer<myclass>::dynamic<myderivedclass>("hello derived", 25);
        expect<std::string>((*pb1).getname()).is("myderivedclass");
        pointer<myabstract> pb2 = pointer<myabstract>::dynamic<myimplementation>();
        expect<std::string>((*pb2).getname()).is("myimplementation");

        myimplementation &mi = (myimplementation&)*pb2;
        expect<std::string>(mi.getname()).is("myimplementation");

        myclass &mb = dynamic_cast<myclass&>(*pb1);
        expect<std::string>(mb.myclass::getname()).is("myclass");
      }
    )

    .test(
      "Thread safe assignment",
      __testfunc__ {
        concurrent_pointer<int> p(5);
        int r1 = 0, r2 = 0;
        auto addref =
          [&]() -> long long {
            concurrent_pointer<int> ptrs[1000000];
            for (int i = 0; i < 1000000; i ++) {
              ptrs[i] = p;
            }
            long long r = p.references();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            return r;
          };
        auto run1 = [&]() -> void { r1 = addref(); };
        auto run2 = [&]() -> void { r2 = addref(); };
        std::thread t1(run1), t2(run2);
        t1.join();
        t2.join();
        std::cout << "r1=" << r1 << ", r2=" << r2 << std::endl;
        expect<bool>(r1 == 2000001 or r2 == 2000001).istrue();
      }
    )

    .test(
      "Thread safe dereference",
      __testfunc__ {
        concurrent_pointer<int> p(5);
        concurrent_pointer<int> ptrs[1000000];
        for (int i=0; i < 1000000; i++) {
          ptrs[i] = p;
        }
        std::cout << "before p.references()=" << p.references() << std::endl;
        auto rmref1 =
          [&](int start) -> void {
            for (int i = start; i < start + 500000; i ++) {
              ptrs[i] = concurrent_pointer<int>(i);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
          };
        auto rmref2 =
          [&](int start) -> void {
            for (int i = start; i < start + 500000; i ++) {
              ptrs[i] = std::move(concurrent_pointer<int>(i));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
          };
        auto run1 = [&]() -> void { rmref1(0); };
        auto run2 = [&]() -> void { rmref2(500000); };
        std::thread t1(run1), t2(run2);
        t1.join();
        t2.join();
        std::cout << "after p.references()=" << p.references() << std::endl;
        expect<long long>(p.references()).is(1);
      }
    )

    .test(
      "Concurrent assignment and deassignement",
      __testfunc__ {
        concurrent_pointer<int> p(5);
        concurrent_pointer<int> ptrs_add[500000];
        auto addref =
          [&]() -> long long {
            for (int i = 0; i < 500000; i ++) {
              ptrs_add[i] = p;
            }
            long long r = p.references();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            return r;
          };
        concurrent_pointer<int> ptrs[500000];
        for (int i=0; i < 500000; i++) {
          ptrs[i] = p;
        }
        expect<int>(*ptrs[12345]).is(5);
        auto rmref =
          [&]() -> void {
            for (int i = 0; i < 500000; i ++) {
              ptrs[i] = concurrent_pointer<int>(i);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
          };
        auto run1 = [&]() -> void { addref(); };
        auto run2 = [&]() -> void { rmref(); };
        std::thread t1(run1), t2(run2);
        t1.join();
        t2.join();
        std::cout << "p.references()=" << p.references() << std::endl;
        expect<long long>(p.references()).is(500001);
        expect<int>(*ptrs_add[1990]).is(5);
        expect<int>(*ptrs[12345]).is(12345);
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

