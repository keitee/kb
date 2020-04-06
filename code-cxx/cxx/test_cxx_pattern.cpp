#include <boost/lexical_cast.hpp>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <random>
#include <regex>
#include <set>
#include <thread>
#include <vector>

#include "gmock/gmock.h"

#include <sys/prctl.h>

using namespace std;
using namespace std::placeholders;
using namespace testing;

/*
={=============================================================================
cxx_pattern_decorator

The Morden C++ Challenge, Design Pattern

67. Validating passwords

Write a program that validates password strength based on predefined rules,
which may then be selected in various combinations. At a minimum, every password
must meet a minimum length requirement. In addition, other rules could be
enforced, such as the presence of at least one symbol, digit, uppercase and
lowercase letter, and so on.

The problem described here is a typical case for the decorator pattern. This
design pattern allows adding behavior to an object without affecting other
objects of the same type. This is achieved by wrapping an object within another
object. Multiple decorators could be stacked on top of each other, each time
adding new functionality. In our case, the functionality would be validating
that a given password meets a particular requirement.

The following class diagram describes the pattern for validating passwords:

        password_validator (abc)
        +                   +
length_validator         password_validator_decorator

                         digit_password_validator, case_password_validator,
symbol_password_validator
*/

namespace cxx_pattern_decorator
{
  class password_validator
  {
  public:
    virtual ~password_validator() {}
    // virtual bool validate(std::string& password) = 0;
    // virtual bool validate(std::string password) = 0;
    virtual bool validate(std::string const &password) = 0;
  };

  // mandatory(default) implementation

  class length_validator : public password_validator
  {
  public:
    length_validator(unsigned int min_length) noexcept
        : length_(min_length)
    {}

    // bool validate(std::string& password) override
    // bool validate(std::string password) override
    bool validate(std::string const &password) override
    {
      cout << "length_validator {" << endl;

      auto result = password.length() >= length_;

      cout << "length_validator }" << endl;

      return result;
    }

  private:
    unsigned int length_;
  };

  class password_validator_decorator : public password_validator
  {
  public:
    explicit password_validator_decorator(
      std::unique_ptr<password_validator> validator)
        : validator_(std::move(validator))
    {}

    // bool validate(std::string& password) override
    // bool validate(std::string password) override
    bool validate(std::string const &password) override
    {
      cout << "password_validator_decorator {" << endl;
      auto result = validator_->validate(password);
      cout << "password_validator_decorator }" << endl;

      return result;
    }

  private:
    std::unique_ptr<password_validator> validator_;
  };

  // must have one digits

  class digit_password_validator final : public password_validator_decorator
  {
  public:
    explicit digit_password_validator(
      std::unique_ptr<password_validator> validator)
        : password_validator_decorator(std::move(validator))
    {}

    // bool validate(std::string& password) override
    // bool validate(std::string password) override
    bool validate(std::string const &password) override
    {
      cout << "digit_password_validator {" << endl;

      if (password_validator_decorator::validate(password) == false)
        return false;

      cout << "digit_password_validator }" << endl;

      return password.find_first_of("0123456789") != std::string::npos;
    }
  };

  // must have one lower and one upper case

  class case_password_validator final : public password_validator_decorator
  {
  public:
    explicit case_password_validator(
      std::unique_ptr<password_validator> validator)
        : password_validator_decorator(std::move(validator))
    {}

    // bool validate(std::string& password) override
    // bool validate(std::string password) override
    bool validate(std::string const &password) override
    {
      cout << "case_password_validator {" << endl;

      if (password_validator_decorator::validate(password) == false)
        return false;

      bool has_lower{false};
      bool has_upper{false};

      for (size_t i = 0; i < password.size() && !(has_lower && has_upper); ++i)
      {
        if (std::islower(password[i]))
          has_lower = true;
        else if (std::isupper(password[i]))
          has_upper = true;
      }

      cout << "case_password_validator }" << endl;

      return has_lower && has_upper;
    }
  };

  // must have one of symbols

  class symbol_password_validator final : public password_validator_decorator
  {
  public:
    explicit symbol_password_validator(
      std::unique_ptr<password_validator> validator)
        : password_validator_decorator(std::move(validator))
    {}

    // error
    // bool validate(std::string& password) override
    // okay
    // bool validate(std::string password) override
    bool validate(std::string const &password) override
    {
      cout << "symbol_password_validator {" << endl;

      if (password_validator_decorator::validate(password) == false)
        return false;

      cout << "symbol_password_validator }" << endl;

      return password.find_first_of("!@#$%^&*(){}[]?<>") != std::string::npos;
    }
  };

} // namespace cxx_pattern_decorator

// shows how chain of objects works
//
// [ RUN      ] PatternDecorator.ValidatingPasswords
// digit_password_validator {
// password_validator_decorator {
// length_validator {
// length_validator }
// password_validator_decorator }
// digit_password_validator }
// [       OK ] PatternDecorator.ValidatingPasswords (1 ms)

TEST(PatternDecorator, ValidatingPasswords_1)
{
  using namespace cxx_pattern_decorator;

  // okay
  // std::unique_ptr<length_validator> validator1(new length_validator(8));
  // std::unique_ptr<digit_password_validator> validator2(
  //  new digit_password_validator(std::move(validator1)));

  // use temporary
  std::unique_ptr<digit_password_validator> validator2(
    new digit_password_validator(
      std::unique_ptr<length_validator>(new length_validator(8))));

  // *cxx-14*
  // auto validator1 = std::make_unique<digit_password_validator>(
  //     std::make_unique<length_validator>(8));

  EXPECT_THAT(validator2->validate("abc123!@#"), true);

  // since do not have a digit
  EXPECT_THAT(validator2->validate("abcde!@#"), false);
}

// now added `case` in the middle of decorator chain and this fails on `case
// validator` so ends there and see no ends for `digit validator`
//
// digit_password_validator {
// password_validator_decorator {
// case_password_validator {
// password_validator_decorator {
// length_validator {
// length_validator }
// password_validator_decorator }
// case_password_validator }
// password_validator_decorator }
// cxx_pattern.cpp:242: Failure
// Value of: validator2->validate("abc123!@#")
// Expected: is equal to true
//   Actual: false (of type bool)
// [  FAILED  ] PatternDecorator.ValidatingPasswords (3 ms)

TEST(PatternDecorator, ValidatingPasswords_2)
{
  using namespace cxx_pattern_decorator;

  // use temporary
  std::unique_ptr<digit_password_validator> validator2(
    new digit_password_validator(
      std::unique_ptr<case_password_validator>(new case_password_validator(
        std::unique_ptr<length_validator>(new length_validator(8))))));

  // now expects false
  EXPECT_THAT(validator2->validate("abc123!@#"), false);
}

// TEST(PatternDecorator, ValidatingPasswords_3)
// {
//    auto validator2 =
//       std::make_unique<symbol_password_validator>(
//          std::make_unique<case_password_validator>(
//             std::make_unique<digit_password_validator>(
//                std::make_unique<length_validator>(8))));
//
//    assert(validator2->validate("Abc123!@#"));
//    assert(!validator2->validate("Abc123567"));
// }

/*
={=============================================================================
cxx_pattern_composite

The Morden C++ Challenge, Design Pattern

68. Generating random passwords

Write a program that can generate random passwords according to some predefined
rules. Every password must have a configurable minimum length. In addition, it
should be possible to include in the generation rules such as the presence of at
least one digit, symbol, lower or uppercase character, and so on. These
additional rules must be configurable and composable.

This problem could be solved using the composite pattern or a variation of the
pattern. This design pattern composes objects into tree hierarchies and enables
treating groups (or trees) of objects the same way as individual objects of the
same type. The following class diagram shows a hierarchy of classes that can be
used for generating passwords:

*/

namespace cxx_pattern_composite
{
  class password_generator
  {
  public:
    virtual std::string generate()                                  = 0;
    virtual std::string allowed_chars() const                       = 0;
    virtual size_t length() const                                   = 0;
    virtual void add(std::unique_ptr<password_generator> generator) = 0;
    virtual ~password_generator() {}
  };

  class basic_password_generator : public password_generator
  {
  public:
    explicit basic_password_generator(size_t const length) noexcept
        : len_(length)
    {}

    virtual std::string generate() override
    {
      throw std::runtime_error("not implemented");
    }

    virtual void add(std::unique_ptr<password_generator>) override
    {
      throw std::runtime_error("not implemented");
    }

    virtual size_t length() const override final { return len_; }

  private:
    size_t len_;
  };

  class digit_generator : public basic_password_generator
  {
  public:
    explicit digit_generator(size_t const length) noexcept
        : basic_password_generator(length)
    {}

    virtual std::string allowed_chars() const override { return "0123456789"; }
  };

  class symbol_generator : public basic_password_generator
  {
  public:
    explicit symbol_generator(size_t const length) noexcept
        : basic_password_generator(length)
    {}

    virtual std::string allowed_chars() const override
    {
      return "!@#$%^&*(){}[]?<>";
    }
  };

  class upper_letter_generator : public basic_password_generator
  {
  public:
    explicit upper_letter_generator(size_t const length) noexcept
        : basic_password_generator(length)
    {}

    virtual std::string allowed_chars() const override
    {
      return "ABCDEFGHIJKLMNOPQRSTUVXYWZ";
    }
  };

  class composite_password_generator : public password_generator
  {
  public:
    composite_password_generator() {}

    virtual std::string generate() override
    {
      std::string password{};

      for (auto &g : generators_)
      {
        std::string chars = g->allowed_chars();
        std::uniform_int_distribution<> ud(
          0,
          static_cast<int>(chars.length() - 1));

        for (size_t i = 0; i < g->length(); ++i)
          password += chars[ud(eng)];
      }

      std::shuffle(std::begin(password), std::end(password), eng);

      return password;
    }

    virtual void add(std::unique_ptr<password_generator> generator) override
    {
      generators_.push_back(std::move(generator));
    }

  private:
    virtual std::string allowed_chars() const override
    {
      throw std::runtime_error("not implemented");
    }

    virtual size_t length() const override
    {
      throw std::runtime_error("not implemented");
    }

    std::default_random_engine eng;
    std::vector<std::unique_ptr<password_generator>> generators_;
  };

} // namespace cxx_pattern_composite

TEST(PatternComposite, PasswordGenerator_1)
{
  using namespace cxx_pattern_composite;

  composite_password_generator generator;

  generator.add(std::unique_ptr<symbol_generator>(new symbol_generator(2)));
  generator.add(std::unique_ptr<digit_generator>(new digit_generator(2)));

  auto password = generator.generate();
  cout << "password: " << password << endl;
}

TEST(PatternComposite, PasswordGenerator_2)
{
  using namespace cxx_pattern_composite;

  composite_password_generator generator;

  generator.add(std::unique_ptr<symbol_generator>(new symbol_generator(2)));
  generator.add(std::unique_ptr<digit_generator>(new digit_generator(2)));
  generator.add(
    std::unique_ptr<upper_letter_generator>(new upper_letter_generator(2)));

  auto password = generator.generate();
  cout << "password: " << password << endl;
}

