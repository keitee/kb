#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <chrono>
#include <limits>
#include <thread>
#include <list>
#include <forward_list>
#include <regex>
#include <boost/lexical_cast.hpp>
#include <random>
#include <mutex>
#include <condition_variable>

#include "gmock/gmock.h"

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

                         digit_password_validator, case_password_validator, symbol_password_validator
*/

namespace cxx_pattern_decorator
{
  class password_validator
  {
    public:
      virtual ~password_validator() {}
      // virtual bool validate(std::string& password) = 0;
      // virtual bool validate(std::string password) = 0;
      virtual bool validate(std::string const& password) = 0;
  };

  // mandatory(default) implementation

  class length_validator : public password_validator
  {
    public:
      length_validator(unsigned int min_length) noexcept : length_(min_length) 
      {}

      // bool validate(std::string& password) override
      // bool validate(std::string password) override
      bool validate(std::string const& password) override
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
      explicit password_validator_decorator(std::unique_ptr<password_validator> validator)
        : validator_(std::move(validator)) 
      {}

      // bool validate(std::string& password) override
      // bool validate(std::string password) override
      bool validate(std::string const& password) override
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
      explicit digit_password_validator(std::unique_ptr<password_validator> validator)
        : password_validator_decorator(std::move(validator))
      {}

      // bool validate(std::string& password) override
      // bool validate(std::string password) override
      bool validate(std::string const& password) override
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
      explicit case_password_validator(std::unique_ptr<password_validator> validator)
        : password_validator_decorator(std::move(validator))
      {}

      // bool validate(std::string& password) override
      // bool validate(std::string password) override
      bool validate(std::string const& password) override
      {
        cout << "case_password_validator {" << endl;

        if (password_validator_decorator::validate(password) == false)
          return false;

        bool has_lower{false};
        bool has_upper{false};

        for (size_t i = 0; i < password.size() && !(has_lower && has_upper); ++i)
        {
          if (std::islower(password[i])) has_lower = true;
          else if (std::isupper(password[i])) has_upper = true;
        }

        cout << "case_password_validator }" << endl;

        return has_lower && has_upper;
      }
  };

  // must have one of symbols

  class symbol_password_validator final : public password_validator_decorator
  {
    public:
      explicit symbol_password_validator(std::unique_ptr<password_validator> validator)
        : password_validator_decorator(std::move(validator))
      {}

      // error
      // bool validate(std::string& password) override
      // okay
      // bool validate(std::string password) override
      bool validate(std::string const& password) override
      {
        cout << "symbol_password_validator {" << endl;

        if (password_validator_decorator::validate(password) == false)
          return false;

        cout << "symbol_password_validator }" << endl;

        return password.find_first_of("!@#$%^&*(){}[]?<>") != std::string::npos;
      }
  };

} // namespace


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
      new digit_password_validator(std::unique_ptr<length_validator>(new length_validator(8)))
      );

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
  std::unique_ptr<digit_password_validator> validator2(new digit_password_validator(
        std::unique_ptr<case_password_validator>(new case_password_validator(
            std::unique_ptr<length_validator>(new length_validator(8))
            ))));

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
      virtual std::string generate() = 0;
      virtual std::string allowed_chars() const = 0;
      virtual size_t length() const = 0;
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
      { throw std::runtime_error("not implemented"); }

      virtual void add(std::unique_ptr<password_generator>) override
      { throw std::runtime_error("not implemented"); }

      virtual size_t length() const override final
      { return len_; }

    private:
      size_t len_;
  };

  class digit_generator : public basic_password_generator
  {
    public:
      explicit digit_generator(size_t const length) noexcept
        : basic_password_generator(length)
        {}

      virtual std::string allowed_chars() const override
      { return "0123456789"; }
  };

  class symbol_generator : public basic_password_generator
  {
    public:
      explicit symbol_generator(size_t const length) noexcept
        : basic_password_generator(length)
        {}

      virtual std::string allowed_chars() const override
      { return "!@#$%^&*(){}[]?<>"; }
  };

  class upper_letter_generator : public basic_password_generator
  {
    public:
      explicit upper_letter_generator(size_t const length) noexcept
        : basic_password_generator(length)
        {}

      virtual std::string allowed_chars() const override
      { return "ABCDEFGHIJKLMNOPQRSTUVXYWZ"; }
  };

  class composite_password_generator : public password_generator
  {
    public:
      composite_password_generator()
      {}

      virtual std::string generate() override
      {
        std::string password{};

        for (auto& g : generators_)
        {
          std::string chars = g->allowed_chars();
          std::uniform_int_distribution<> ud(0, static_cast<int>(chars.length()-1));

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
      { throw std::runtime_error("not implemented"); }

      virtual size_t length() const override
      { throw std::runtime_error("not implemented"); }

      std::default_random_engine eng;
      std::vector<std::unique_ptr<password_generator>> generators_;
  };

} // namespace


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
  generator.add(std::unique_ptr<upper_letter_generator>(new upper_letter_generator(2)));

  auto password = generator.generate();
  cout << "password: " << password << endl;
}


