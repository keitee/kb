/*

*/

#include <iostream>

using namespace std;

struct Foo {
  Foo(int value = 10): value_(value) { }
  ~Foo() {}

  Foo &operator+=(const Foo &f)
  {
    value_ += f.value_;
    return *this;
  }

  void PrintValue()
  {
    cout << "Foo::PrintValue: " << value_ << endl;
  } 

  Foo op_plus1(const Foo &a, const Foo &b)
  {
    Foo r(a);
    r += b;
    return r;
  }

  Foo op_plus2(const Foo &a, const Foo &b)
  {
    return Foo(a) += b;
  }

  private:
  int value_;
};

int main()
{
  Foo f1(10), f2(20);

  Foo f3 = f1.op_plus1(f1, f2);
  f3.PrintValue();

  Foo f4 = f1.op_plus2(f1, f2);
  f4.PrintValue();

  return 0;
}