/*
={=============================================================================
cxx_pattern_template

The-Modern-Cpp-Challenge/Chapter08/problem_69

The Morden C++ Challenge, Design Pattern

69. Generating social security numbers

Write a program that can generate social security numbers for two countries,
Northeria and Southeria, that have different but similar formats for the
numbers:

In Northeria, the numbers have the format SYYYYMMDDNNNNNC, where
  S is a digit representing the sex, 9 for females and 7 for males,
  YYYYMMDD is the birth date, (same)
  NNNNN is a five-digit random number, unique for a day (meaning that the same
  number can appear twice for two different dates, but not the same date),
  and C is a digit picked so that the checksum computed as described later is a
  multiple of 11.

In Southeria, the numbers have the format SYYYYMMDDNNNNC, where
  S is a digit representing the sex, 1 for females and 2 for males,
  YYYYMMDD is the birth date, (same)
  NNNN is a four-digit random number, unique for a day,
  and C is a digit picked so that the checksum computed as described below is a
  multiple of 10.


The checksum in both cases is a sum of all the digits, each multiplied by its
weight (the position from the most significant digit to the least).

For example, the checksum for the Southerian number 12017120134895 is computed
as follows:

crc = 14*1 + 13*2 + 12*0 + 11*1 + 10*7 + 9*1 + 8*2 + 7*0 + 6*1 + 5*3
           +  4*4 +  3*8 +  2*9 +  1*5
    = 230 = 23 * 10

The formats for both countries are very similar, although several details are
different:

The value of the digit for the sex
The length of the random part, and therefore the length of the entire number
The number the checksum must be a multiple of

This problem can be solved using the template method design pattern, which
defines the skeleton of an algorithm and lets subclasses redefine particular
steps:

*/

namespace cxx_pattern_template
{
  enum class sex_type
  {
    female,
    male
  };

  class social_number_generator
  {
  public:
    std::string
    generate(sex_type sex, unsigned year, unsigned month, unsigned day)
    {
      std::stringstream snumber;

      snumber << sex_digit(sex);
      snumber << year << month << day;
      snumber << next_random(year, month, day);

      // For example, the checksum for the Southerian number 12017120134895 is
      // computed as follows:
      //
      // crc = 14*1 + 13*2 + 12*0 + 11*1 + 10*7 + 9*1 + 8*2 + 7*0 + 6*1 + 5*3
      //     +  4*4 +  3*8 +  2*9 +  1*5

      auto number = snumber.str();
      auto index  = number.length();

      // cxx-algo-accumulate
      auto sum = std::accumulate(
        std::begin(number),
        std::end(number),
        0, // init value
        [&](int init, char c) { return init + (index-- * (c - '0')); });

      auto rest = sum % modulo_value();
      snumber << modulo_value() - rest;

      return snumber.str();
    }

    // dtor-in-cxx-abc
    virtual ~social_number_generator() {}

    // works but public also works
    // protected:

  public:
    social_number_generator(int min, int max)
        : ud(min, max)
    {}

  protected:
    virtual int sex_digit(sex_type const sex) const noexcept             = 0;
    virtual int next_random(unsigned year, unsigned month, unsigned day) = 0;
    virtual int modulo_value() const noexcept                            = 0;

  protected:
    std::map<unsigned, int> cache;
    std::uniform_int_distribution<> ud;
    std::default_random_engine eng;
  };

  class southeria_social_number_generator final : public social_number_generator
  {
  public:
    // 4 digit.
    southeria_social_number_generator()
        : social_number_generator(1000, 9999)
    {}

  protected:
    virtual int sex_digit(sex_type sex) const noexcept override
    {
      return sex == sex_type::female ? 1 : 2;
    }

    // cxx-algo-unique use cache to have unique number?
    virtual int
    next_random(unsigned year, unsigned month, unsigned day) override
    {
      auto key = year * 10000 + month * 100 + day;
      while (true)
      {
        auto number = ud(eng);

        // when not found *cxx-cache*
        auto pos = cache.find(number);
        if (pos == std::end(cache))
        {
          cache[key] = number;
          return number;
        }
      }
    }

    virtual int modulo_value() const noexcept override { return 10; }
  };

  class northeria_social_number_generator final : public social_number_generator
  {
  public:
    // 5 digit
    northeria_social_number_generator()
        : social_number_generator(10000, 99999)
    {}

  protected:
    virtual int sex_digit(sex_type sex) const noexcept override
    {
      return sex == sex_type::female ? 9 : 7;
    }

    virtual int
    next_random(unsigned year, unsigned month, unsigned day) override
    {
      auto key = year * 10000 + month * 100 + day;
      while (true)
      {
        auto number = ud(eng);

        auto pos = cache.find(number);
        if (pos == std::end(cache))
        {
          cache[key] = number;
          return number;
        }
      }
    }

    virtual int modulo_value() const noexcept override { return 11; }
  };

  class social_number_generator_factory
  {
  public:
    social_number_generator_factory()
    {
      generators_["north"] = std::unique_ptr<northeria_social_number_generator>(
        new northeria_social_number_generator());

      generators_["south"] = std::unique_ptr<southeria_social_number_generator>(
        new southeria_social_number_generator());
    }

    social_number_generator *get_generator(std::string country) const
    {
      auto it = generators_.find(country);
      if (it != std::end(generators_))
        return it->second.get();

      throw std::runtime_error("not found country");
    }

  private:
    std::map<std::string, std::unique_ptr<social_number_generator>> generators_;
  };

} // namespace cxx_pattern_template

TEST(PatternTemplate, GeneratingSecurityNumber)
{
  using namespace cxx_pattern_template;

  social_number_generator_factory factory;

  {
    auto snorth1 = factory.get_generator("north");
    std::string result;
    if (snorth1 != nullptr)
    {
      result = snorth1->generate(sex_type::female, 2017, 12, 25);

      cout << "snorth1: " << result << endl;
    }
  }

  {
    auto ssouth1 = factory.get_generator("south");
    std::string result;
    if (ssouth1 != nullptr)
    {
      result = ssouth1->generate(sex_type::female, 2017, 12, 25);

      cout << "ssouth1: " << result << endl;
    }
  }
}

/*
={=============================================================================
cxx_pattern_chain_of_responsibility

The Morden C++ Challenge, Design Pattern

70. Approval system

Write a program for a purchasing department of a company that allows employees
to approve new purchases (or expenses). However, based on their position, each
employee may only approve expenses up to a predefined limit. For instance,
regular employees can approve expenses up to 1,000 currency units, team managers
up to 10,000, and the department manager up to 100,000. Any expense greater than
that must be explicitly approved by the company president.

The problem described can be expressed in a series of if... else if... else...
endif statements. An object-oriented version of this idiom is the chain of
responsibility design pattern. This pattern defines a chain of receiver objects
that have the responsibility of either handling a request or passing it to the
next receiver in the chain if one exists. The following class diagram shows a
possible implementation of the pattern for this problem:

*/

namespace cxx_pattern_chain_of_responsibility
{
  class role
  {
  public:
    virtual double approval_limit() const noexcept = 0;
    virtual ~role() {}
  };

  class employee_role : public role
  {
  public:
    virtual double approval_limit() const noexcept override { return 1000; }
  };

  class team_manager_role : public role
  {
  public:
    virtual double approval_limit() const noexcept override { return 10000; }
  };

  class dept_manager_role : public role
  {
  public:
    virtual double approval_limit() const noexcept override { return 100000; }
  };

  class president_role : public role
  {
  public:
    virtual double approval_limit() const noexcept override
    {
      return std::numeric_limits<double>::max();
    }
  };

  struct expense
  {
    double amount_;
    std::string desc_;

    expense(double const amount, std::string desc)
        : amount_(amount)
        , desc_(desc)
    {}
  };

  class employee
  {
  public:
    explicit employee(std::string name, std::unique_ptr<role> role)
        : name_(name)
        , role_(std::move(role))
    {}

    void set_direct_manager(std::shared_ptr<employee> manager)
    {
      manager_ = manager;
    }

    void approve(expense const &e)
    {
      if (e.amount_ <= role_->approval_limit())
        std::cout << name_ << " approved expense '" << e.desc_
                  << "', cost= " << e.amount_ << std::endl;
      else if (manager_ != nullptr)
        manager_->approve(e);
    }

  private:
    std::string name_;
    std::unique_ptr<role> role_;
    std::shared_ptr<employee> manager_;
  };

} // namespace cxx_pattern_chain_of_responsibility

// [ RUN      ] PatternChainOfResponsibility.ApprovalSystem
// john smith approved expense 'magazins', cost= 500
// robert booth approved expense 'hotel accomodation', cost= 5000
// david jones approved expense 'conference costs', cost= 50000
// cecil williamson approved expense 'new lorry', cost= 200000
// [       OK ] PatternChainOfResponsibility.ApprovalSystem (1 ms)

TEST(PatternChainOfResponsibility, ApprovalSystem)
{
  using namespace cxx_pattern_chain_of_responsibility;

  // employee
  auto john = std::make_shared<employee>(
    "john smith",
    std::unique_ptr<employee_role>(new employee_role()));

  // team manager
  auto robert = std::make_shared<employee>(
    "robert booth",
    std::unique_ptr<team_manager_role>(new team_manager_role()));

  // dept manager
  auto david = std::make_shared<employee>(
    "david jones",
    std::unique_ptr<dept_manager_role>(new dept_manager_role()));

  // president
  auto cecil = std::make_shared<employee>(
    "cecil williamson",
    std::unique_ptr<president_role>(new president_role()));

  // build approval chain
  john->set_direct_manager(robert);
  robert->set_direct_manager(david);
  david->set_direct_manager(cecil);

  john->approve(expense{500, "magazins"});
  john->approve(expense{5000, "hotel accomodation"});
  john->approve(expense{50000, "conference costs"});
  john->approve(expense{200000, "new lorry"});
}

/*
={=============================================================================
cxx_pattern_observer

The Morden C++ Challenge, Design Pattern, Chapter 08

71. Observable vector container

Write a class template that behaves like a vector but can notify registered
parties of internal state changes. The class must provide at least the following
operations:

Various constructors for creating new instances of the class
operator= to assign values to the container
push_back() to add a new element at the end of the container
pop_back() to remove the last element from the container
clear() to remove all the elements from the container
size() to return the number of elements from the container
empty() to indicate whether the container is empty or has elements

operator=, push_back(), pop_back(), and clear() must notify others of the state
changes. The notification should include the type of the change, and, when the
case, the index of the element that was changed (such as added or removed).


The observable vector described in this problem is a typical example of a
subject in the design pattern called observer. This pattern describes an object,
called the subject, that maintains a list of dependent objects, called
observers, and notifies them of any state changes by calling one of their
methods. The class diagram shown here describes a possible pattern
implementation for the proposed problem:


{
  observable_vector<int> ov;
  observer o;
  ov.add_observer(&o);
}

If there is no need to call remove_observer() or removeListener(), then no need
to hold reference to source(notifier). For example, only add interest and use it
until application finishes. Or, use of expired weak_ptr.


<case>
this is the same.
- defines observer class from source class
- use macro for making notifications

// from onbserver
class MediaStreamer : public public A2DPTransportListener
{
};

{
  m_x = new A2DPTransport(path);
  m_x->addListener(listener);
};

{
  m_x->removeListener(xport_listener);
}

// from source class

class A2DPTransportListener;
class A2DPTransport
{
  bool addListener(A2DPTransportListener *listener);

#define notify(_what) do { \
    std::vector<BluetoothA2DPTransportListener*> listeners; \
    { \
        Locker lock(&m_listener_lock); \
        listeners = m_listeners; \
    } \
    for (std::vector<BluetoothA2DPTransportListener*>::iterator it =
      listeners.begin(); it != listeners.end(); ++it) if (*it) (*it)->_what; \
    } while(0)

// use notify() macro
  {
    notify(onBufferReady(this, buffer, nbytes, sampleno));
  }
};


<case>
Do not use lister class T which has callback interfaces since it has only one
callback and do not use inheritance. Instead, use std::function in listener
registeration and callback as:

typedef std::function<void (EventSource, uint16_t, bool)> KeyListener;

*/

