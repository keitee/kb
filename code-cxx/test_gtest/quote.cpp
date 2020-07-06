/*

copied from "code-cxx/cxx/test_cxx_case.cpp"

={=============================================================================
CXXPP, Quote

1. use size(count) and head only
2. in push, no full check since it overwrites and in pop, it simply
calculates first from head substracing size.
3. no iterator support is needed if not use begin()/end()

*/

// when introduce `discount` concept

namespace case_quote
{
  class Quote
  {
  private:
    std::string book_no_{};

  protected:
    double price_;

  public:
    Quote()
        : book_no_()
        , price_(0.0)
    {}

    Quote(string const &book, double sale_price)
        : book_no_(book)
        , price_(sale_price)
    {}

    virtual ~Quote() {}

    std::string isbn() const { return book_no_; }

    // calculate net price but do not have "discount" concept
    virtual double net_price(size_t count) const { return count * price_; }
  };

  class Discount_Quote : public Quote
  {
  protected:
    size_t quantity_;
    double discount_;

  public:
    // *cxx-ctor*
    // Why need to have constructors in abstract class although cannot define
    // objects of this type directly? Becuase ctors in classes derived from
    // Discount_quote will use the Discount_quote ctor to construct the
    // Disc_quote part of their objects. Default ctor default initialize those
    // members.

    Discount_Quote()
        : quantity_(0)
        , discount_(0.0)
    {}

    Discount_Quote(string const &book,
                   double price,
                   size_t quantity,
                   double discount_percent)
        : Quote(book, price)
        , quantity_(quantity)
        , discount_(discount_percent)
    {}

    // *cxx-dtor*
    // no need to have virtual dtor here since Quote has it already

    // *cxx-abc*
    // okay to have in the middle of inheritance and "discount" concept
    virtual double net_price(size_t count) const = 0;
  };

  class Bulk_Quote : public Discount_Quote
  {
  public:
    Bulk_Quote(string const &book,
               double price,
               size_t quantity,
               double discount_percent)
        : Discount_Quote(book, price, quantity, discount_percent)
    {}

    // cxx-override cxx-const
    // const is one of override conditions

    double net_price(size_t count) const override
    {
      if (count >= quantity_)
        return count * (1 - discount_) * price_;
      else
        return count * price_;
    }
  };

  double print_total(ostream &os, Quote const &item, size_t sold)
  {
    double net_price = item.net_price(sold);

    os << "isbn: " << item.isbn() << ", sold: " << sold
       << ", total due: " << net_price << endl;

    return net_price;
  }
} // namespace case_quote

#if 0
when use it directly than using *gtest-fixture*:

class Basket
{
  public:
    Basket() : items(compare) {}

    void add_item(const shared_ptr<Quote> &item);

    double total_receipt(ostream &os) const;

  private:
    static bool compare(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs) 
    { 
      return lhs->isbn() < rhs->isbn(); 
    }

    // using comp = bool (*)(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs);
    // multiset<shared_ptr<Quote>, comp> items;

    using comp = bool (const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs); 

    multiset<shared_ptr<Quote>, comp*> items;
};

void Basket::add_item(const shared_ptr<Quote> &item)
{
  cout << "basket::add_item::copy version" << endl;
  items.insert(item);
}

double Basket::total_receipt(ostream &os) const
{
  for (auto iter = items.cbegin(); 
      iter != items.cend();
      iter = items.upper_bound(*iter))
  {
    os << "isbn : " << (*iter)->isbn()
      << ", sold : " << items.count(*iter)
      << ", total sales: " << (*iter)->net_price( items.count(*iter))
      << endl;
  }
}

int main()
{
   Basket sale;

   // Quote sales which has no discount. 45*3 = 135
   sale.add_item(shared_ptr<Quote>(new Quote("123", 45)));
   sale.add_item(shared_ptr<Quote>(new Quote("123", 45)));
   sale.add_item(make_shared<Quote>("123", 45));

   // minimum 3 and 15% discount. no discount 45*2 = 90
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));

   // Bulk_quote sales which has discount: minimum 3 and 15% discount
   // 35*4*(1-.15) = 119
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));

   // Bulk_quote sales which has discount: minimum 5 and 25% discount
   // 35*6*(1-.25) = 157.5
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
   sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));

   sale.display(cout);
   sale.total_receipt(cout);
}
#endif

