#include <iostream>
#include <set>
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;

// class Quote {
//     private:
//         string s_book_no;
// 
//     protected:
//         double price;
// 
//     public:
//         Quote() : s_book_no(""), price(0.0) {}
//         Quote(const string book, const double price) : s_book_no(book), price(price) {}
// 
//         // have to since it's intended to be inherited
//         virtual ~Quote() {}
// 
//         string isbn() const { return s_book_no; }
// 
//         // get net price without discount
//         virtual double net_price(size_t sales_count) const
//         { return sales_count*price; }
// };

class Quote {
    public:
        Quote() {}
        Quote(const string book, const double price) 
            : book_(book), price_(price) {}

        // have to since it's intended to be inherited
        virtual ~Quote() {}

        string isbn() const { return book_; }

        // get net price without discount
        virtual double net_price(size_t sales_count) const = 0;

    private:
        string book_;
        double price_;
};

class MockQuote : public Quote {
    public:
        MockQuote(const string book, const double price)
            : Quote(book, price) {}

        // MOCK_METHOD0(~Quote, ());
        MOCK_CONST_METHOD1(net_price, double (size_t sales_count));
};


class Disc_quote : public Quote
{
    protected:
        double d_quantity;
        double d_discount;

    public:
        Disc_quote(const string book, const double price, const double quantity, const double discount) :
            Quote(book, price), d_quantity(quantity), d_discount(discount) {}

        // double net_price(size_t sales_count) const = 0;
};

class Bulk_quote : public Disc_quote
{
    public:
        Bulk_quote(const string book, const double price, const double quantity, const double discount) :
            Disc_quote(book, price, quantity, discount) {}

        // double net_price(size_t sales_count) const override
        // double net_price(size_t sales_count) const 
        // {
        //     if( sales_count >= d_quantity )
        //         return sales_count*(1-d_discount)*price;
        //     else
        //         return sales_count*price;
        // };
};

// class Bulk_quote {
//     public:
//         Bulk_quote(const string book, const double price, 
//                 const double quantity, const double discout) {}
//         virtual ~Bulk_quote() {}
//         string isbn() const {}
//         virtual double net_price(size_t sales_count) const = 0;
// };

class MockBulk : public Bulk_quote {
    public:

        MockBulk(const string book, const double price, const double quantity, const double discount) :
            Bulk_quote(book, price, quantity, discount) {}

        MOCK_CONST_METHOD1(net_price, double (size_t sales_count));
};


// user class
class Basket
{
    public:
        Basket() : items(compare) {}

        void add_item(const shared_ptr<Quote> &item);
        double total_receipt(ostream &os) const;

    private:
        static bool compare(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs)
        { return lhs->isbn() < rhs->isbn(); }

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
    double total{};

    for (auto iter = items.cbegin(); iter != items.cend();
            iter = items.upper_bound(*iter))
    {
        total = (*iter)->net_price(items.count(*iter));
        os << "isbn : " << (*iter)->isbn() 
            << ", sold : " << items.count(*iter)
            << ", total sales: " << total
            << endl;
    }

    return total;
}


TEST(BasketTest, checkTotal1) 
{
    using ::testing::Return;
    using ::testing::_;
    using ::testing::AnyNumber;

    // Quote sales which has no discount. 45*3 = 135
    shared_ptr<MockQuote> q1(new MockQuote("123", 45));
    shared_ptr<MockQuote> q2(new MockQuote("123", 45));
    shared_ptr<MockQuote> q3(new MockQuote("123", 45));

    EXPECT_CALL(*q1, net_price(_))
        .WillOnce(Return(135));

    // t_ex_basket.cpp:169: Failure
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

    // EXPECT_CALL(*q3, net_price(_))
    //     .WillOnce(Return(135));

    EXPECT_CALL(*q2, net_price(_))
        .Times(AnyNumber())
        .WillOnce(Return(135));

    EXPECT_CALL(*q3, net_price(_))
        .Times(AnyNumber())
        .WillOnce(Return(135));

    Basket sale;

    sale.add_item(q1);
    sale.add_item(q2);
    sale.add_item(q3);

    EXPECT_EQ(135, sale.total_receipt(cout));
}

TEST(BasketTest, checkTotal2) 
{
    using ::testing::Return;
    using ::testing::_;
    using ::testing::AnyNumber;

    // Bulk_quote sales which has discount: minimum 5 and 25% discount
    // 35*6*(1-.25) = 157.5
    shared_ptr<MockBulk> q1(new MockBulk("912", 35, 5, .25));
    shared_ptr<MockBulk> q2(new MockBulk("912", 35, 5, .25));
    shared_ptr<MockBulk> q3(new MockBulk("912", 35, 5, .25));
    shared_ptr<MockBulk> q4(new MockBulk("912", 35, 5, .25));
    shared_ptr<MockBulk> q5(new MockBulk("912", 35, 5, .25));

    EXPECT_CALL(*q1, net_price(_))
        .WillOnce(Return(157.5));

    EXPECT_CALL(*q2, net_price(_))
        .Times(AnyNumber())
        .WillOnce(Return(157.5));

    EXPECT_CALL(*q3, net_price(_))
        .Times(AnyNumber())
        .WillOnce(Return(157.5));

    EXPECT_CALL(*q4, net_price(_))
        .Times(AnyNumber())
        .WillOnce(Return(157.5));

    EXPECT_CALL(*q5, net_price(_))
        .Times(AnyNumber())
        .WillOnce(Return(157.5));

    Basket sale;

    sale.add_item(q1);
    sale.add_item(q2);
    sale.add_item(q3);
    sale.add_item(q4);
    sale.add_item(q5);

    // EXPECT_EQ(135, sale.total_receipt(cout));
    EXPECT_EQ(157.5, sale.total_receipt(cout));
}

int main(int argc, char **argv)
{
    std::cout << "Running main() from gmock_main.cc\n";

    // Since Google Mock depends on Google Test, InitGoogleMock() is
    // also responsible for initializing Google Test.  Therefore there's
    // no need for calling testing::InitGoogleTest() separately.
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