/*
={=============================================================================

cxx_pattern_template

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
  enum class sex_type {female, male};

  class social_number_generator
  {
    public:
      std::string generate(sex_type sex, unsigned year, unsigned month, unsigned day)
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
        auto index = number.length();

        // cxx-algo-accumulate
        auto sum = std::accumulate(std::begin(number), std::end(number),
            0, // init value
            [&](int init, char c)
            { return init + (index-- * (c - '0')); });

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
      virtual int sex_digit(sex_type const sex) const noexcept = 0;
      virtual int next_random(unsigned year, unsigned month, unsigned day) = 0;
      virtual int modulo_value() const noexcept = 0;

    protected:
      std::map<unsigned, int> cache;
      std::uniform_int_distribution<> ud;
      std::default_random_engine eng;
  };

  class southeria_social_number_generator final : public social_number_generator
  {
    public:
      southeria_social_number_generator():
        social_number_generator(1000, 9999)
    {}

    protected:
      virtual int sex_digit(sex_type sex) const noexcept override
      {
        return sex == sex_type::female ? 1 : 2;
      }

      // 4 digit.
      // cxx-algo-unique use cache to have unique number?
      virtual int next_random(unsigned year, unsigned month, unsigned day) override
      {
        auto key = year*10000 + month*100 + day;
        while (true)
        {
          auto number = ud(eng);

          // when not found
          auto pos = cache.find(number);
          if (pos == std::end(cache))
          {
            cache[key] = number;
            return number;
          }
        }
      }

      virtual int modulo_value() const noexcept override
      { return 11; }
  };

  class northeria_social_number_generator final : public social_number_generator
  {
    public:
      northeria_social_number_generator():
        social_number_generator(1000, 9999)
    {}

    protected:
      virtual int sex_digit(sex_type sex) const noexcept override
      {
        return sex == sex_type::female ? 9 : 7;
      }

      // 5 digit. why are these two functions same as southeria?
      
      virtual int next_random(unsigned year, unsigned month, unsigned day) override
      {
        auto key = year*10000 + month*100 + day;
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

      virtual int modulo_value() const noexcept override
      { return 11; }
  };

  class social_number_generator_factory
  {
    public:
      social_number_generator_factory()
      {
        generators_["north"] = 
          std::unique_ptr<northeria_social_number_generator>(
              new northeria_social_number_generator());

        generators_["south"] = 
          std::unique_ptr<southeria_social_number_generator>(
              new southeria_social_number_generator());
      }

      social_number_generator* get_generator(std::string country) const
      {
        auto it = generators_.find(country);
        if (it != std::end(generators_))
          return it->second.get();

        throw std::runtime_error("not found country");
      }

    private:
      std::map<std::string,
        std::unique_ptr<social_number_generator>> generators_;
  };

} // namespace

TEST(PatternTemplate, GeneratingSecurityNumber)
{
  using namespace cxx_pattern_template;

  social_number_generator_factory factory;

  {
    auto snorth1 = factory.get_generator("north");
    std::string result;
    if (snorth1 != nullptr)
    {
      result = snorth1->generate(
          sex_type::female, 2017, 12, 25);

      cout << "snorth1: " << result << endl;
    }
  }

  {
    auto ssouth1 = factory.get_generator("south");
    std::string result;
    if (ssouth1 != nullptr)
    {
      result = ssouth1->generate(
          sex_type::female, 2017, 12, 25);

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
      virtual double approval_limit() const noexcept override
      {
        return 1000;
      }
  };

  class team_manager_role : public role
  { 
    public:
      virtual double approval_limit() const noexcept override
      {
        return 10000;
      }
  };

  class dept_manager_role : public role
  {
    public:
      virtual double approval_limit() const noexcept override
      {
        return 100000;
      }
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
      : amount_(amount), desc_(desc)
    {}
  };

  class employee
  {
    public:
      explicit employee(std::string name, std::unique_ptr<role> role)
        : name_(name), role_(std::move(role)) 
      {}

      void set_direct_manager(std::shared_ptr<employee> manager)
      {
        manager_ = manager;
      }

      void approve(expense const& e)
      {
        if (e.amount_ <= role_->approval_limit())
          std::cout << name_ << " approved expense '" << e.desc_ << "', cost= "
            << e.amount_ << std::endl;
        else if (manager_ != nullptr)
          manager_->approve(e);
      }

    private:
      std::string name_;
      std::unique_ptr<role> role_;
      std::shared_ptr<employee> manager_;
  };

} // namespace

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
  auto john = std::make_shared<employee>("john smith",
      std::unique_ptr<employee_role>(new employee_role()));

  // team manager
  auto robert = std::make_shared<employee>("robert booth",
      std::unique_ptr<team_manager_role>(new team_manager_role()));

  // dept manager
  auto david = std::make_shared<employee>("david jones",
      std::unique_ptr<dept_manager_role>(new dept_manager_role()));

  // president 
  auto cecil = std::make_shared<employee>("cecil williamson",
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
    for (std::vector<BluetoothA2DPTransportListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it) if (*it) (*it)->_what; \
} while(0)

  {
    notify(onBufferReady(this, buffer, nbytes, sampleno));
  }
};


{
  observable_vector<int> ov;
  observer o;
  ov.add_observer(&o);
}

If there is no need to call remove_observer() or removeListener(), then no need
to hold reference to source(notifier). For example, only add interest and use it
until application finishes. Or, use of expired weak_ptr.

*/