// *gmock-error*
// must use different name on fixture and other tests
//
// [ RUN      ] CxxCaseQuote.MockQuote
// /home/kyoupark/works/googletest/googletest/src/gtest.cc:2300: Failure
// Failed
// All tests in the same test case must use the same test fixture
// class, so mixing TEST_F and TEST in the same test case is
// illegal.  In test case CxxCaseQuote,
// test CheckTotal_1 is defined using TEST_F but
// test MockQuote is defined using TEST.  You probably
// want to change the TEST to TEST_F or move it to another test
// case.
// [  FAILED  ] CxxCaseQuote.MockQuote (3 ms)

// use fixture and real quotes

class CxxCaseQuoteX : public ::testing::Test
{
protected:
  // *cxx-using* is under *cxx-access-control*

  using Quote = case_quote::Quote;

  CxxCaseQuoteX()
      : items_{compare}
  {}

  virtual ~CxxCaseQuoteX() {}

  virtual void SetUp() override {}
  virtual void TearDown() override {}

  double total_receipt() const
  {
    double total{};

    for (auto it = items_.cbegin(); it != items_.cend();
         it      = items_.upper_bound(*it))
    {
      // when there are duplicates, skip them since net_price() gets called for
      // them in single call with count(num of books)

      total = (*it)->net_price(items_.count(*it));

      cout << "isbn: " << (*it)->isbn() << ", sold: " << items_.count(*it)
           << ", total sales: " << total << endl;
    }

    return total;
  }

  // why not const &?
  // static bool compare(const std::shared_ptr<Quote> lhs,
  //                     const std::shared_ptr<Quote> rhs)
  // {
  //   return lhs->isbn() < rhs->isbn();
  // }

  static bool compare(const std::shared_ptr<Quote> &lhs,
                      const std::shared_ptr<Quote> &rhs)
  {
    return lhs->isbn() < rhs->isbn();
  }

  using comp = bool(const std::shared_ptr<Quote> &lhs,
                    const std::shared_ptr<Quote> &rhs);

  // NOTE: unique_ptr??
  std::multiset<std::shared_ptr<Quote>, comp *> items_;
};

TEST_F(CxxCaseQuoteX, CheckTotal_1)
{
  // WHY no error without this???
  // using namespace case_quote;

  // Quote("book no", price)
  items_.insert(std::shared_ptr<Quote>(new Quote("123", 45)));
  items_.insert(std::shared_ptr<Quote>(new Quote("123", 45)));
  items_.insert(std::shared_ptr<Quote>(new Quote("123", 45)));

  // Quote sales which has no discount. 45*3 = 135
  EXPECT_THAT(total_receipt(), 135);

  items_.clear();
}

TEST_F(CxxCaseQuoteX, CheckTotal_2)
{
  using namespace case_quote;

  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("345", 45, 3, .15)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("345", 45, 3, .15)));

  // minimum 3 to have 15% discount. 2 books so no discount 45*2 = 90
  EXPECT_THAT(total_receipt(), 90);

  items_.clear();
}

TEST_F(CxxCaseQuoteX, CheckTotal_3)
{
  using namespace case_quote;

  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("678", 35, 3, .15)));

  // Bulk_quote sales which has discount: minimum 3 and 15% discount
  // 35*4*(1-.15) = 119
  EXPECT_EQ(total_receipt(), 119);

  items_.clear();
}

TEST_F(CxxCaseQuoteX, CheckTotal_4)
{
  using namespace case_quote;

  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));
  items_.insert(std::shared_ptr<Quote>(new Bulk_Quote("912", 35, 5, .25)));

  // Bulk_quote sales which has discount: minimum 5 and 25% discount
  // 35*6*(1-.25) = 157.5
  EXPECT_EQ(157.5, total_receipt());

  items_.clear();
}

// To test user(driver class), Basket. not use fixture and mock out quotes.

namespace case_quote
{
  class MockQuote : public Quote
  {
  public:
    MockQuote(string const &book, double sale_price)
        : Quote(book, sale_price)
    {}

    MOCK_CONST_METHOD1(net_price, double(size_t count));
  };

  class MockBulk : public Bulk_Quote
  {
  public:
    MockBulk(string const &book,
             double price,
             size_t quantity,
             double discount_percent)
        : Bulk_Quote(book, price, quantity, discount_percent)
    {}

    MOCK_CONST_METHOD1(net_price, double(size_t count));
  };

  class Basket
  {
  public:
    // *cxx-undefined*
    // Without it, cause core when add_item is called.
    Basket()
        : items_{compare}
    {}

    void add_item(const std::shared_ptr<Quote> &item)
    {
      cout << "basket::add_item(shard_ptr)" << endl;
      items_.insert(item);
    }

