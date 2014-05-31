#include <unistd.h>
#include <string>

#include "glog/logging.h"
#include "gtest/gtest.h"

#include "closure.h"

int echo_int(int i) {
  LOG(INFO) << "func: " << i;
  return i;
}

class Base {
 public:
  Base(const char *name) : name_(name) {}
  virtual ~Base() {}
  std::string name() const {
    return name_;
  }
  virtual int echo_int(int i) const {
    LOG(INFO) << "base: " << i;
    return i;
  }
  virtual std::string echo_string(const std::string &msg) {
    LOG(INFO) << "base: " << msg;
    return msg;
  }
 private:
  std::string name_;
};

class Derived : public Base {
 public:
  Derived(const char *name) : Base(name) {}
  virtual int echo_int(int i) const {
    LOG(INFO) << "derived: " << i;
    return -i;
  }
};

TEST(ClosureTest, FunctionClosure) {
  Closure<int(int)> *closure = NewClosure(echo_int);
  ASSERT_TRUE(closure != NULL);
  ASSERT_EQ(100, closure->Run(100));

  closure = NewPermanentClosure(echo_int);
  ASSERT_TRUE(closure != NULL);
  ASSERT_EQ(200, closure->Run(200));
  ASSERT_EQ(300, closure->Run(300));
  ASSERT_EQ(400, closure->Run(400));
  delete closure;
}

TEST(ClosureTest, MethodClosure) {
  const char *kName = "aaaaaa";
  Base base(kName);
  ASSERT_EQ(kName, base.name());

  // non-virtual
  {
    Closure<std::string()> *closure = NewClosure(&base, &Base::name);
    ASSERT_EQ(kName, closure->Run());

    closure = NewPermanentClosure(&base, &Base::name);
    ASSERT_EQ(kName, closure->Run());
    ASSERT_EQ(kName, closure->Run());
    ASSERT_EQ(kName, closure->Run());
    delete closure;
  }

  // virtual
  {
    Closure<int(int)> *closure = NewClosure(&base, &Base::echo_int);
    ASSERT_EQ(100, closure->Run(100));

    closure = NewPermanentClosure(&base, &Base::echo_int);
    ASSERT_EQ(101, closure->Run(101));
    ASSERT_EQ(102, closure->Run(102));
    ASSERT_EQ(103, closure->Run(103));
    delete closure;
  }

  // virtual const
  {
    Closure<std::string(const std::string&)> *closure =
        NewClosure(&base, &Base::echo_string);
    ASSERT_EQ("hello world", closure->Run("hello world"));

    closure = NewPermanentClosure(&base, &Base::echo_string);
    ASSERT_EQ("processing", closure->Run("processing"));
    ASSERT_EQ("goodbye world", closure->Run("goodbye world"));
    delete closure;
  }
}

TEST(ClosureTest, DerivedMethodClosure) {
  const char *kName = "bbbbbb";
  Derived derived(kName);
  ASSERT_EQ(kName, derived.name());

  // non-virtual
  {
    Closure<std::string()> *closure = NewClosure(&derived, &Base::name);
    ASSERT_EQ(kName, closure->Run());

    closure = NewPermanentClosure(&derived, &Base::name);
    ASSERT_EQ(kName, closure->Run());
    ASSERT_EQ(kName, closure->Run());
    ASSERT_EQ(kName, closure->Run());
    delete closure;
  }

  // virtual
  {
    Closure<int(int)> *closure = NewClosure(&derived, &Derived::echo_int);
    ASSERT_EQ(-100, closure->Run(100));

    closure = NewPermanentClosure(&derived, &Derived::echo_int);
    ASSERT_EQ(-101, closure->Run(101));
    ASSERT_EQ(-102, closure->Run(102));
    ASSERT_EQ(-103, closure->Run(103));
    delete closure;
  }

  // virtual const
  {
    Closure<std::string(const std::string&)> *closure =
        NewClosure(&derived, &Derived::echo_string);
    ASSERT_EQ("hello world", closure->Run("hello world"));

    closure = NewPermanentClosure(&derived, &Derived::echo_string);
    ASSERT_EQ("processing", closure->Run("processing"));
    ASSERT_EQ("goodbye world", closure->Run("goodbye world"));
    delete closure;
  }
}

TEST(ClosureTest, Bind) {
  Base base("aaaa");
  Closure<int()> *c = NULL;

  // method
  c = NewClosure(&base, &Base::echo_int, 30);
  ASSERT_EQ(30, c->Run());
  c = NewClosure(&base, &Base::echo_int, 31);
  ASSERT_EQ(31, c->Run());
  c = NewClosure(&base, &Base::echo_int, 32);
  ASSERT_EQ(32, c->Run());
  c = NewPermanentClosure(&base, &Base::echo_int, 36);
  ASSERT_EQ(36, c->Run());
  ASSERT_EQ(36, c->Run());
  delete c;

  // function
  c = NewClosure(echo_int, 40);
  ASSERT_EQ(40, c->Run());
  c = NewClosure(echo_int, 41);
  ASSERT_EQ(41, c->Run());
  c = NewClosure(echo_int, 42);
  ASSERT_EQ(42, c->Run());
  c = NewPermanentClosure(echo_int, 46);
  ASSERT_EQ(46, c->Run());
  ASSERT_EQ(46, c->Run());
  delete c;
}