namespace cxx_pattern_observer
{
#if 0
  // this is code from the text which works but not sure why this is complicated
  // as this.

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
#endif
} // namespace

namespace cxx_pattern_observer
{
  enum class changed_action {add, remove, clear, assign};

  std::string action_name(changed_action const action)
  {
    switch(action)
    {
      case changed_action::add: return "added";
      case changed_action::remove: return "removed";
      case changed_action::clear: return "cleared";
      case changed_action::assign: return "assigned";
      default: return "unsupported";
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

  template<typename T>
    class observable_vector
    {
      using size_type = typename std::vector<T>::size_type;

      public: 
        explicit observable_vector() 
          : data_(), observers_() 
        {}

        explicit observable_vector(std::initializer_list<T> init) 
          : data_(init)
        {}

        // copy assign
        // do not update(overwrite) `observers_` from rhs so keep current
        // observers

        observable_vector& operator=(observable_vector const &rhs)
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
        observable_vector& operator=(observable_vector &&rhs)
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

        size_type size() const noexcept
        { return data_.size(); }

        bool empty() const noexcept
        { return data_.empty(); }


        // observer interfaces
        void add_observer(collection_observer *o)
        { observers_.push_back(o); }

        void remove_observer(collection_observer const *o)
        {
          // this assumes that there are duplicated observers
          observers_.erase(
              std::remove(std::begin(observers_), std::end(observers_), o), // begin
              observers_.end() // end
              );
        }

      private:
        std::vector<T> data_;
        std::vector<collection_observer *> observers_;
    };
} // namespace


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

  observable_vector<int> ovv{1,2,3};

  // copy assign
  cout << "=========" << endl;
  ov = ovv;

  // move assign
  cout << "=========" << endl;
  ov = observable_vector<int>{7,8,9};
  ov.push_back(1);
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

    virtual double 
      discount_percent(double price, double quantity) const noexcept = 0;
  };

  struct fixed_discount final : public discount_type
  {
    explicit fixed_discount(double discount) noexcept
      : discount_(discount) 
      {}

    virtual double
      discount_percent(double, double) const noexcept override
      { return discount_; }

    private:
      double discount_;
  };

  struct volume_discount final : public discount_type
  {
    explicit volume_discount(double quantity, double discount) noexcept
      : discount_(discount), min_quantity_(quantity)
      {}

    virtual double
      discount_percent(double, double quantity) const noexcept override
      { return quantity >= min_quantity_ ? discount_ : 0; }

    private:
      double discount_;
      double min_quantity_;
  };

  // discount on total price
  struct price_discount final : public discount_type
  {
    explicit price_discount(double price, double discount) noexcept
      : discount_(discount), min_total_price_(price)
      {}

    virtual double
      discount_percent(double price, double quantity) const noexcept override
      { return (price * quantity) >= min_total_price_ ? discount_ : 0; }

    private:
      double discount_;
      double min_total_price_;
  };

  // discount on item price
  struct amount_discount final : public discount_type
  {
    explicit amount_discount(double price, double discount) noexcept
      : discount_(discount), min_total_price_(price)
      {}

    virtual double
      discount_percent(double price, double) const noexcept override
      { return price >= min_total_price_ ? discount_ : 0; }

    private:
      double discount_;
      double min_total_price_;
  };


  // user(client) class

  struct customer
  {
    std::string name_;
    discount_type* discount_;
  };

  enum class article_unit
  { piece, kg, meter, sqmeter, cmeter, liter };

  struct article
  {
    int             id_;
    std::string     name_;
    double          price_;
    article_unit    unit_;
    discount_type*  discount_;
  };

  struct order_line
  {
    article         product_;
    int             quantity_;
    discount_type*  discount_;
  };

  struct order
  {
    int                     id_;
    customer*               buyer_;
    std::vector<order_line> lines_;
    discount_type*          discount_;
  };

  struct price_calculator
  {
    virtual double calculate_price(order const& o) = 0;
  };