    double total_receipt(ostream &os) const
    {
      double total{};

      for (auto iter = items_.cbegin(); iter != items_.cend();
           iter      = items_.upper_bound(*iter))
      {
        total = (*iter)->net_price(items_.count(*iter));
        os << "isbn: " << (*iter)->isbn() << ", sold: " << items_.count(*iter)
           << ", total sales: " << total << endl;
      }

      return total;
    }

  private:
    static bool compare(const std::shared_ptr<Quote> lhs,
                        const std::shared_ptr<Quote> rhs)
    {
      return lhs->isbn() < rhs->isbn();
    }

    using comp = bool(const std::shared_ptr<Quote>,
                      const std::shared_ptr<Quote>);

    std::multiset<std::shared_ptr<Quote>, comp *> items_;
  };

} // namespace case_quote

TEST(CxxCaseQuote, MockQuote)
{
  using namespace case_quote;

  // Quote sales which has no discount, 45*3 = 135
  std::shared_ptr<MockQuote> q1(new MockQuote("123", 45));
  std::shared_ptr<MockQuote> q2(new MockQuote("123", 45));
  std::shared_ptr<MockQuote> q3(new MockQuote("123", 45));

  EXPECT_CALL(*q1, net_price(_)).WillOnce(Return(135));

  // when expects that net_price() gets called on q2 and q3 since net_price()
  // will be called once.
  //
  // .cpp:169: Failure
  // Actual function call count doesn't match EXPECT_CALL(*q3, net_price(_))...
  //          Expected: to be called once
  //            Actual: never called - unsatisfied and active
  // t_ex_basket.cpp:166: Failure
  // Actual function call count doesn't match EXPECT_CALL(*q2, net_price(_))...
  //          Expected: to be called once
  //            Actual: never called - unsatisfied and active
  // [  FAILED  ] BasketTest.checkTotal1 (2 ms)
  //
  // EXPECT_CALL(*q2, net_price(_))
  //     .WillOnce(Return(135));
  //
  // EXPECT_CALL(*q3, net_price(_))
  //     .WillOnce(Return(135));

  // so change "Times" or comment them out
  //
  // EXPECT_CALL(*q2, net_price(_))
  //   .Times(AnyNumber())
  //   .WillOnce(Return(135));
  //
  // EXPECT_CALL(*q3, net_price(_))
  //   .Times(AnyNumber())
  //   .WillOnce(Return(135));

  Basket sale;

  sale.add_item(q1);
  sale.add_item(q2);
  sale.add_item(q3);

  EXPECT_EQ(sale.total_receipt(cout), 135);
}

TEST(CxxCaseQuote, MockBulk)
{
  using namespace case_quote;

  // Bulk_quote sales which has discount: minimum 5 and 25% discount
  // 35*6*(1-.25) = 157.5
  shared_ptr<MockBulk> q1(new MockBulk("912", 35, 5, .25));
  shared_ptr<MockBulk> q2(new MockBulk("912", 35, 5, .25));
  shared_ptr<MockBulk> q3(new MockBulk("912", 35, 5, .25));
  shared_ptr<MockBulk> q4(new MockBulk("912", 35, 5, .25));
  shared_ptr<MockBulk> q5(new MockBulk("912", 35, 5, .25));

  EXPECT_CALL(*q1, net_price(_)).WillOnce(Return(157.5));

  Basket sale;

  sale.add_item(q1);
  sale.add_item(q2);
  sale.add_item(q3);
  sale.add_item(q4);
  sale.add_item(q5);

  EXPECT_EQ(sale.total_receipt(cout), 157.5);
}

namespace case_quote_clone
{
  class Quote
  {
  public:
    Quote()
        : book_no_()
        , price_(0.0)
    {}

    Quote(string const &book, double sale_price)
        : book_no_(book)
        , price_(sale_price)
    {}

    virtual ~Quote() {}

    std::string isbn() const { return book_no_; }

    // calculate net price but do not have "discount" concept
    virtual double net_price(size_t count) const { return count * price_; }

    // *cxx-clone* *cxx-move* copy version
    // *cxx-reference-qualifier*
    // note: shall have `const` or see *cxx-const-to-nonconst-error*
    // note: if there's no move verison, copy version will be used instead
    virtual Quote *clone() const &
    {
      cout << "quote::clone() &" << endl;
      return new Quote(*this);
    }

    // cxx-clone, move version
    virtual Quote *clone() const &&
    {
      cout << "quote::clone() &&" << endl;
      return new Quote(std::move(*this));
    }

  private:
    std::string book_no_;

  protected:
    double price_;
  };

  class Discount_Quote : public Quote
  {
  public:
    // *cxx-ctor*
    // Why need to have constructors in abstract class although cannot
    // define objects of this type directly? Becuase ctors in classes
    // derived from Disc_quote will use the Disc_quote ctor to construct
    // the Disc_quote part of their objects. Default ctor default
    // initialize those members.