#if 0
// this is code from the text which works but not sure why this is complicated
// as this.

namespace cxx_pattern_observer
{
  // final?
  template <typename T, typename Allocator = std::allocator<T>>
  class observable_vector final
  {
    typedef typename std::vector<T, Allocator>::size_type size_type;

    public:

    // ???
    observable_vector() noexcept(noexcept(Allocator()))
      : observable_vector(Allocator()) 
    {}

    explicit observable_vector(Allocator const& alloc) noexcept
      : data_(alloc) 
      {}

    observable_vector(size_type count, const T& value, 
        Allocator const& alloc = Allocator())
      : data_(count, value, alloc)
    {}

    explicit observable_vector(size_type count, 
        Allocator const& alloc = Allocator())
      : data_(count, alloc)
    {}

    // moves

    observable_vector(observable_vector&& other) noexcept
      : data_(other.data_)
      {}

    observable_vector(observable_vector&& other, Allocator const& alloc)
      : data_(other.data_, alloc)
    {}

    // init list
    observable_vector(std::initializer_list<T> init,
        Allocator const& alloc = Allocator())
      : data_(init, alloc)
    {}

    template<typename InputIterator>
      observable_vector(InputIterator first, InputIterator last,
          Allocator const& alloc = Allocator())
      : data_(first, last, alloc)
      {}

    observable_vector& operator=(observable_vector const& other)
    {
      if (this != &other)
      {
        data_ = other.data_;

        // 1. weak_ptr?
        // 2. std::vector<size_t>{} is empty when assign?

        for (auto o : observers_)
        {
          if (o != nullptr)
          {
            o->collection_changed(
                // {collection_action::assign, std::vector<size_t>{}}
                {collection_action::assign, 0}
                );
          }
        }
      }

      return *this;
    }

    observable_vector& operator=(observable_vector&& other)
    {
      if (this != &other)
      {
        data_ = std::move(other.data_);

        for (auto o : observers_)
        {
          if (o != nullptr)
          {
            o->collection_changed(
                // {collection_action::assign, std::vector<size_t>{}}
                {collection_action::assign, 0}
                );
          }
        }
      }

      return *this;
    }

    // only rvalue version?
    // the index of the element that was changed (such as added or removed)

    void push_back(T&& value)
    {
      data_.push_back(value);

      for (auto o : observers_)
      {
        if (o != nullptr)
        {
          o->collection_changed(
              // {collection_action::add, std::vector<size_t>{data_.size()-1}}
              {collection_action::add, data_.size()-1}
              );
        }
      } // end for
    }

    void pop_back()
    {
      data_.pop_back();

      for (auto o : observers_)
      {
        if (o != nullptr)
        {
          o->collection_changed(
              // {collection_action::remove, std::vector<size_t>{data_.size()+1}}
              // {collection_action::remove, std::vector<size_t>{data_.size()}}
              {collection_action::remove, data_.size()}
              );
        }
      } // end for
    }

    void clear() noexcept
    {
      data_.clear();

      for (auto o : observers_)
      {
        if (o != nullptr)
        {
          o->collection_changed(
              // {collection_action::clear, std::vector<size_t>{}}
              {collection_action::clear, 0}
              );
        }
      } // end for
    }

    size_type size() const noexcept
    { return data_.size(); }


    bool empty() const noexcept
    { return data_.empty(); }

    void add_observer(collection_observer* const o)
    { observers_.push_back(o); }

    void remove_observer(collection_observer const* const o)
    {
      observers_.erase(
          std::remove(std::begin(observers_), std::end(observers_), o),
          std::end(observers_)
          );
    }

    private:
    // what's the difference when use std::vector<T>?
    std::vector<T, Allocator> data_;
    std::vector<collection_observer*> observers_;
  };
} // namespace cxx_pattern_observer
#endif

#if 0
namespace cxx_pattern_observer
{
  enum class changed_action
  {
    add,
    remove,
    clear,
    assign
  };

  std::string action_name(changed_action const action)
  {
    switch (action)
    {
    case changed_action::add:
      return "added";
    case changed_action::remove:
      return "removed";
    case changed_action::clear:
      return "cleared";
    case changed_action::assign:
      return "assigned";
    default:
      return "unsupported";
    }
  }

  struct changed_notification
  {
    changed_action action;
    size_t index;
  };

  // abc
  class collection_observer
  {
  public:
    virtual ~collection_observer() {}
    virtual void onChanged(changed_notification const &noti) = 0;
  };

  // observers(listeners)
  class observer : public collection_observer
  {
  public:
    virtual void onChanged(changed_notification const &noti)
    {
      std::cout << "onChanged(action: " << action_name(noti.action)
                << ", index: " << noti.index << ")" << std::endl;
    }
  };

  // observee(source)
  template <typename T> class observable_vector
  {
    using size_type = typename std::vector<T>::size_type;

  public:
    explicit observable_vector()
        : data_()
        , observers_()
    {}

    explicit observable_vector(std::initializer_list<T> init)
        : data_(init)
    {}

    // copy assign
    // do not update(overwrite) `observers_` from rhs so keep current
    // observers

    observable_vector &operator=(observable_vector const &rhs)
    {
      // for self assign
      if (this != &rhs)
      {
        data_ = rhs.data_;

        for (auto const o : observers_)
        {
          if (nullptr != o)
          {
            o->onChanged({changed_action::assign, data_.size()});
          }
        }
      }

      return *this;
    }

    // move assign
    observable_vector &operator=(observable_vector &&rhs)
    {
      // for self assign
      if (this != &rhs)
      {
        // done by vector's move
        data_ = std::move(rhs.data_);

        for (auto const o : observers_)
        {
          if (nullptr != o)
          {
            o->onChanged({changed_action::assign, data_.size()});
          }
        }
      }

      return *this;
    }

    void push_back(T &value)
    {
      cout << "push_back(T&)" << endl;

      data_.push_back(value);

      // see that this code nearly same except `parameters` to onChange()
      for (auto const o : observers_)
      {
        if (nullptr != o)
        {
          o->onChanged({changed_action::add, data_.size()});
        }
      }
    }

    // same as push_back(T&) since std::vector.push_back() will handle
    void push_back(T &&value)
    {
      cout << "push_back(T&&)" << endl;

      data_.push_back(value);

      for (auto const o : observers_)
      {
        if (nullptr != o)
        {
          o->onChanged({changed_action::add, data_.size()});
        }
      }
    }

    void pop_back()
    {
      data_.pop_back();

      for (auto const o : observers_)
      {
        if (o != nullptr)
        {
          o->onChanged({changed_action::remove, data_.size()});
        }
      }
    }

    void clear() noexcept
    {
      data_.clear();

      for (auto const o : observers_)
      {
        if (o != nullptr)
        {
          o->onChanged({changed_action::clear, data_.size()});
        }
      }
    }

    size_type size() const noexcept { return data_.size(); }

    bool empty() const noexcept { return data_.empty(); }

    // observer interfaces
    void add_observer(collection_observer *o) { observers_.push_back(o); }

    void remove_observer(collection_observer const *o)
    {
      // this assumes that there are duplicated observers
      observers_.erase(
        std::remove(std::begin(observers_), std::end(observers_), o), // begin
        observers_.end()                                              // end
      );
    }

  private:
    std::vector<T> data_;
    std::vector<collection_observer *> observers_;
  };
} // namespace cxx_pattern_observer

TEST(PatternObserver, ObservableVectorContainer)
{
  using namespace cxx_pattern_observer;

  observable_vector<int> ov;
  observer o;
  int value{5};

  ov.add_observer(&o);

  // move
  cout << "=========" << endl;
  ov.push_back(1);

  // move
  cout << "=========" << endl;
  ov.push_back(2);

  // copy
  cout << "=========" << endl;
  ov.push_back(value);

  cout << "=========" << endl;
  ov.pop_back();

  cout << "=========" << endl;
  ov.clear();

  ov.remove_observer(&o);

  // no notification since observer is removed.

  observable_vector<int> ovv{1, 2, 3};

  // copy assign
  cout << "=========" << endl;
  ov = ovv;

  // move assign
  cout << "=========" << endl;
  ov = observable_vector<int>{7, 8, 9};
  ov.push_back(1);
}
#endif

namespace observer_ex_2019_12_25
{
  // use observable vector

  enum class ChangedAction
  {
    add,
    remove,
    clear,
    assign
  };

  struct ChangedNotification
  {
    ChangedAction action;
    size_t index;
  };

  std::string GET_ACTION_NAME(ChangedAction const action)
  {
    switch (action)
    {
      case ChangedAction::add:
        return "add notification";
      case ChangedAction::remove:
        return "remove notification";
      case ChangedAction::clear:
        return "clear notification";
      case ChangedAction::assign:
        return "assign notification";
      default:
        return "unknown notification";
    }
  };

  // observer uses `interface` and let's use ABC and inheritance in observer
  // tree

  class CollectionObserver
  {
  public:
    virtual ~CollectionObserver() {}
    virtual void onChanged(ChangedNotification const &changedNotification) = 0;
  };

  class Observer1 : public CollectionObserver
  {
  public:
    virtual void onChanged(ChangedNotification const &changedNotification)
    {
      std::cout << "Observer1::onChanged("
                << GET_ACTION_NAME(changedNotification.action) << " on index "
                << changedNotification.index << std::endl;
    }
  };

  class Observer2 : public CollectionObserver
  {
  public:
    virtual void onChanged(ChangedNotification const &changedNotification)
    {
      std::cout << "Observer2::onChanged("
                << GET_ACTION_NAME(changedNotification.action) << " on index "
                << changedNotification.index << std::endl;
    }
  };

  // observee, subject

  template <typename T>
  class ObservableVector
  {
    using size_type = typename std::vector<T>::size_type;

  private:
    std::vector<T> data_;
    std::vector<CollectionObserver *> observers_;

  public:
    explicit ObservableVector()
        : data_()
        , observers_()
    {}

    explicit ObservableVector(std::initializer_list<T> list)
        : data_(list)
        , observers_()
    {}

    // copy supports
    // copies data vector but not observers. notify current observers

    // no need to use this
    // ObservableVector<T> &operator=(const ObservableVector<T> &rhs)

