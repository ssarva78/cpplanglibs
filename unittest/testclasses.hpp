#ifndef __LANGLIBS_TEST_CLASSES__
#define __LANGLIBS_TEST_CLASSES__
class myclass {
  public:
    myclass(const std::string& s, int n) : _m_str(s), _m_n(n) { }
    myclass() : _m_str("default string value"), _m_n(0) {}
    const std::string& strval() const { return _m_str; }
    void strval(const std::string& s) { _m_str = s; }
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

class noncopyableclass {
  public:
    explicit noncopyableclass(const std::string& s) : _str(s) {}
    const std::string& strval() const { return _str; }
  private:
    std::string _str;
    noncopyableclass(const noncopyableclass&) = delete;
};

#endif //__LANGLIBS_TEST_CLASSES__