    Discount_Quote()
        : quantity_(0)
        , discount_(0.0)
    {}
    Discount_Quote(string const &book,
                   double price,
                   size_t quantity,
                   double discount_percent)
        : Quote(book, price)
        , quantity_(quantity)
        , discount_(discount_percent)
    {}

    // *cxx-dtor*
    // no need to have virtual dtor here since Quote has it already

    // *cxx-abc*
    // okay to have in the middle of inheritance and "discount" concept
    virtual double net_price(size_t count) const = 0;

  protected:
    size_t quantity_;
    double discount_;
  };

  class Bulk_Quote : public Discount_Quote
  {
  public:
    Bulk_Quote(string const &book,
               double price,
               size_t quantity,
               double discount_percent)
        : Discount_Quote(book, price, quantity, discount_percent)
    {}

    // cxx-override cxx-const
    // const is one of override condition

    virtual double net_price(size_t count) const override
    {
      if (count >= quantity_)
        return count * (1 - discount_) * price_;
      else
        return count * price_;
    }

    // cxx-clone
    // cxx-error virtual Bulk_Quote* clone() const override &
    // cxx-const
    // if don't have const, no error but no override. so use override keyword
    virtual Bulk_Quote *clone() const & override
    {
      cout << "bulk_quote::clone() &" << endl;
      return new Bulk_Quote(*this);
    }

    virtual Bulk_Quote *clone() const && override
    {
      cout << "bulk_quote::clone() &&" << endl;
      return new Bulk_Quote(std::move(*this));
    }
  };

  double print_total(ostream &os, Quote const &item, size_t sold)
  {
    double net_price = item.net_price(sold);

    os << "isbn: " << item.isbn() << ", sold: " << sold
       << ", total due: " << net_price << endl;

    return net_price;
  }

  class Basket
  {
  public:
    // *cxx-undefined*
    // Without it, cause core when add_item is called.
    Basket()
        : items_{compare}
    {}

    void add_item(std::shared_ptr<Quote> const &item)
    {
      cout << "basket::add_item(shard_ptr)" << endl;
      items_.insert(item);
    }

    // copy version
    void add_item(Quote const &item)
    {
      cout << "basket::add_item(Quote const&)" << endl;
      items_.insert(std::shared_ptr<Quote>(item.clone()));
    }

    // move version *cxx-dynamic-binding*
    void add_item(Quote &&item)
    {
      cout << "basket::add_item(Quote &&)" << endl;
      items_.insert(std::shared_ptr<Quote>(std::move(item).clone()));
    }

    double total_receipt(ostream &os) const
    {
      double total{};

      for (auto iter = items_.cbegin(); iter != items_.cend();
           iter      = items_.upper_bound(*iter))
      {
        total = (*iter)->net_price(items_.count(*iter));
        os << "isbn: " << (*iter)->isbn() << ", sold: " << items_.count(*iter)
           << ", total sales: " << total << endl;
      }

      return total;
    }

  private:
    static bool compare(const std::shared_ptr<Quote> lhs,
                        const std::shared_ptr<Quote> rhs)
    {
      return lhs->isbn() < rhs->isbn();
    }

    using comp = bool(const std::shared_ptr<Quote>,
                      const std::shared_ptr<Quote>);

    std::multiset<shared_ptr<Quote>, comp *> items_;
  };

} // namespace case_quote_clone

// [ RUN      ] CxxCaseQuote.Clone
// basket::add_item(Quote &&)
// quote::clone() &&
// basket::add_item(Quote &&)
// quote::clone() &&
// basket::add_item(Quote &&)
// quote::clone() &&
// basket::add_item(Quote &&)
// bulk_quote::clone() &&
// basket::add_item(Quote &&)
// bulk_quote::clone() &&
// isbn: 123, sold: 3, total sales: 135
// isbn: 345, sold: 2, total sales: 90
// [       OK ] CxxCaseQuote.Clone (2 ms)

TEST(CxxCaseQuote, Clone)
{
  using namespace case_quote_clone;

  Basket sale;

  // Quote sales which has no discount. 45*3 = 135
  sale.add_item(Quote("123", 45));
  sale.add_item(Quote("123", 45));
  sale.add_item(Quote("123", 45));

  // minimum 3 and 15% discount. no discount 45*2 = 90
  sale.add_item(Bulk_Quote("345", 45, 3, .15));
  sale.add_item(Bulk_Quote("345", 45, 3, .15));

  sale.total_receipt(cout);
}
