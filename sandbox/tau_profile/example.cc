#include <iostream>


void hw(const std::string source) {
  std::cout << "\"Hello World!\" from " << source << std::endl;
}

class A {

public:
  virtual void foo(void) {
    hw("A::foo");
  }
};

class B : public A {
public:
  virtual void foo(void) override {
    hw("B::foo");
  }
};


int main (void) {
  hw("main");
  A a;
  a.foo();
  A *ap = new B();
  ap->foo();
  B *b = (B *)ap;
  b->foo();
  b = new B();
  b->foo();
  delete ap;
  return 0;
}