    ObservableVector &operator=(const ObservableVector &rhs)
    {
      std::cout << "operator=(&)" << std::endl;

      // self assign
      if (this != &rhs)
      {
        data_ = rhs.data_;

        for (const auto &e : observers_)
        {
          // although it is less likely that e is nullptr
          // if (nullptr != e)
          // so can use

          if (__builtin_expect(nullptr != e, 1))
          {
            // NOTE: see cxx-temporary
            e->onChanged({ChangedAction::assign, data_.size()});
          }
        }
      }

      return *this;
    }

    // no copy ctor
    ObservableVector(const ObservableVector &rhs) = delete;

    // move supports
    ObservableVector &operator=(const ObservableVector &&rhs)
    {
      std::cout << "operator=(&&)" << std::endl;

      if (this != &rhs)
      {
        data_ = std::move(rhs.data_);

        for (const auto &e : observers_)
        {
          if (__builtin_expect(nullptr != e, 1))
          {
            e->onChanged({ChangedAction::assign, data_.size()});
          }
        }
      }

      return *this;
    }

    ObservableVector(const ObservableVector &&rhs) = delete;

    void push_back(const T &value)
    {
      data_.push_back(value);

      for (const auto &e : observers_)
      {
        if (__builtin_expect(nullptr != e, 1))
        {
          // e.onChanged({ChangedAction::add, 1});
          e->onChanged({ChangedAction::add, data_.size()});
        }
      }
    }

    void push_back(const T &&value)
    {
      // data_.push_back(std::move(value));
      // no need since it's already rvalue adn std::vector() will handle
      // move context
      //
      // this makes the move version same as the copy version.

      data_.push_back(value);

      for (const auto &e : observers_)
      {
        if (__builtin_expect(nullptr != e, 1))
        {
          e->onChanged({ChangedAction::add, data_.size()});
        }
      }
    }

    void pop_back()
    {
      data_.pop_back();

      for (const auto &e : observers_)
      {
        if (__builtin_expect(nullptr != e, 1))
        {
          e->onChanged({ChangedAction::remove, data_.size()});
        }
      }
    }

    void clear() noexcept
    {
      data_.clear();

      for (const auto &e : observers_)
      {
        if (__builtin_expect(nullptr != e, 1))
        {
          e->onChanged({ChangedAction::clear, data_.size()});
        }
      }
    }

    size_type size() const noexcept { return data_.size(); }

    bool empty() const noexcept { return data_.empty(); }

    // void add_observer(const CollectionObserver *observer)
    void add_observer(CollectionObserver *observer)
    {
      observers_.push_back(observer);
    }

    // assumes that there are duplicates in observers
    void remove_observer(const CollectionObserver *observer)
    {
      observers_.erase(
        std::remove(observers_.begin(), observers_.end(), observer),
        observers_.end());
    }
  }; // ObservableVector

} // namespace observer_ex_2019_12_25

// [ RUN      ] Observer.Ex_ObservableVectorContainer
// =========
// Observer1::onChanged(add notification on index 1
// Observer2::onChanged(add notification on index 1
// =========
// Observer1::onChanged(add notification on index 2
// Observer2::onChanged(add notification on index 2
// =========
// Observer1::onChanged(add notification on index 3
// Observer2::onChanged(add notification on index 3
// =========
// Observer1::onChanged(remove notification on index 2
// Observer2::onChanged(remove notification on index 2
// =========
// Observer1::onChanged(clear notification on index 0
// Observer2::onChanged(clear notification on index 0
// =========
// operator=(&)
// =========
// operator=(&&)
// [       OK ] Observer.Ex_ObservableVectorContainer (0 ms)

// TEST(Observer, Ex_ObservableVectorContainer)
TEST(PatternObserver, ObservableVectorContainer)
{
  using namespace observer_ex_2019_12_25;

  ObservableVector<int> ov1;

  Observer1 o1;
  Observer2 o2;

  int value{5};

  ov1.add_observer(&o1);
  ov1.add_observer(&o2);

  // move
  cout << "=========" << endl;
  ov1.push_back(1);

  // move
  cout << "=========" << endl;
  ov1.push_back(2);

  // copy
  cout << "=========" << endl;
  ov1.push_back(value);

  cout << "=========" << endl;
  ov1.pop_back();

  cout << "=========" << endl;
  ov1.clear();

  ov1.remove_observer(&o1);
  ov1.remove_observer(&o2);

  // no notification since observer is removed.

  ObservableVector<int> ov2{1, 2, 3};

  // copy assign
  cout << "=========" << endl;
  ov1 = ov2;

  // move assign
  cout << "=========" << endl;
  ov1 = ObservableVector<int>{7, 8, 9};
  ov1.push_back(1);
}

/*
={=============================================================================
cxx_pattern_strategy

The Morden C++ Challenge, Design Pattern, Chapter 08

72. Computing order price with discounts

// discount for product, each item, customer

A retail store sells a variety of goods and can offer various types of discount,
for selected customers, articles, or per order. The following types of discount
could be available:

// discount types

A fixed discount, such as 5%, regardless of the article or the quantity that is
purchased.

A volume discount, such as 10%, for each article when buying more than a
particular quantity of that article.

A price discount per total order of an article, that is, a discount for an
article when a customer buys a quantity of that article so that the total cost
exceeds a particular amount. For instance, a 15% discount for an article when
the total cost of that article exceeds $100. If the article costs $5, and the
customer buys 30 units, the total cost is $150; therefore, a 15% discount
applies to the order of that article.

A price discount per entire order (regardless what articles and in which
quantity they were ordered).

Write a program that can calculate the final price of a particular order. It is
possible to compute the final price in different ways; for instance, all
discounts could be cumulative, or on the other hand, if an article has a
discount, a customer or total order discount might not be considered.


The problem proposed here can be solved with the strategy pattern. This design
pattern defines a family of algorithms and makes them interchangeable within the
family. In this particular problem, both the discounts and the final order price
calculators could be implemented based on the strategy pattern. The following
diagram describes the hierarchy of discount types and their interchangeable use
within the other classes, customer, article, order_line, and order:

*/

namespace cxx_pattern_strategy
{
  struct discount_type
  {
    virtual ~discount_type() {}

    // get discount percentage for each type

    virtual double discount_percent(double price, double quantity) const
      noexcept = 0;
  };

  struct fixed_discount final : public discount_type
  {
    explicit fixed_discount(double discount) noexcept
        : discount_(discount)
    {}

    virtual double discount_percent(double, double) const noexcept override
    {
      return discount_;
    }

  private:
    double discount_;
  };

  struct volume_discount final : public discount_type
  {
    explicit volume_discount(double quantity, double discount) noexcept
        : discount_(discount)
        , min_quantity_(quantity)
    {}

    virtual double discount_percent(double, double quantity) const
      noexcept override
    {
      return quantity >= min_quantity_ ? discount_ : 0;
    }

  private:
    double discount_;
    double min_quantity_;
  };

  // discount on total price
  struct price_discount final : public discount_type
  {
    explicit price_discount(double price, double discount) noexcept
        : discount_(discount)
        , min_total_price_(price)
    {}

    virtual double discount_percent(double price, double quantity) const
      noexcept override
    {
      return (price * quantity) >= min_total_price_ ? discount_ : 0;
    }

  private:
    double discount_;
    double min_total_price_;
  };

  // discount on item price
  struct amount_discount final : public discount_type
  {
    explicit amount_discount(double price, double discount) noexcept
        : discount_(discount)
        , min_total_price_(price)
    {}

    virtual double discount_percent(double price, double) const
      noexcept override
    {
      return price >= min_total_price_ ? discount_ : 0;
    }

  private:
    double discount_;
    double min_total_price_;
  };

  // user(client) class

  struct customer
  {
    std::string name_;
    discount_type *discount_;
  };

  enum class article_unit
  {
    piece,
    kg,
    meter,
    sqmeter,
    cmeter,
    liter
  };

  struct article
  {
    int id_;
    std::string name_;
    double price_;
    article_unit unit_;
    discount_type *discount_;
  };

  struct order_line
  {
    article product_;
    int quantity_;
    discount_type *discount_;
  };

  struct order
  {
    int id_;
    customer *buyer_;
    std::vector<order_line> lines_;
    discount_type *discount_;
  };

  struct price_calculator
  {
    virtual double calculate_price(order const &o) = 0;
  };

  struct cumulative_price_calculator : public price_calculator
  {
    virtual double calculate_price(order const &o) override
    {
      double price = 0;

      // each item ordered
      for (auto ol : o.lines_)
      {
        double line_price = ol.product_.price_ * ol.quantity_;

        // product discount
        if (ol.product_.discount_ != nullptr)
        {
          line_price *=
            (1.0 - ol.product_.discount_->discount_percent(ol.product_.price_,
                                                           ol.quantity_));
        }

        // order_line discount
        if (ol.discount_ != nullptr)
        {
          line_price *=
            (1.0 -
             ol.discount_->discount_percent(ol.product_.price_, ol.quantity_));
        }

        // buyer discount
        if (o.buyer_ != nullptr && o.buyer_->discount_ != nullptr)
        {
          line_price *=
            (1.0 - o.buyer_->discount_->discount_percent(ol.product_.price_,
                                                         ol.quantity_));
        }

        price += line_price;
      } // end for

      // whole order discount
      if (o.discount_ != nullptr)
        price *= (1.0 - o.discount_->discount_percent(price, 0));

      return price;
    };
  };

} // namespace cxx_pattern_strategy

TEST(PatternStrategy, ComputingOrderPriceWithDiscounts)
{
  using namespace cxx_pattern_strategy;

  fixed_discount fd(0.1); // 10%
  volume_discount vd(10, 0.15);
  price_discount pd(100, 0.05);
  amount_discount ad(100, 0.05);

  // customers, (name, discount)
  customer c1{"default", nullptr};
  customer c2{"john", &fd};
  customer c3{"joane", &pd};

  // items, (id, name, price, unit, discount)
  article a1{1, "pen", 5, article_unit::piece, nullptr};
  article a2{2, "expensive pen", 15, article_unit::piece, &fd};
  article a3{3, "scissors", 10, article_unit::piece, &vd};

  cumulative_price_calculator calc;

  // order, (order id, customer, ordered items, discount)
  order o1{101, &c1, {{a1, 1, nullptr}}, nullptr};
  EXPECT_THAT(calc.calculate_price(o1), 5);

  order o2{102, &c2, {{a1, 1, nullptr}}, nullptr};
  EXPECT_THAT(calc.calculate_price(o2), 4.5);
}

/*
={=============================================================================
cxx_pattern_observer_notifier_simple

o this pattern is the same as the observer. The observer side uses
  observer/callback/listener interface ABC and concrete observers have to
  implement them.

o The source/subject side inherites Notifier<T> has T* and T is interface ABC
  and T points to observer. Notifier<T> provides notification functinality and a
  subclass, source, inherits to have them. This is main difference to observer
  pattern.

o The both have to use multiple inheritance if want to supports multiple
  callback interfaces.

*/

namespace cxx_notifier_simple_ex_2019_12
{
  // A observer(callback) interface ABC
  class StateEvents
  {
  public:
    virtual void stateChanged(int state)                                = 0;
    virtual void nameChanged(std::string name)                          = 0;
    virtual void keyAndValueChanged(std::string key, std::string value) = 0;
    virtual void eventOccurred()                                        = 0;
  };