  struct cumulative_price_calculator : public price_calculator
  {
    virtual double calculate_price(order const& o) override
    {
      double price = 0;

      // each item ordered
      for (auto ol : o.lines_)
      {
        double line_price = ol.product_.price_ * ol.quantity_;

        // product discount
        if (ol.product_.discount_ != nullptr)
        {
          line_price *= (1.0 - ol.product_.discount_->discount_percent(
                ol.product_.price_, ol.quantity_));
        }

        // order_line discount
        if (ol.discount_ != nullptr)
        {
          line_price *= (1.0 - ol.discount_->discount_percent(
                ol.product_.price_, ol.quantity_));
        }

        // buyer discount
        if (o.buyer_ != nullptr && o.buyer_->discount_ != nullptr)
        {
          line_price *= (1.0 - o.buyer_->discount_->discount_percent(
                ol.product_.price_, ol.quantity_));
        }

        price += line_price;
      } // end for

      // whole order discount
      if (o.discount_ != nullptr)
        price *= (1.0 - o.discount_->discount_percent(price, 0));

      return price;
    };
  };

} // namespace

TEST(PatternStrategy, ComputingOrderPriceWithDiscounts)
{
  using namespace cxx_pattern_strategy;
  
  fixed_discount fd(0.1);   // 10%
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

cxx_pattern_dispatcher

*/

namespace cxx_pattern_dispatcher
{
  // Polymorphic.h
  //
  // original comments:
  // Inherit from this from all types that have virtual functions. Doing so
  // ensures that you have virtual destructor and saves you nasty surprises.

  class Polymorphic
  {
    public:
      virtual ~Polymorphic() {};
  };

  // IDispatcher.h

  class IDispatcher : public Polymorphic
  {
    public:
      // post an work item to be executed
      virtual void post(std::function<void ()>) = 0;

      // ensures that any works that was in the queue before the call has been
      // executed
      virtual void sync() = 0;

      // check if it's called from the dispatch thread
      virtual bool invoked_from_this() = 0;
  };

  // ThreadedDispatcher.h
  //
  // (see) that use `public keyword` to interfaces from parent and ones from this
  // class.

  class ThreadedDispatcher : public IDispatcher
  {
    public: // IDispatcher

      // post an work item to be executed
      virtual void post(std::function<void ()>) final;

      // ensures that any works that was in the queue before the call has been
      // executed
      virtual void sync() final;

      // true when calling thread is the  dispatcher
      virtual bool invoked_from_this() final;

    public: // this class

      ThreadedDispatcher(std::string const &name = std::string());

      // create dispatcher with supplied SCHED_RR priority and name.
      ThreadedDispatcher(int priority, std::string const &name = std::string());

      ~ThreadedDispatcher();

      // Perform any work remaining in the queue, then stop accepting new work.
      void flush();

      // stop accepting new work and stop dispatcher even if there are works in
      // the queue.
      void stop();

    private:
      std::mutex _m;
      std::condition_variable _cv;
      bool _running;
      std::deque<std::function<void()>> _q;
      std::thread _t;

      // returns next work item to run. this function assumes that there is work
      // to be done and that the mutex is acquired

      std::function<void()> _next();

      // entry point of the dispatcher 
      void _do_work(std::string const &name, int priority);
  };

  ThreadedDispatcher::ThreadedDispatcher(std::string const &name)
    : ThreadedDispatcher(-1, name)
  {}

  // cxx_pattern.cpp:1645:25: error: ISO C++ forbids taking the address of an
  // unqualified or parenthesized non-static member function to form a pointer
  // to member function. 
  // Say ‘&cxx_pattern_dispatcher::ThreadedDispatcher::_do_work’ [-fpermissive] ,
  //
  // _t(std::thread(&_do_work, this, name, priority))
 
  ThreadedDispatcher::ThreadedDispatcher(int priority, std::string const &name)
    : _running(true) 
      , _t(std::thread(&ThreadedDispatcher::_do_work, this, name, priority))
  {}

  ThreadedDispatcher::~ThreadedDispatcher()
  {
    if (_running)
    {
      stop();
    }
  }

  void ThreadedDispatcher::post(std::function<void()> work)
  {
    // (see) this is original. does it make difference?
    //
    // std::unique_lock<std::mutex> lock(m);
    // if(running)
    // {
    //     q.push_back(work);
    //     lock.unlock();
    //     cv.notify_one();
    // }

    std::lock_guard<std::mutex> lock(_m);

    if (_running)
    {
      _q.push_back(work);
      _cv.notify_one();
    }
    else
    {
      std::cout << "Ignoring work because the dispatcher is not running anymore" << std::endl;

      // original comment:
      // LOG_WARN("Ignoring work because the dispatcher is not running anymore");
      // can't throw an exception here because if this is executed from destructor,
      // which occurs when work adds more work things go horribly wrong.
      // Instead, ignore work.
    }
  }

  bool ThreadedDispatcher::invoked_from_this()
  {
    bool res = (std::this_thread::get_id() == _t.get_id());
    return res;
  }

  void ThreadedDispatcher::stop()
  {
    // (see) have to use unique_lock() since it uses thread::join()
    
    std::unique_lock<std::mutex> lock(_m);
    _running = false;
    _cv.notify_one();
    lock.unlock();
    _t.join();
    // std::cout << "stop: q size is " << _q.size() << std::endl;
  }

  std::function<void()> ThreadedDispatcher::_next()
  {
    // (see) std::move() has an effect here?
    auto work = std::move(_q.front());
    _q.pop_front();
    return work;
  }

  void ThreadedDispatcher::_do_work(std::string const &name, int priority)
  {
    std::unique_lock<std::mutex> lock(_m);

    // std::cout << "td is created" << std::endl;

    while (_running)
    {
      // (see)
      // exit wait when _running is false? which means when it's requested to
      // stop? 
      //
      // so when requested to stop, run one work if there is even if it is
      // possible to have many works in the queue.
      //
      // the original code is:
      // cv.wait(lock, bind(&This::hasMoreWorkOrWasStopRequested, this));

      // cxx_pattern.cpp:1716:27: error: ‘this’ was not captured for this lambda function
      //       _cv.wait(lock, []{ !_q.empty() || !_running; });
 
      _cv.wait(lock, [this]{ return !_q.empty() || !_running; });
      if (!_q.empty())
      {
        std::function<void()> work = _next();

        // (see) don't block adding work to the queue such as post() while
        // dispatcher does the work

        lock.unlock();
        // std::cout << "td do work" << std::endl;
        work();
        lock.lock();
      }
    }
  }

  namespace
  {
    void unlockAndSetFlagToFalse(std::mutex& m, bool& flag)
    {
      std::this_thread::sleep_for(std::chrono::seconds(5));
      // std::cout << "flush thread: waits ends" << std::endl;
      m.unlock();

      // (see)
      // original code. without setting flag, still works
      // flag = false;
      //
      // using namespace std;
      // m.unlock();
      // flag = false;
    }
  }

  // (see)
  // calling thread post a work and wait the same lock. (this post a work which unlock a lock. 

  void ThreadedDispatcher::flush()
  {
    // To ensure all the work that is in the queue is done, we lock a mutex.
    // post a job to the queue that unlocks it and stops running further jobs.
    // Then block here until that's done.

    if(_running)
    {
      std::mutex m2;
      m2.lock();
      post(bind(unlockAndSetFlagToFalse, std::ref(m2), std::ref(this->_running)));
      // std::cout << "flush calling thread: locked again" << std::endl;
      m2.lock();
      // std::cout << "flush calling thread: unlocked" << std::endl;
      m2.unlock();
      stop();
    }
    else
    {
      // AI_LOG_WARN("This dispatcher is no longer running. Ignoring flush request.");
    }
  }


  // (see) same as flush() which stops() and this is difference from sync().
  
  /**
   *  @brief Ensures that any items in the dispatch queue before this call are
   *  processed before the function returns.
   *
   *  The function blocks until everything in the queue prior to the call is
   *  processed.
   *
   *  It works by putting a dummy work item on the queue which takes a reference
   *  to a local conditional variable, we then wait on the conditional triggering.
   *
   */
  namespace
  {
    // -----------------------------------------------------------------------------
    /**
     *  @brief Work item callback for the sync method.
     *
     *  This function is put on the queue by the sync method, it simply sets a
     *  boolean flag and notifies the conditional variable.
     *
     *  @param[in]  lock    The mutex lock to hold when setting the fired param
     *  @param[in]  cond    The conditional variable to use to wake up the caller
     *  @param[in]  fired   Reference to a boolean variable to set to true
     *
     */
    void syncCallback(std::mutex* lock, std::condition_variable* cond, bool* fired)
    {
      std::unique_lock<std::mutex> locker(*lock);
      *fired = true;
      cond->notify_all();
      locker.unlock();
    }
  } // namespace

  void ThreadedDispatcher::sync()
  {
    std::mutex lock;
    std::condition_variable cond;
    bool fired = false;

    // Take the queue lock and ensure we're still running
    std::unique_lock<std::mutex> qlocker(_m);
    if (!_running)
    {
      // AI_LOG_DEBUG("Ignoring sync because dispatcher is not running");
      return;
    }

    // Add the work object to the queue which takes the lock and sets 'fired' to true
    _q.push_back(std::bind(syncCallback, &lock, &cond, &fired));
    qlocker.unlock();
    _cv.notify_one();

    // Wait for 'fired' to become true
    std::unique_lock<std::mutex> locker(lock);
    while (!fired)
    {
      cond.wait(locker);
    }
  }
} // namespace

namespace cxx_pattern_dispatcher
{
  void assign1(bool &what, bool value)
  {
    // std::cout << "assign1 is called, what: " << what << std::endl;
    what = value;
  }

  void assign2(bool &what, bool value)
  {
    // std::cout << "assign2 is called, what: " << what << std::endl;
    what = value;
  }
} // namespace

TEST(PatternDispatcher, PostedWorkIsDone)
{
  using namespace cxx_pattern_dispatcher;

  // use sleep_for() to give dispatcher chance to run
  {
    bool isDone{false};
    shared_ptr<ThreadedDispatcher> td = make_shared<ThreadedDispatcher>();
    td->post(std::bind(assign1, std::ref(isDone), true));
    // td->flush();

    std::this_thread::sleep_for(chrono::seconds(1));

    EXPECT_THAT(isDone, true);
  }

  // use flush() instead
  {
    bool isDone{false};
    shared_ptr<ThreadedDispatcher> td = make_shared<ThreadedDispatcher>();
    td->post(std::bind(assign1, std::ref(isDone), true));
    td->flush();

    EXPECT_THAT(isDone, true);
  }

  // to see second work
  {
    bool isDone{false};
    shared_ptr<ThreadedDispatcher> td = make_shared<ThreadedDispatcher>();
    td->post(std::bind(assign1, std::ref(isDone), true));
    td->post(std::bind(assign2, std::ref(isDone), false));

    std::this_thread::sleep_for(chrono::seconds(1));

    EXPECT_THAT(isDone, false);
  }

}


// [ RUN      ] PatternDispatcher.Flush
// flush calling thread: locked again
// assign_wait is called, what: 0
// assign_wait is called, what: 1
// flush thread: waits ends
// flush calling thread: unlocked
// [       OK ] PatternDispatcher.Flush (9001 ms)

namespace cxx_pattern_dispatcher
{
  void assign_wait(bool &what, bool value)
  {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "assign_wait is called, what: " << what << std::endl;
    what = value;
  }
} // namespace

TEST(PatternDispatcher, Flush)
{
  using namespace cxx_pattern_dispatcher;

  // use flush() instead
  {
    bool isDone{false};
    shared_ptr<ThreadedDispatcher> td = make_shared<ThreadedDispatcher>();
    td->post(std::bind(assign_wait, std::ref(isDone), true));
    td->post(std::bind(assign_wait, std::ref(isDone), false));
    td->flush();

    EXPECT_THAT(isDone, false);
  }
}


namespace cxx_pattern_dispatcher
{
  // dispatcher runs this so std::this_thread::get_id() is dispatcher.
  void check_thread_id(thread::id test)
  {
    EXPECT_NE(std::this_thread::get_id(), test);
  }
} // namesapce


// expect that gtest thread is different from dispatcher

TEST(PatternDispatcher, PostedWorkIsDoneOnDispatcher)
{
  using namespace cxx_pattern_dispatcher;

  {
    shared_ptr<ThreadedDispatcher> td = make_shared<ThreadedDispatcher>();
    td->post(std::bind(check_thread_id, std::this_thread::get_id()));

    std::this_thread::sleep_for(chrono::seconds(1));
  }
}


namespace cxx_pattern_dispatcher
{
  void save_sequence(int &value)
  {
    static int sequence = 0;
    value = ++sequence;
  }
} // namesapce

// expect that works will be done in the order that they are posted.

TEST(PatternDispatcher, PostedWorkIsDoneInOrder)
{
  using namespace cxx_pattern_dispatcher;

  {
    int first{0};
    int second{0};

    shared_ptr<ThreadedDispatcher> td = make_shared<ThreadedDispatcher>();
    td->post(std::bind(save_sequence, std::ref(first)));
    td->post(std::bind(save_sequence, std::ref(second)));

    std::this_thread::sleep_for(chrono::seconds(1));

    EXPECT_THAT(first, 1);
    EXPECT_THAT(second, 2);
    EXPECT_LT(first, second);
  }
}

// may stop() cause deadlock? why?

TEST(PatternDispatcher, StopDoNotCanuseDeadlock)
{
  using namespace cxx_pattern_dispatcher;

  {
    int first{0};
    int second{0};

    shared_ptr<ThreadedDispatcher> td = make_shared<ThreadedDispatcher>();
    td->post(std::bind(save_sequence, std::ref(first)));
    td->post(std::bind(save_sequence, std::ref(second)));
    td->stop();
  }
}

namespace cxx_pattern_dispatcher
{
  void increments(int &value)
  {
    ++value;
    std::this_thread::sleep_for(chrono::milliseconds(10));
  }
} // namesapce

// expect that all works will be done without one missed.

TEST(DISABLED_PatternDispatcher, DoLotsOfWorks)
{
  using namespace cxx_pattern_dispatcher;

  {
    int value{0};
    const int count{100000};
    shared_ptr<ThreadedDispatcher> td = make_shared<ThreadedDispatcher>();

    for (int i = 0; i < count; ++i)
    {
      td->post(std::bind(increments, std::ref(value)));
    }

    td->flush();

    EXPECT_THAT(value, count);
  }
}


namespace cxx_pattern_dispatcher
{
  void notify_cv(std::mutex &m, std::condition_variable& cv)
  {
    std::lock_guard<std::mutex> lock(m);
    cv.notify_one();
  }
} // namesapce

// what is it going to test for?

TEST(PatternDispatcher, AddMoreWork)
{
  using namespace cxx_pattern_dispatcher;

  {
    auto td = std::make_shared<ThreadedDispatcher>();
    std::mutex m;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(m);

    td->post(std::bind(notify_cv, std::ref(m), std::ref(cv)));

    EXPECT_THAT(cv.wait_for(lock, std::chrono::seconds(5)), std::cv_status::no_timeout);

    td->flush();
  }
}

namespace
{
  void sleepy_increments(int &value)
  {
    ++value;
    std::this_thread::sleep_for(chrono::milliseconds(10));
  }
} // namesapce


// Hang sometines???

TEST(PatternDispatcher, Sync)
{
  using namespace cxx_pattern_dispatcher;

  {
    int value{0};
    const int count{100000};
    auto td = std::make_shared<ThreadedDispatcher>();

    for (int i = 0; i < count; ++i)
    {
      td->post(std::bind(sleepy_increments, std::ref(value)));
    }

    // td->flush();
    td->sync();

    EXPECT_THAT(value, count);
  }
}


/*
={=============================================================================

cxx_pattern_dispatcher cxx_pattern_observer

observer which use dispatcher

from appinfrastructure/AppInfrastructure/Public/Common/Notifier.h
 
*/

namespace cxx_pattern_dispatcher
{
  // A template of observable objects that send notifications defined in
  // interface T
  //
  // Inherit to use, call notify() to send an update

  template<typename T>
    class Notifier : virtual public Polymorphic
  {
    public:
      Notifier()
      {}

      // register
      void add_observer(std::shared_ptr<T> const &o)
      {
        std::lock_guard<std::mutex> lock(m_);
        observers_.push_back(o);
      }

      void remove_observer(std::shared_ptr<T> const &o)
      {
        std::unique_lock<std::mutex> lock(m_);

#if (_BUILD_TYPE == _DEBUG)
        // less likely  checks
        if (dispatcher_ && dispatcher_->invoked_from_this())
        {
          throw std::logic_error("potential deadlock since this should not be called from dispatcher");
        }
#endif

        for (size_t i = 0; i < observers_.size(); ++i)
        {
          // if stored strong pointer is still valid which means object is still
          // around ans it's safe to use
          if (observers_[i].lock() == o)
          {
            // since erase() needs iterator
            observers_.erase(observers_.begin() + i);
            break;
          }
        }

        // when observer calls this to remove itself, notifier is running so
        // cannot remove it now. hold them for now until notifier finishes.

        if (notifying_)
        {
          waitee_count_++;

          do
          {
            // cxx-condition-variable-wait
            // wait()
            // blocks the current thread until the condition variable is woken up 
            cv_.wait(lock);
          } while (notifying_);

          waitee_count_--;
        } 
      }

      void set_dispatcher(std::shared_ptr<IDispatcher> const & d)
      { 
        std::lock_guard<std::mutex> lock(m_);
        dispatcher_ = d;
      }

    protected:

      template<typename F, typename... Args>
        void notify(F f, Args&&... args)
        {
          notify(std::bind(f, std::placeholders::_1, std::forward<Args>(args)...));
        }

      template<typename F>
        void notify(F f)
        {
          notify_impl(f);
        }

    private:
      std::shared_ptr<IDispatcher> dispatcher_;
      std::mutex m_;
      std::deque<std::weak_ptr<T>> observers_;
      std::condition_variable cv_;
      bool notifying_{false};
      uint32_t waitee_count_{0};

      void notify_impl(std::function<void (std::shared_ptr<T> const &)> fun)
      {
        std::unique_lock<std::mutex> lock(m_);

        if (!dispatcher_)
        {
          throw std::logic_error("you must set a dispatcher before you produce events.");
        }

        // don't want to lock adding new observers while callbacks are executed.
        // so make a copy instead.
        //
        // in the unlikely event that there are expired observers, remove
        // expired observers by copying only if use_count() > 0.

        decltype(observers_) observers_copy;
        std::copy_if(observers_.begin(), observers_.end(),
            std::back_inserter(observers_copy), 
            std::bind(&std::weak_ptr<T>::use_count, std::placeholders::_1));

        if (observers_copy.size() != observers_.size())
          observers_ = observers_copy;

        // okay, start notifying
        notifying_ = true;
        lock.unlock();

        //----------------- NOTE ----------------------------------------------
        // We maintain vector of strong pointers pointing to observer objects as
        // otherwise bad things can happen. Lets consider, the observer object
        // point backs to the notifier object itself.  That means, there is a
        // circular dependency between the notifier and the observer, but we
        // break that by using a combination of shared and weak pointers.
        // However, imagine, within the notify_impl() method, we gets a shared
        // pointer of observer object out of weak_ptr. After the shared pointer
        // is constructed (bit still in use), now the owner of the observer
        // resets its pointer that is pointing to the observer object. This
        // might result one to one references between the notifier and the
        // observer, i.e., as soon as the observer will be destroyed the
        // notifier will also be destroyed. It means, if now the observer object
        // is destroyed from the notify_imp() method, it will cause the notifier
        // object itself to be destroyed, where the notify_impl can still
        // continue to access its member variable (e.g. dispatcher). This might
        // result an undefined behaviour.
        //---------------------------------------------------------------------

        std::vector<std::shared_ptr<T>> observers_strong;

        for (auto const &o : observers_copy)
        {
          std::shared_ptr<T> strong = o.lock();
          if (strong)
          {
            dispatcher_->post(std::bind(fun, strong));
          }

          observers_strong.push_back(strong);
        }

        // okay, finish notifying
        lock.lock();

        // about to unregister an observer so make sure that there is no work
        // for this observer after that.
        if (dispatcher_ && (waitee_count_ > 0))
        {
          lock.unlock();
          dispatcher_->sync();
          lock.lock();
        }

        notifying_ = false;

        // okay, notify all waited observers
        if (waitee_count_ > 0)
          cv_.notify_all();

        lock.unlock();
      }
  };
} // namesapce

namespace cxx_pattern_dispatcher
{
  using namespace std;
  using namespace std::placeholders;

  /**
   * @brief A template for observing objects that accept signals defined in T.
   *
   * @note Arguably you could inherit directly from T, however inheriting from
   *       Observer<T> is more intention revealing than inheriting from T.
   *       There is no extra overhead because of Empty Base Class Optimisation.
   */
  template <class T>
    class Observer : public T, virtual public Polymorphic
  {
  };

  // a basic callback interface with two types of callbacks
  class StateEvents
  {
    public:
      virtual void stateChanged(int newState) = 0;
      virtual void nameChanged(std::string newName) = 0;
      virtual void keyAndValueChanged(std::string newKey, std::string newValue) = 0;
      virtual void eventOccured() = 0;
  };

  class Observee : public Notifier<StateEvents>
  {
    public:
      void setState(int state)
      {
        notify(std::bind(&StateEvents::stateChanged, _1, state));
      }

      void setName(std::string name)
      {
        notify(&StateEvents::nameChanged, name);
      }

      void setKeyAndValue(std::string key, std::string value)
      {
        notify(&StateEvents::keyAndValueChanged, key, value);
      }

      void generateEvent()
      {
        notify(&StateEvents::eventOccured);
      }
  };

  class TestObserver : public Observer<StateEvents>
  {
    public:
      MOCK_METHOD1(stateChanged, void (int));
      MOCK_METHOD1(nameChanged, void (std::string));
      MOCK_METHOD2(keyAndValueChanged, void (std::string, std::string));
      MOCK_METHOD0(eventOccured, void());
  };


  /**
   * @brief A dispatcher that does all the work immediately on the thread that
   * calls post.
   */
  class CallerThreadedDispatcher : public IDispatcher
  {
    public:
      virtual void post(std::function<void ()> work) final
      { work(); }

      virtual void sync() final 
      {}

      virtual bool invoked_from_this() final
      { return false; }
  };

} // namespace


TEST(PatternDispatcherAndObserver, SendNotificationWithRealDispatcher)
{
  using namespace cxx_pattern_dispatcher;

  Observee noti;
  std::shared_ptr<TestObserver> observer = std::make_shared<TestObserver>();
  noti.set_dispatcher(std::make_shared<ThreadedDispatcher>());
  noti.add_observer(observer);

  EXPECT_CALL(*observer, stateChanged(5)).Times(1);

  noti.setState(5);
}

// can use real dispatcher as above but since not control dispatcher such as
// sync(), flush() or wait it to finish, cannot have deterministic result. Hence
// CallerThreadedDispatcher which call a work on calling thread.

TEST(PatternDispatcherAndObserver, SendNotificationWithCallerDispatcher)
{
  using namespace cxx_pattern_dispatcher;

  Observee noti;
  std::shared_ptr<TestObserver> observer = std::make_shared<TestObserver>();
  noti.set_dispatcher(std::make_shared<CallerThreadedDispatcher>());
  noti.add_observer(observer);

  EXPECT_CALL(*observer, stateChanged(5)).Times(1);

  noti.setState(5);
}

TEST(PatternDispatcherAndObserver, SendNotificationManyArgs)
{
  using namespace cxx_pattern_dispatcher;

  Observee noti;
  std::shared_ptr<TestObserver> observer = std::make_shared<TestObserver>();
  noti.set_dispatcher(std::make_shared<CallerThreadedDispatcher>());
  noti.add_observer(observer);

  EXPECT_CALL(*observer, keyAndValueChanged("key", "value")).Times(1);

  noti.setKeyAndValue("key", "value");
}


// ={=========================================================================
int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

