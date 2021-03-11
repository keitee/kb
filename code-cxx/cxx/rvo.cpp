#include <iostream>
#include <vector>

namespace cxx_rvo
{
  using namespace std;

  // Note: All methods have side effects
  struct Snitch
  {
    Snitch(int value)
        : value_(value)
    {
      cout << "c'tor" << endl;
    }
    ~Snitch() { cout << "d'tor" << endl; }

    // what if prevents copies?
    // Snitch(const Snitch &) = delete;
    // Snitch &operator=(Snitch &) = delete;

    Snitch(const Snitch &) { cout << "copy c'tor" << endl; }

    Snitch &operator=(const Snitch &)
    {
      cout << "copy assignment" << endl;
      return *this;
    }

    /*
    Snitch(Snitch &&) { cout << "move c'tor" << endl; }

    Snitch &operator=(Snitch &&)
    {
      cout << "move assignment" << endl;
      return *this;
    }
    */

    int getValue() const { return value_; }

  private:
    int value_{0};
  };

  Snitch ExampleRVO()
  {
    std::cout << "in ExampleRVO: " << std::endl;
    return Snitch(100);
  }
} // namespace cxx_rvo

int main()
{
  using namespace cxx_rvo;

  std::cout << "main before" << std::endl;
  Snitch snitch = ExampleRVO();
  std::cout << "main after " << std::endl;
}

// int main()
// {
//   using namespace cxx_rvo;

//   std::cout << "main before" << std::endl;
//   Snitch snitch(10);
//   snitch = ExampleRVO();
//   std::cout << "main after " << std::endl;
// }