  class Polymorphic
  {
  public:
    virtual ~Polymorphic() {}
  };

  // source. supports single observer

  // necessary?
  // template <typename T> class Notifier : virtual public Polymorphic

  template <typename T>
  class Notifier : public Polymorphic
  {
  private:
    std::shared_ptr<T> observer_;

    // implies that `observer_` is shared pointer which is target object and
    // calls its member function.
    void notify_impl_(std::function<void(const std::shared_ptr<T> &)> f)
    {
      if (observer_)
        f(observer_);
    }

  protected:
    template <typename F>
    void notify(F f)
    {
      notify_impl_(f);
    }

    // as `Source::setState()` below shows, user have to use bind() to call.
    // To provide more convenient form to user:
    template <typename F, typename... Args>
    void notify(F f, Args &&... args)
    {
      notify(std::bind(f, std::placeholders::_1, std::forward<Args>(args)...));
    }

  public:
    Notifier() {}

    void addObserver(const std::shared_ptr<T> &observer)
    {
      observer_ = observer;
    }
  };

  class Source : public Notifier<StateEvents>
  {
  public:
    void setState(int state)
    {
      // calls 'template <typename F> void notify(F f)'
      notify(
        std::bind(&StateEvents::stateChanged, std::placeholders::_1, state));
    }

    void setName(std::string name)
    {
      // use `convenient form`
      notify(&StateEvents::nameChanged, name);
    }

    void setKeyAndValue(std::string key, std::string value)
    {
      notify(&StateEvents::keyAndValueChanged, key, value);
    }

    void emitEvent() { notify(&StateEvents::eventOccurred); }
  };

  // observer

  // necessry?
  // template <class T> class Observer : public T, virtual public Polymorphic

  template <typename T>
  class Observer : public T, public Polymorphic
  {};

  class Observer1 : public Observer<StateEvents>
  {
  public:
    // if use gmock
    // MOCK_METHOD1(stateChanged, void (int));
    // MOCK_METHOD1(nameChanged, void (std::string));
    // MOCK_METHOD2(keyAndValueChanged, void (std::string, std::string));
    // MOCK_METHOD0(eventOccured, void());

  public:
    virtual void stateChanged(int state)
    {
      std::cout << "Observer1::stateChanged(" << state << ") called"
                << std::endl;
    }

    virtual void nameChanged(std::string name)
    {
      std::cout << "Observer1::nameChanged(" << name << ") called" << std::endl;
    }

    virtual void keyAndValueChanged(std::string key, std::string value)
    {
      std::cout << "Observer1::keyAndValueChanged(" << key << "," << value
                << ") called" << std::endl;
    }

    virtual void eventOccurred()
    {
      std::cout << "Observer1::eventOccurred() called" << std::endl;
    }
  };

} // namespace cxx_notifier_simple_ex_2019_12

// [ RUN      ] PatternObserver.NotifierSimple
// Observer1::stateChanged(5) called
// Observer1::nameChanged(notifier) called
// Observer1::keyAndValueChanged(name,Observer1) called
// [       OK ] PatternObserver.NotifierSimple (0 ms)

TEST(PatternObserver, NotifierSimple)
{
  using namespace cxx_notifier_simple_ex_2019_12;

  Source source;

  auto observer = std::make_shared<Observer1>();

  source.addObserver(observer);
  source.setState(5);
  source.setName("notifier");
  source.setKeyAndValue("name", "Observer1");
}

/*
={=========================================================================
cxx_pattern_dispatcher

*/

namespace cxx_dispatcher_ex_2019_12
{
  // Inherit from this for all types that have virtual functions. Doing so
  // ensures that you have virtual destructor and saves you nasty surprises.
  class Polymorphic
  {
  public:
    virtual ~Polymorphic() {}
  };

  // IDispatcher.h
  class Dispatcher : public Polymorphic
  {
  public:
    // post an `work` to be run
    virtual void post(std::function<void()>) = 0;

    // ensure that any works in the queue before tha call has been run
    virtual void sync() = 0;

    // check if it's called from this dispatcher thread
    virtual bool invoked_from_this() = 0;
  };

  // ThreadedDispatcher.h
  class ThreadedDispatcher : public Dispatcher
  {
  private:
    std::thread t_;
    std::mutex m_;
    std::condition_variable cv_;

    std::deque<std::function<void()>> q_;

    bool running_;

    void do_work_(const std::string &name, int priority);
    std::function<void()> next_();

  public:
    ThreadedDispatcher(const std::string &name = std::string());
    ThreadedDispatcher(int priority, const std::string &name = std::string());
    ~ThreadedDispatcher();

    // perform any remaining work in the queue and stop taking new work
    void flush();

    // stop a dispatcher even if there are works in the queue
    void stop();

  public:
    virtual void post(std::function<void()>);
    virtual void sync();
    virtual bool invoked_from_this();
  };

  // *cxx-error* : ISO C++ forbids taking the address of an
  // unqualified or parenthesized non-static member function to form a pointer
  // to member function.
  // Say ‘&cxx_pattern_dispatcher::ThreadedDispatcher::_do_work’ [-fpermissive]
  //
  // _t(std::thread(&_do_work, this, name, priority))

  ThreadedDispatcher::ThreadedDispatcher(int priority, const std::string &name)
      : running_(true)
      , t_(std::thread(&ThreadedDispatcher::do_work_, this, name, priority))
  {}

  // see that ctor calls ctor
  ThreadedDispatcher::ThreadedDispatcher(const std::string &name)
      : ThreadedDispatcher(-1, name)
  {}

  ThreadedDispatcher::~ThreadedDispatcher()
  {
    if (running_)
      stop();
  }

  void ThreadedDispatcher::do_work_(const std::string &name, int priority)
  {
    if (prctl(PR_SET_NAME,
              name.empty() ? "DISPATCHER" : name.c_str(),
              0,
              0,
              0) < 0)
    {
      // LOG_ERR("failed to set thread name");
    }

    // if (priority > 0)
    // {
    //   struct sched_param param;
    //   param.sched_priority = priority;
    //   int err = pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    //   if (err != 0)
    //   {
    //     AI_LOG_SYS_ERROR(err, "Failed to set thread priority to %d",
    //     priority);
    //   }
    // }

    std::unique_lock<std::mutex> lock(m_);

    // 1. exit from wait() when running_ is false which means when stop() is
    // called and there are works in the queue, runs one work and ends dispatch
    // thread.
    //
    // 2. the original code:
    //
    // bool ThreadedDispatcher::hasMoreWorkOrWasStopRequested()
    // { return !q.empty() || !running; }
    //
    // cv.wait(lock, bind(&This::hasMoreWorkOrWasStopRequested, this));

    while (running_)
    {
      // wait until q is not empty or running is false
      cv_.wait(lock, [&] { return !q_.empty() || !running_; });

      // check if wakes up due to q entry
      if (!q_.empty())
      {
        auto work = next_();

        // while calling `work()`, allow uses to post more works
        lock.unlock();
        // this makes `void f()`
        work();
        lock.lock();
      }
    }
  }

  std::function<void()> ThreadedDispatcher::next_()
  {
    // necessary? seems not
    // auto work = std::move(q_.front());

    auto work = q_.front();
    q_.pop_front();
    return work;
  }

  // this is original code. so the difference is to use unique_lock for fine
  // control. does it run faster??
  //
  // std::uniqie_lock<std::mutex> lock(m);
  // if (running)
  // {
  //   q.push_back(work);
  //   lock.unlock();
  //   cv.notify_one();
  // }
  // else
  // {
  //   ...
  // }

  void ThreadedDispatcher::post(std::function<void()> work)
  {
    std::lock_guard<std::mutex> lock(m_);

    if (running_)
    {
      q_.push_back(work);
      cv_.notify_one();
    }
    else
    {
      std::cout << "ignore work as the dispatcher is not running" << std::endl;

      // NOTE: when occurrs?
      // original comment:
      // can't throw an exception here because if this is executed from
      // destructor, which occurs when work adds more work things go horribly
      // wrong. Instead, ignore work.
    }
  }

  bool ThreadedDispatcher::invoked_from_this()
  {
    // see if calling thread is the same as dispatcher t_ thread
    return (std::this_thread::get_id() == t_.get_id());
  }

  namespace
  {
    // 1. the original uses unique_lock and it seems unnecessary
    // 2. when dispatcher calls this, how does it gets all arguments? all
    //    arguments are vaild to use? through std::bind()
    // 3. use of pointer but not reference which is used in flush()

    void syncCallback_(std::mutex *m, std::condition_variable *cv, bool *done)
    {
      // NOTE: using lock_guard is a problem? nope.
      // std::unique_lock<std::mutex> lock(*m);
      std::lock_guard<std::mutex> lock(*m);
      *done = true;
      cv->notify_all();
      // unnecessary
      // lock.unlock();
    }
  } // namespace

  // ensure that any works in the queue before tha call has been run
  void ThreadedDispatcher::sync()
  {
    std::mutex sm;
    std::condition_variable scv;
    bool done{false};

    std::unique_lock<std::mutex> lock(m_);

    if (!running_)
    {
      std::cout << "ignoring sync request because the dispatcher is not running"
                << std::endl;
      return;
    }

    q_.push_back(std::bind(syncCallback_, &sm, &scv, &done));
    lock.unlock();

    // signal dispatcher to run
    cv_.notify_one();

    // wait for `sync work` to be done
    std::unique_lock<std::mutex> sync_lock(sm);

    // same as wait(sync_lock, predicate) *cxx-condition-variable-wait*
    while (!done)
    {
      scv.wait(sync_lock);
    }
  }

  namespace
  {
    // original code
    // void unlockAndSetFlagToFalse(std::mutex& m, bool& flag)
    // {
    //   using namespace std;
    //   m.unlock();
    //   flag = false;
    // }

    void unlockAndSetFlagToFalse_(std::mutex &m, bool &flag)
    {
      // NOTE: okay without this since flush() calls stop() which do set flag
      // false?
      // flag = false;
      m.unlock();
    }
  } // namespace

  // void ThreadedDispatcher::flush()
  // {
  //   if (running_)
  //   {
  //     std::mutex m2;
  //     m2.lock();
  //     // *cxx-bind*
  //     post(std::bind(unlockAndSetFlagToFalse,
  //                    std::ref(m2),
  //                    std::ref(this->running_)));
  //     // blocks here until unlockAndSetFlagToFalse() unlock it since post()
  //     will
  //     // signal and it make signal false as stop() do.
  //     m2.lock();
  //     m2.unlock();
  //     stop();
  //   }
  //   else
  //   {
  //     std::cout
  //       << "ignoring flush request because the dispatcher is not running"
  //       << std::endl;
  //   }
  // }

  void ThreadedDispatcher::flush()
  {
    std::mutex fm;

    if (!running_)
    {
      std::cout << "ignoring sync request because the dispatcher is not running"
                << std::endl;
      return;
    }

    fm.lock();

    post(std::bind(unlockAndSetFlagToFalse_,
                   std::ref(fm),
                   std::ref(this->running_)));

    // NOTE:
    // 1. unlike sync() which use condition variable, use mutex and `double
    // lock` on mutex waits for flush callback is called
    //
    // 2. flush() do the same as sync and do stop() as well.

    fm.lock();
    fm.unlock();
    stop();
  }

  void ThreadedDispatcher::stop()
  {
    std::unique_lock<std::mutex> lock(m_);
    running_ = false;
    lock.unlock();

    cv_.notify_one();
    t_.join();
  }

} // namespace cxx_dispatcher_ex_2019_12

namespace
{
  void work1(int &what, int value) { what += value; }

  class Work
  {
  private:
    int &what_; // see reference
    int value_;

  public:
    Work(int &what, int value)
        : what_(what)
        , value_(value)
    {}

    void assign() { what_ += value_; }
  };
} // namespace

TEST(PatternDispatcher, checkPostedWorkDone)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // single work
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(work1, std::ref(value), 10));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 10);
  }

  // more work
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(work1, std::ref(value), 10));
    td->post(std::bind(work1, std::ref(value), 10));
    td->post(std::bind(work1, std::ref(value), 10));
    td->post(std::bind(work1, std::ref(value), 10));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 40);
  }
}

TEST(PatternDispatcher, checkPostedMemberFunctionDone)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // single work
  {
    int value{};
    Work w1(std::ref(value), 10);

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(&Work::assign, &w1));

    std::this_thread::sleep_for(chrono::seconds(1));
    EXPECT_THAT(value, 10);
  }
}

TEST(PatternDispatcher, checkFlush)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // do one work and uses flush()
  {
    int value{};
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(work1, std::ref(value), 10));
    td->flush();

    // since flush() used so no need to sleep
    // std::this_thread::sleep_for(chrono::seconds(2));

    EXPECT_THAT(value, 10);
  }
}

namespace
{
  void checkThreadId_(std::thread::id id)
  {
    EXPECT_NE(std::this_thread::get_id(), id);
  }
} // namespace

TEST(PatternDispatcher, checkDoneOnDispatcher)
{
  using namespace cxx_dispatcher_ex_2019_12;

  {
    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    // expect false since calling thread is not dispatcher
    EXPECT_THAT(td->invoked_from_this(), false);

    td->post(std::bind(checkThreadId_,
                       std::this_thread::get_id() // calling thread id
                       ));

    std::this_thread::sleep_for(chrono::seconds(1));
  }
}

namespace
{
  void saveSequence_(int &value)
  {
    static int sequence{33};
    value = ++sequence;
  }
} // namespace

TEST(PatternDispatcher, checkDoneInOrder)
{
  using namespace cxx_dispatcher_ex_2019_12;

  {
    int v1{};
    int v2{};
    int v3{};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(saveSequence_, std::ref(v1)));
    td->post(std::bind(saveSequence_, std::ref(v2)));
    td->post(std::bind(saveSequence_, std::ref(v3)));

    std::this_thread::sleep_for(chrono::seconds(1));

    EXPECT_THAT(v1, 34);
    EXPECT_THAT(v2, 35);
    EXPECT_THAT(v3, 36);
  }
}

// do not see possibilities of deadlock in using stop()
TEST(PatternDispatcher, checkStopDoNotCauseDeadlock)
{
  using namespace cxx_dispatcher_ex_2019_12;

  {
    int v1{};
    int v2{};
    int v3{};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    td->post(std::bind(saveSequence_, std::ref(v1)));
    td->post(std::bind(saveSequence_, std::ref(v2)));
    td->post(std::bind(saveSequence_, std::ref(v3)));

    td->stop();
  }
}

namespace
{
  void doIncrements_(int &value)
  {
    ++value;
    std::this_thread::sleep_for(chrono::milliseconds(10));
  }

  void doDecrements_(int &value)
  {
    {
      --value;
      std::this_thread::sleep_for(chrono::milliseconds(20));
    }
  }
} // namespace

TEST(PatternDispatcher, doLostOfWorks)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // single work
  {
    int value{};
    const int count{100000};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    for (int i = 0; i < count; ++i)
    {
      td->post(std::bind(doIncrements_, std::ref(value)));
    }

    td->flush();

    // expect that the number of work done must be count value.
    EXPECT_THAT(value, count);
  }

  // multiple works
  {
    int value{};
    const int count{100000};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    for (int i = 0; i < count; ++i)
    {
      if ((i % 2) == 0)
        td->post(std::bind(doIncrements_, std::ref(value)));
      else
        td->post(std::bind(doDecrements_, std::ref(value)));
    }

    td->flush();

    // expect that the number of work done must be count value.
    EXPECT_THAT(value, 0);
  }
}

namespace
{
  void notify_(std::mutex &m, std::condition_variable &cv)
  {
    std::lock_guard<std::mutex> lock(m);
    cv.notify_one();
  }
} // namespace

TEST(PatternDispatcher, checkDonwInTimeout)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // since there is no determistic way to check how many works can be done in
  // certain time, do one test in timeout.
  {
    // std::shared_ptr<ThreadedDispatcher> td =
    //   std::make_shared<ThreadedDispatcher>();

    auto td = std::make_shared<ThreadedDispatcher>();

    std::mutex m;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(m);

    td->post(std::bind(notify_, std::ref(m), std::ref(cv)));

    // *cxx-condition-variable-wait*
    // check if condition is signaled within timeout value
    EXPECT_THAT(cv.wait_for(lock, std::chrono::seconds(5)),
                std::cv_status::no_timeout);
  }
}

TEST(PatternDispatcher, checkSync)
{
  using namespace cxx_dispatcher_ex_2019_12;

  // single work
  {
    int value{};
    const int count{100000};

    std::shared_ptr<ThreadedDispatcher> td =
      std::make_shared<ThreadedDispatcher>();

    for (int i = 0; i < count; ++i)
    {
      td->post(std::bind(doIncrements_, std::ref(value)));
    }

    td->sync();

    // expect that the number of work done must be count value.
    EXPECT_THAT(value, count);
  }
}

/*
={=========================================================================
cxx_pattern_observer_notifier_full

*/

namespace cxx_notifier_full_ex_2019_12
{
  // A observer(callback) interface ABC
  class StateEvents
  {
  public:
    virtual void stateChanged(int state)                                = 0;
    virtual void nameChanged(std::string name)                          = 0;
    virtual void keyAndValueChanged(std::string key, std::string value) = 0;
    virtual void eventOccurred()                                        = 0;
  };

  // Inherit from this for all types that have virtual functions. Doing so
  // ensures that you have virtual destructor and saves you nasty surprises.
  class Polymorphic
  {
  public:
    virtual ~Polymorphic() {}
  };

  // IDispatcher.h
  class Dispatcher : public Polymorphic
  {
  public:
    // post an `work` to be run
    virtual void post(std::function<void()>) = 0;

    // ensure that any works in the queue before tha call has been run
    virtual void sync() = 0;

    // check if it's called from this dispatcher thread
    virtual bool invoked_from_this() = 0;
  };

  // source. supports `multiple` observer

  // necessary?
  // template <typename T> class Notifier : virtual public Polymorphic

  template <typename T>
  class Notifier : public Polymorphic
  {
  private:
    std::mutex m_;
    std::condition_variable cv_;
    std::deque<std::weak_ptr<T>> observers_;

    std::shared_ptr<Dispatcher> dispatcher_;
    bool notifying_{false};
    uint32_t waitee_count_{0};

    void notify_impl_(std::function<void(const std::shared_ptr<T> &)> f)
    {
      std::unique_lock<std::mutex> lock(m_);

      if (!dispatcher_)
      {
        throw std::logic_error(
          "must set a dispatcher before you produces events");
      }

      // In the unlikely event where there are expired observers, remove expired
      // observers by copying only if weak_ptr.use_count() > 0 since if
      // corresponding shared pointer is gone, then use_count() of weak pointer
      // gets decreased.

      decltype(observers_) observers_copy;

      std::copy_if(
        observers_.cbegin(),
        observers_.cend(),
        std::back_inserter(observers_copy),
        std::bind(&std::weak_ptr<T>::use_count, std::placeholders::_1));

      // update observers_
      if (observers_copy.size() != observers_.size())
        observers_ = observers_copy;

      // okay, now we have all living observers so starts notifying
      notifying_ = true;

      // do not want to block adding observers while notifying them so use a
      // copy instead.

      lock.unlock();

      //----------------- NOTE ----------------------------------------------
      // We maintain vector of strong pointers pointing to observer objects as
      // otherwise bad things can happen. Lets consider, the observer object
      // point backs to the notifier object itself.
      //
      // NOTE: cannot see where the observer points back to the notifier?
      //
      // That means, there is a circular dependency between the notifier and the
      // observer, but we break that by using a combination of shared and weak
      // pointers.  However, imagine, within the notify_impl() method, we gets a
      // shared pointer of observer object out of weak_ptr. After the shared
      // pointer is constructed (bit still in use), now the owner of the
      // observer resets its pointer that is pointing to the observer object.
      // This might result one to one references between the notifier and the
      // observer, i.e., as soon as the observer will be destroyed the notifier
      // will also be destroyed. It means, if now the observer object is
      // destroyed from the notify_imp() method, it will cause the notifier
      // object itself to be destroyed, where the notify_impl can still continue
      // to access its member variable (e.g. dispatcher). This might result an
      // undefined behaviour.
      //---------------------------------------------------------------------

      // Q: WHY need observers_strong vector? that's NOTE above but not sure?
      std::vector<std::shared_ptr<T>> observers_strong;

      for (auto const &o : observers_copy)
      {
        std::shared_ptr<T> strong = o.lock();
        if (strong)
        {
          // dispatcher expects void()
          dispatcher_->post(std::bind(f, strong));
        }
        observers_strong.push_back(strong);
      }

      // okay, notifying finishes
      lock.lock();

      // about to unregister an observer so make sure that there is no work
      // for this observer after that.
      if (dispatcher_ && (waitee_count_ > 0))
      {
        lock.unlock();
        // run all works in dispatcher queue. so not only for this observer
        // but for all.
        dispatcher_->sync();
        lock.lock();
      }

      notifying_ = false;

      // notify all waited observers on remove_observer()
      if (waitee_count_ > 0)
        cv_.notify_all();

      lock.unlock();
    }

  protected:
    template <typename F>
    void notify(F f)
    {
      notify_impl_(f);
    }

    template <typename F, typename... Args>
    void notify(F f, Args &&... args)
    {
      notify_impl_(
        std::bind(f, std::placeholders::_1, std::forward<Args>(args)...));
    }

  public:
    explicit Notifier() {}

    void addObserver(const std::shared_ptr<T> &observer)
    {
      std::lock_guard<std::mutex> lock(m_);
      observers_.emplace_back(observer);
    }

    void removeObserver(const std::shared_ptr<T> &observer)
    {
      std::unique_lock<std::mutex> lock(m_);

      // if (_BUILD_TYPE == DEBUG)
      if (dispatcher_ && dispatcher_->invoked_from_this())
      {
        throw std::logic_error(
          "potential deadlock since it should not be called from dispatcher");
      }
      // #endif

      // 1. have to use this form using index for searching since have to use
      // weak_ptr.lock().
      // 2. if shared pointer can be made from a weak pointer, it means a
      // object is still around and it is safe to use.

      for (size_t i = 0; i < observers_.size(); ++i)
      {
        if (observers_[i].lock() == observer)
        {
          observers_.erase(observers_.begin() + i);
        }
      }

      // observers can call this while notification from source is pending so
      // to make observers is still around, hold them until notification
      // finishes.
      // TODO: to do that, should it be moved before `deleting it`? that is to
      // move before `for loop` to search?

      if (notifying_)
      {
        waitee_count_++;

        // cxx-condition-variable-wait
        // same as `wait(lock, condition)` and blocks the current thread until
        // the condition variable is woken up.
        while (notifying_)
        {
          cv_.wait(lock);
        }

        waitee_count_--;
      }
    }

    void setDispatcher(const std::shared_ptr<Dispatcher> &dispatcher)
    {
      std::lock_guard<std::mutex> lock(m_);
      dispatcher_ = dispatcher;
    }
  };

  class Source : public Notifier<StateEvents>
  {
  public:
    void setState(int state)
    {
      // calls 'template <typename F> void notify(F f)'
      notify(
        std::bind(&StateEvents::stateChanged, std::placeholders::_1, state));
    }

    void setName(std::string name)
    {
      // use `convenient form`
      notify(&StateEvents::nameChanged, name);
    }

    void setKeyAndValue(std::string key, std::string value)
    {
      notify(&StateEvents::keyAndValueChanged, key, value);
    }

    void emitEvent() { notify(&StateEvents::eventOccurred); }
  };

  // observer
  // NOTE:
  // Arguably you could inherit directly from T, however inheriting from
  // Observer<T> is more intention revealing than inheriting from T. There is
  // no extra overhead because of Empty Base Class Optimisation.

  // necessary?
  // template <class T> class Observer : public T, virtual public Polymorphic

  template <typename T>
  class Observer : public T, public Polymorphic
  {};

  class Observer1 : public Observer<StateEvents>
  {
  public:
    // if use gmock
    MOCK_METHOD1(stateChanged, void(int));
    MOCK_METHOD1(nameChanged, void(std::string));
    MOCK_METHOD2(keyAndValueChanged, void(std::string, std::string));
    MOCK_METHOD0(eventOccurred, void());
  };

  class ObserverWrong : public Polymorphic
  {
  public:
    virtual void stateOccurred()
    {
      std::cout << "stateOccurred()" << std::endl;
    }
  };

  // Common/lib/include/CallerThreadDispatcher.h
  // do not use seperate thread for dispatcher and simply use caller thread as
  // if it's a `real` dispatcher

  class CallerThreadedDispatcher : public Dispatcher
  {
  public:
    virtual void post(std::function<void()> work) final { work(); }
    virtual void sync() final {}
    virtual bool invoked_from_this() final { return false; }
  };

} // namespace cxx_notifier_full_ex_2019_12

// what if use unrelated observer? type error at compile.
//
// test_cxx_ex.cpp:794:23: error: no matching function for call to
// ‘cxx_notifier_full_ex_2019_12::Source::addObserver(
//    std::shared_ptr<cxx_notifier_full_ex_2019_12::ObserverWrong>&)’
//   source.addObserver(o);
#if 0
TEST(PatternObserver, tryUnrelatedObserver)
{
  using namespace cxx_notifier_full_ex_2019_12;

  Source source;
  source.setDispatcher(std::make_shared<CallerThreadedDispatcher>());

  std::shared_ptr<ObserverWrong> o = std::make_shared<ObserverWrong>();
  source.addObserver(o);
}
#endif

#if 0

// can use real dispatcher but since not able to control dispatcher directly
// such as sync(), flush() or wait it to finish, cannot have deterministic
// result. Hence use CallerThreadDispatcher which call a work on calling thread.

TEST(PatternNotifierFull, sendNotificationWithRealDispatcher)
{
  using namespace cxx_notifier_full_ex_2019_12;

  Source noti;
  std::shared_ptr<Observer1> observer = std::make_shared<Observer1>();
  noti.set_dispatcher(std::make_shared<ThreadedDispatcher>());
  noti.add_observer(observer);

  EXPECT_CALL(*observer, stateChanged(5)).Times(1);

  noti.setState(5);
}
#endif

TEST(PatternNotifierFull, sendNotificationWithCallerDispatcher)
{
  using namespace cxx_notifier_full_ex_2019_12;

  Source source;
  source.setDispatcher(std::make_shared<CallerThreadedDispatcher>());

  std::shared_ptr<Observer1> o1 = std::make_shared<Observer1>();
  std::shared_ptr<Observer1> o2 = std::make_shared<Observer1>();
  source.addObserver(o1);
  source.addObserver(o2);

  EXPECT_CALL(*o1, stateChanged(5)).Times(1);
  EXPECT_CALL(*o2, stateChanged(5)).Times(1);
  source.setState(5);
}

TEST(PatternNotifierFull, sendNotificationManyArgs)
{
  using namespace cxx_notifier_full_ex_2019_12;

  Source noti;
  std::shared_ptr<Observer1> observer = std::make_shared<Observer1>();
  noti.setDispatcher(std::make_shared<CallerThreadedDispatcher>());
  noti.addObserver(observer);

  EXPECT_CALL(*observer, keyAndValueChanged("key", "value")).Times(1);

  noti.setKeyAndValue("key", "value");
}

/*
={=========================================================================
cxx_pattern_singleton

*/

namespace cxx_singleton
{
  class Foo1
  {
    private:
      std::string name_{};
      int count_{0};
      int calls_{0};
      static Foo1 *instance_;

      // to wait for random
      std::default_random_engine dre;
      std::uniform_int_distribution<int> id;

    public:
      // use `count_` to check num of instance created
      static Foo1 *instance()
      {
        if (nullptr == instance_)
        {
          instance_ = new Foo1("Foo1");
          instance_->increase();
        }

        return instance_;
      }

      Foo1(const Foo1&) = delete;
      Foo1& operator=(const Foo1&) = delete;

      // a destructor?
      // No need to delete _instance? Yes, will be reclaimed when an application
      // terminates.

    public:
      std::string name() const
      { return name_; }

      void increase() { ++count_; }

      // to give calling thread waiting time
      void exec() 
      { 
        std::this_thread::sleep_for(std::chrono::milliseconds(id(dre)));
        ++calls_; 
      }

      int calls() { return calls_; }

      int count() const
      { return count_; }

    private:
      explicit Foo1(std::string const &name)
          : name_(name), dre('r'),id(10, 1000)
      {}
  };

  Foo1 *Foo1::instance_ = nullptr;

  int instance_count = 0;

  // when use reference
  class Foo2
  {
    private:
      std::string name_{};
      int count_{0};
      int calls_{0};

      // to wait for random
      std::default_random_engine dre;
      std::uniform_int_distribution<int> id;

    public:
      // use `count_` to check num of instance created
      static Foo2 &instance()
      {
        static Foo2 instance_("Foo2");
        return instance_;
      }

      Foo2(const Foo2&) = delete;
      Foo2& operator=(const Foo2&) = delete;

      // a destructor?
      // No need to delete _instance? Yes, will be reclaimed when an application
      // terminates.

    public:
      std::string name() const
      { return name_; }

      void increase() { ++count_; }

      // to give calling thread waiting time
      void exec() 
      { 
        std::this_thread::sleep_for(std::chrono::milliseconds(id(dre)));
        ++calls_; 
      }

      int calls() { return calls_; }

      int count() const
      { return count_; }

    private:
      explicit Foo2(std::string const &name)
          : name_(name), dre('r'),id(10, 1000)
      {
        ++count_;
      }
  };
} // namespace cxx_singleton

TEST(PatternSingleton, check_on_single_thread_1)
{
  using namespace cxx_singleton;

  EXPECT_THAT(Foo1::instance()->name(), "Foo1");
  EXPECT_THAT(Foo1::instance()->count(), 1);

  // uses `instance` more
  EXPECT_THAT(Foo1::instance()->name(), "Foo1");
  EXPECT_THAT(Foo1::instance()->name(), "Foo1");

  // count stays the same
  EXPECT_THAT(Foo1::instance()->count(), 1);
}

TEST(PatternSingleton, check_on_single_thread_2)
{
  using namespace cxx_singleton;

  EXPECT_THAT(Foo2::instance().name(), "Foo2");
  EXPECT_THAT(Foo2::instance().count(), 1);

  // uses `instance` more
  EXPECT_THAT(Foo2::instance().name(), "Foo2");
  EXPECT_THAT(Foo2::instance().name(), "Foo2");

  // count stays the same
  EXPECT_THAT(Foo2::instance().count(), 1);
}

namespace cxx_singleton
{
  void do_access_1()
  {
#ifndef DISABLE_WAIT
    // to wait for random
    std::default_random_engine dre('r');
    std::uniform_int_distribution<int> id(10, 1000);

    std::this_thread::sleep_for(std::chrono::milliseconds(id(dre)));
#endif

    EXPECT_THAT(Foo1::instance()->name(), "Foo1");
    std::cout << std::this_thread::get_id() << " ends" << std::endl;
  }

  void do_access_2()
  {
    for (size_t i = 0; i < 10; ++i)
    {
#ifndef DISABLE_WAIT
      // to wait for random
      std::default_random_engine dre('r');
      std::uniform_int_distribution<int> id(10, 1000);

      std::this_thread::sleep_for(std::chrono::milliseconds(id(dre)));
#endif

      Foo1::instance()->exec();
    }
  }
}

// creates multiple threads which uses singleton to see how it works
// thread safe as cxx11 supports
TEST(PatternSingleton, check_on_multiple_thread_1)
{
  using namespace cxx_singleton;

  std::vector<std::future<void>> threads;

  // lets creates multiple threads to run
  for (int i = 0; i < 10; ++i)
  {
    auto result = std::async(std::launch::async, do_access_1);

    // use std::move() and otherwise, compile error
    threads.emplace_back(std::move(result));

    std::cout << std::this_thread::get_id() << " ends" << std::endl;
  }

  for (int i = 0; i < 10; ++i)
  {
    threads[i].get();
    std::cout << "threads " << i << " ends" << std::endl;
  }

  // uses `instance` more
  EXPECT_THAT(Foo1::instance()->name(), "Foo1");
  EXPECT_THAT(Foo1::instance()->name(), "Foo1");

  // count stays the same
  EXPECT_THAT(Foo1::instance()->count(), 1);
}

// ok, `initialisation` is thread-safe. how about meember function?
//
// as you can see it fails sometimes and this when #thread is 10
//
// [ RUN      ] PatternSingleton.check_on_multiple_thread_2
// threads 0 ends
// threads 1 ends
// threads 2 ends
// threads 3 ends
// threads 4 ends
// threads 5 ends
// threads 6 ends
// threads 7 ends
// threads 8 ends
// threads 9 ends
// /home/keitee/git/kb/code-cxx/cxx/test_cxx_pattern.cpp:3245: Failure
// Value of: Foo1::instance()->calls()
// Expected: is equal to 100
//   Actual: 98 (of type int)
// [  FAILED  ] PatternSingleton.check_on_multiple_thread_2 (7160 ms)

TEST(PatternSingleton, check_on_multiple_thread_2)
{
  using namespace cxx_singleton;

  constexpr auto NUM_THREADS{10};

  std::vector<std::future<void>> threads;

  // lets creates multiple threads to run
  for (int i = 0; i < NUM_THREADS; ++i)
  {
    auto result = std::async(std::launch::async, do_access_2);

    // use std::move() and otherwise, compile error
    threads.emplace_back(std::move(result));

    // std::cout << std::this_thread::get_id() << " starts" << std::endl;
  }

  for (int i = 0; i < NUM_THREADS; ++i)
  {
    threads[i].get();
    std::cout << "threads " << i << " ends" << std::endl;
  }

  // uses `instance` more
  EXPECT_THAT(Foo1::instance()->name(), "Foo1");

  // count stays the same
  EXPECT_THAT(Foo1::instance()->calls(), NUM_THREADS * 10);
}

// https://www.modernescpp.com/index.php/thread-safe-initialization-of-a-singleton
//
// I use in the reference implementation the so called Meyers Singleton. The
// elegance of this implementation is that the singleton object instance in line
// 11 is a static variable with block scope. Therefore, instance will exactly be
// initialized, when the static method getInstance (line 10 - 14) will be
// executed the first time. In line 14 the volatile variable dummy is commented
// out. When I translate the program with maximum optimization that has to
// change. So the call MySingleton::getInstance() will not be optimized away.
//
// Guarantees of the C++ runtime
// I already presented the details to the thread safe initialization of
// variables in the post Thread safe initialization of data.
//
// Meyers Singleton
// The beauty of the Meyers Singleton in C++11 is that it's automatically thread
// safe. That is guaranteed by the standard: Static variables with block scope.
// The Meyers Singleton is a static variable with block scope, so we are done.
// It's still left to rewrite the program for four threads.
//
//
// http://www.open-std.org/JTC1/SC22/WG21/
//
// https://stackoverflow.com/questions/8102125/is-local-static-variable-initialization-thread-safe-in-c11
// C++ The relevant section 6.7:
//
// such a variable is initialized the first time control passes through its
// declaration; such a variable is considered initialized upon the completion of
// its initialization. [...] If control enters the declaration concurrently
// while the variable is being initialized, the concurrent execution shall wait
// for completion of the initialization.
//
// https://www.modernescpp.com/index.php/thread-safe-initialization-of-data
//
// #9 Neo 2018-12-26 12:34
//
// "There are three ways in C++ to initialize variables in a thread safe way."
//
// What about non-local static variables?
//
// static MyObject myObject; // ctor executed before main
//
// aren't they thread safe too?  Quote 0#10 Rainer Grimm 2018-12-29 17:18 Of
// course, they are also thread-safe initialised because there exists only one
// thread.

namespace cxx_singleton
{
  constexpr auto tenMill= 10000000;

  class MySingleton1
  {
    public:
      static MySingleton1& getInstance()
      {
        // NOTE:
        static MySingleton1 instance;

        // volatile int dummy{};
        return instance;
      }
    private:
      MySingleton1()= default;

      // NOTE
      ~MySingleton1()= default;

      MySingleton1(const MySingleton1&)= delete;
      MySingleton1& operator=(const MySingleton1&)= delete;
  };

  std::chrono::duration<double> getTime()
  {
    auto begin= std::chrono::system_clock::now();

    for ( size_t i= 0; i <= tenMill; ++i){
      MySingleton1::getInstance();
    }

    return std::chrono::system_clock::now() - begin;
  };
} // namespace cxx_singleton

TEST(PatternSingleton, reference_1)
{
  using namespace cxx_singleton;

  constexpr auto fourtyMill = 4 * tenMill;

  auto begin = std::chrono::system_clock::now();

  for (size_t i = 0; i <= fourtyMill; ++i)
  {
    MySingleton1::getInstance();
  }

  auto end = std::chrono::system_clock::now() - begin;

  std::cout << std::chrono::duration<double>(end).count() << std::endl;
}

// ok, it runs multiple threads but do not show the problem when use multiple
// threads. Probarly, his intension is to measure performance. 

TEST(PatternSingleton, reference_2)
{
  using namespace cxx_singleton;

  auto fut1= std::async(std::launch::async,getTime);
  auto fut2= std::async(std::launch::async,getTime);
  auto fut3= std::async(std::launch::async,getTime);
  auto fut4= std::async(std::launch::async,getTime);

  auto total= fut1.get() + fut2.get() + fut3.get() + fut4.get();

  std::cout << total.count() << std::endl;
}

namespace cxx_singleton
{
  // constexpr auto tenMill= 10000000;

  class MySingleton2{
    public:
      static MySingleton2& getInstance()
      {
        // volatile int dummy{};
        return instance;
      }
    private:
      MySingleton2()= default;

      // NOTE:
      ~MySingleton2()= default;

      MySingleton2(const MySingleton2&)= delete;
      MySingleton2& operator=(const MySingleton2&)= delete;

      // NOTE:
      static MySingleton2 instance;
  };

  // NOTE:
  MySingleton2 MySingleton2::instance;
} // namespace cxx_singleton

/*
={=========================================================================
cxx_pattern_singleton read

to make a singleton testable

Retiring the Singleton Pattern - Peter Muldoon - Meeting C++ 2019

https://www.youtube.com/watch?v=f46jmm7r8Yg&fbclid=IwAR1YP8kkqLp8AWtYx4DAG5ARdqQtGqGeF5RcJ6-yxbgnOO50Nn0x9Ouamf4

*/

/*
case example

DrmController is singleton and has static callback defined, onStateChange()
since it's called from the third party moudle

Can we use member function as a callback instead?

Yes. not sure about its runtime behaviour but comiles okay
 
1. one problem was compile issue. since it's not copyable, callable from
bind() emits compile error and this can be fixed by using `std::ref`

2. note that JSONRPC here uses std::function<> for a callback and this is why
callable from bind() works

namespace
{
  // before
  class DrmController
  {
    private:
      DrmController() = default;

    public:
      static DrmController &getInstance();
      ~DrmController()                     = default;
      DrmController(const DrmController &) = delete;
      DrmController &operator=(const DrmController &) = delete;

    private:
      static void onStateChange(const Core::JSON::String &parameters);
  };

  void DrmController::initialise() const
  {
    m_drm = new JSONRPC::Client(_T(DRMACTIVATION_CALLSIGN.c_str()), _T(""));
    if (nullptr != m_drm)
    {
      Core::ERROR_NONE !=
             m_drm->Subscribe<Core::JSON::String>(1000,
                                                  _T("drmstatechange"),
                                                  &onStateChange);
    }
  }

  // after
  class DrmController
  {
    private:
      DrmController() = default;

    public:
      static DrmController &getInstance();
      ~DrmController()                     = default;
      DrmController(const DrmController &) = delete;
      DrmController &operator=(const DrmController &) = delete;

    private:
      static void onStateChange(const Core::JSON::String &parameters);
  };

  void DrmController::initialise() const
  {
    auto call = std::bind(&DrmController::onStateChange, 
        std::ref(DrmController::getInstance()),
        std::placeholders::_1);

    m_drm = new JSONRPC::Client(_T(DRMACTIVATION_CALLSIGN.c_str()), _T(""));
    if (nullptr != m_drm)
    {
      Core::ERROR_NONE !=
          m_drm->Subscribe<Core::JSON::String>(1000, 
              _T("drmstatechange"), call);
    }
  }
}

*/

/*
namespace
{
  // this is function we want to test but which uses singleton

  Response sendData(const Data &data)
  {
    Request req;

    // codes to transform Data into Request

    return CommSingleton::instance()->send(req);
  }


  // o to make "source compatible" via default parameter so old code still use it
  // with single parameter
  //
  // o to express singleton dependancy

  Response sendData(const Data &data,
      CommSingleton *comm = CommSingleton::instance());

  Response sendData(const Data &data, CommSingleton *comm)
  {
    Request req;

    // codes to transform Data into Request

    return comm->send(req);
  }


  // o introduce wrapper to replace singleton
  // o add "virtual" to allow override
  // NOTE: this is a "mock" 

  class CommWrapper
  {
    enum { SERVICE_ID = 249409 };

    public:
    CommWrapper(int service_id = SERVICE_ID)
      : raw_client(service_id) {...}

    virtual Response send(const Request &req);

    private:
    TcpClient raw_client;
  };

  struct Service
  {
    static CommWrapper comm;
  }

  Response sendData(const Data &data,
      CommWrapper &comm = Service::comm);

  Response sendData(const Data &data, CommWrapper &comm)
  {
    Request req;

    // codes to transform Data into Request

    return comm.send(req);
  }

  // mock
  class CommTester : public CommWrapper
  {
    public:
      CommTester(Request &req) : req_(req) {}

      // override
      Response send(const Request &reg) 
      { 
        req_ = req; 
        return Response(); 
      }

      Request &req_;
  };

  // test
  int TestSendData()
  {
    Data data;
    data.id = 999;

    // fill in more `data` values

    Request req;

    // sendData() will fill in `req` and CommTester use reference to save it
    // back so now we can examine what sendData() do on Request `req`

    CommTester client(req);

    sendData(data, client);

    if (req.senderId != data.id)
      std::cout << "Error ..." << std::endl;

    // further validation of rec values...
  }


  // o if use gmock

  class MockComm : public CommWrapper
  {
    public:
      MOCK_METHOD1(send, Response(const Request &));
  };

  TEST(CommTest, test_sendData)
  {
    Data data;
    data.id = 999;

    // fill in more `data` values

    Request req;

    MockComm comm;

    EXPECT_CALL(comm, send(_)).WillOnce(DoAll(SaveArg<0>(&req), return(Reponse())));

    sendData(data, comm);

    ASSERT_EQ(req.senderId, data.id);

    // further validation of rec values...
  }


  // o if use modern C++
  // NOTE: why it said it's for Cxx-11?

  // see here uses copy
  using comm_func = std::function<Response(Request)>;

  Response sendData(const Data &data,
      comm_func comm = Service::comm);

  Response sendData(const Data &data, comm_func comm)
  {
    Request req;

    // codes to transform Data into Request

    return comm(std::move(req));
  }


  // to remove copy, can do one of these. function_ref is beyond Cxx-11

  Response sendData(const Data &data,
      comm_func comm = std::ref(Service::comm));

  using comm_func = std::function_ref<Response(Request)>;


  // make it callable
  class CommWrapper
  {
    enum { SERVICE_ID = 249409 };

    public:
    CommWrapper(int service_id = SERVICE_ID)
      : raw_client(service_id) {...}

    // virtual Response send(const Request &req);
    Response operator()(const Request &req);

    private:
    TcpClient raw_client;
  };

  struct Service
  {
    static CommWrapper comm;
  }


  class MockComm : public CommWrapper
  {
    public:
      MOCK_METHOD1(send, Response(Request));
      Reponse operator()(Request reg) { return send(std::move(req)); }
  };

  TEST(CommTest, test_sendData)
  {
    Data data;
    data.id = 999;

    // fill in more `data` values

    Request req;
    Response resp;

    MockComm comm;

    EXPECT_CALL(comm, send(_)).WillOnce(DoAll(SaveArg<0>(&req), return(resp)));

    sendData(data, std::ref(comm));

    ASSERT_EQ(req.senderId, data.id);

    // further validation of rec values...
  }
} // namespace
*/

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
