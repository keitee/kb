#include <iostream>
#include <set>
#include <memory>

#include "gtest/gtest.h"

using namespace std;

class Quote {
    private:
        string s_book_no;

    protected:
        double price;

    public:
        Quote() : s_book_no(""), price(0.0) {}
        Quote(const string book, const double price) : s_book_no(book), price(price) {}

        // have to since it's intended to be inherited
        virtual ~Quote() {}

        string isbn() const { return s_book_no; }

        // get net price without discount
        virtual double net_price(size_t sales_count) const
        { return sales_count*price; }

        // virtual copy, copy version
        virtual Quote *clone() const &
        { 
            cout << "quote::clone::copy" << endl;
            return new Quote(*this); 
        }

        // virtual copy, move version
        virtual Quote *clone() const &&
        { 
            cout << "quote::clone::move" << endl;
            return new Quote(std::move(*this)); 
        }
};

class Disc_quote : public Quote
{
    protected:
        double d_quantity;
        double d_discount;

    public:
        Disc_quote(const string book, const double price, const double quantity, const double discount) :
            Quote(book, price), d_quantity(quantity), d_discount(discount) {}

        double net_price(size_t sales_count) const = 0;
};

class Bulk_quote : public Disc_quote
{
    public:
        Bulk_quote(const string book, const double price, const double quantity, const double discount) :
            Disc_quote(book, price, quantity, discount) {}

        // double net_price(size_t sales_count) const override
        double net_price(size_t sales_count) const 
        {
            if( sales_count >= d_quantity )
                return sales_count*(1-d_discount)*price;
            else
                return sales_count*price;
        };

        // virtual copy, copy version
        //  Unlike Quote::clone(), when there is no const, no compile error but
        //  gets always Quote version since no override. see
        //  *cpp-override-condition*
        virtual Bulk_quote *clone() const &
        { 
            cout << "bulk::clone::copy" << endl;
            return new Bulk_quote(*this); 
        }

        // virtual copy, move version
        virtual Bulk_quote *clone() const &&
        { 
            cout << "bulk::clone::move" << endl;
            return new Bulk_quote(std::move(*this)); 
        }
};

// // user class
// class Basket
// {
//     public:
//         Basket() : items(compare) {}
// 
//         void add_item(const shared_ptr<Quote> &item);
// 
//         // virtual copy, copy version
//         // which signals to use lvalue object, lvalue reference qualifier
//         void add_item(const Quote &item)
//         {
//             cout << "basket::add_item::copy version" << endl;
//             items.insert(shared_ptr<Quote>(item.clone()));
//         }
// 
//         // virtual copy, move version
//         // which signals to use rvalue object, rvalue reference qualifier
//         void add_item(Quote &&item)
//         {
//             cout << "basket::add_item::move version" << endl;
//             items.insert(shared_ptr<Quote>(std::move(item).clone()));
//         }
// 
// 
//         double total_receipt(ostream &os) const;
// 
//     private:
//         static bool compare(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs)
//         { return lhs->isbn() < rhs->isbn(); }
// 
//         // using comp = bool (*)(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs);
//         // multiset<shared_ptr<Quote>, comp> items;
// 
//         using comp = bool (const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs);
//         multiset<shared_ptr<Quote>, comp*> items;
// };
// 
// void Basket::add_item(const shared_ptr<Quote> &item)
// {
//     cout << "basket::add_item::copy version" << endl;
//     items.insert(item);
// }
// 
// double Basket::total_receipt(ostream &os) const
// {
//     for (auto iter = items.cbegin(); iter != items.cend();
//             iter = items.upper_bound(*iter))
//     {
//         os << "isbn : " << (*iter)->isbn() 
//             << ", sold : " << items.count(*iter)
//             << ", total sales: " << (*iter)->net_price( items.count(*iter)) 
//             << endl;
//     }
// }

// namespace {

class BulkQuote1Test : public ::testing::Test
{
    protected:
        BulkQuote1Test() : items{compare} {}

        virtual ~BulkQuote1Test() {}

        virtual void SetUp() 
        {
            // *TN*
            // In this approach, have to crate each class for each input set
            // condition.
            //
            // // Quote sales which has no discount. 45*3 = 135
            // items.insert(shared_ptr<Quote>(new Quote("123", 45)));
            // items.insert(shared_ptr<Quote>(new Quote("123", 45)));
            // items.insert(shared_ptr<Quote>(new Quote("123", 45)));
        }

        virtual void TearDown()
        {
        }

        double total_receipt() const
        {
            double total{};

            // *TN*
            // can use cout
            for (auto it = items.cbegin(); it != items.cend();
                    it = items.upper_bound(*it))
            {
                total = (*it)->net_price(items.count(*it));

                cout << "isbn: " << (*it)->isbn()
                    << ", sold: " << items.count(*it)
                    << ", total sales: " << total;
                cout << endl;
            }

            return total;
        }

    // private:
    // *TN* TEST_F seems to create derived class since emits errors to access
    // private members.
    protected:
        static bool compare(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs)
        {
            return lhs->isbn() < rhs->isbn();
        }

        using comp = bool(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs);
        multiset<shared_ptr<Quote>, comp*> items;
};

// *TN*
// In this approach, have to crate each class for each input set
// condition.
//
// TEST_F(BulkQuote1Test, checkTotal)
// {
//     EXPECT_EQ(135, total_receipt());
// }


// Quote sales which has no discount. 45*3 = 135
TEST_F(BulkQuote1Test, checkTotal1)
{
    items.insert(shared_ptr<Quote>(new Quote("123", 45)));
    items.insert(shared_ptr<Quote>(new Quote("123", 45)));
    items.insert(shared_ptr<Quote>(new Quote("123", 45)));

    EXPECT_EQ(135, total_receipt());

    items.clear();
}

// minimum 3 and 15% discount. no discount 45*2 = 90
TEST_F(BulkQuote1Test, checkTotal2)
{
    items.insert(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));
    items.insert(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));

    // ERROR
    EXPECT_EQ(135, total_receipt());

    items.clear();
}

// Bulk_quote sales which has discount: minimum 3 and 15% discount
// 35*4*(1-.15) = 119
TEST_F(BulkQuote1Test, checkTotal3)
{
    items.insert(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
    items.insert(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
    items.insert(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
    items.insert(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));

    EXPECT_EQ(119, total_receipt());

    items.clear();
}

// Bulk_quote sales which has discount: minimum 5 and 25% discount
// 35*6*(1-.25) = 157.5
TEST_F(BulkQuote1Test, checkTotal4)
{
    items.insert(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    items.insert(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    items.insert(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    items.insert(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    items.insert(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    items.insert(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));

    EXPECT_EQ(157.5, total_receipt());

    items.clear();
}


// *TN*
// In this approach, have to crate each class for each input set
// condition.
// //     // minimum 3 and 15% discount. no discount 45*2 = 90
// //     sale.add_item(Bulk_quote("345", 45, 3, .15));
// //     sale.add_item(Bulk_quote("345", 45, 3, .15));
// 
// class BulkQuote2Test : public ::testing::Test
// {
//     protected:
//         BulkQuote2Test() : items{compare} {}
// 
//         virtual ~BulkQuote2Test() {}
// 
//         virtual void SetUp() 
//         {
//             // Quote sales which has no discount. 45*3 = 135
//             items.insert(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));
//             items.insert(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));
//         }
// 
//         virtual void TearDown()
//         {
//         }
// 
//         double total_receipt() const
//         {
//             double total{};
// 
//             for (auto it = items.cbegin(); it != items.cend();
//                     it = items.upper_bound(*it))
//             {
//                 total = (*it)->net_price(items.count(*it));
// 
//                 cout << "isbn: " << (*it)->isbn()
//                     << ", sold: " << items.count(*it)
//                     << ", total sales: " << total;
//                 cout << endl;
//             }
// 
//             return total;
//         }
// 
//     private:
//         static bool compare(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs)
//         {
//             return lhs->isbn() < rhs->isbn();
//         }
// 
//         using comp = bool(const shared_ptr<Quote> lhs, const shared_ptr<Quote> rhs);
//         multiset<shared_ptr<Quote>, comp*> items;
// };
// 
// TEST_F(BulkQuote2Test, checkTotal)
// {
//     EXPECT_EQ(135, total_receipt());
// }


// }   // namespace

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// kyoupark@kit-debian64:~/works/t_ex_quote$ ./unittest 
// [==========] Running 4 tests from 1 test case.
// [----------] Global test environment set-up.
// [----------] 4 tests from BulkQuote1Test
// [ RUN      ] BulkQuote1Test.checkTotal1
// isbn: 123, sold: 3, total sales: 135
// [       OK ] BulkQuote1Test.checkTotal1 (1 ms)
// [ RUN      ] BulkQuote1Test.checkTotal2
// isbn: 345, sold: 2, total sales: 90
// t_ex_quote.cpp:231: Failure
// Expected equality of these values:
//   135
//   total_receipt()
//     Which is: 90
// [  FAILED  ] BulkQuote1Test.checkTotal2 (0 ms)
// [ RUN      ] BulkQuote1Test.checkTotal3
// isbn: 678, sold: 4, total sales: 119
// [       OK ] BulkQuote1Test.checkTotal3 (0 ms)
// [ RUN      ] BulkQuote1Test.checkTotal4
// isbn: 912, sold: 6, total sales: 157.5
// [       OK ] BulkQuote1Test.checkTotal4 (0 ms)
// [----------] 4 tests from BulkQuote1Test (1 ms total)
// 
// [----------] Global test environment tear-down
// [==========] 4 tests from 1 test case ran. (1 ms total)
// [  PASSED  ] 3 tests.
// [  FAILED  ] 1 test, listed below:
// [  FAILED  ] BulkQuote1Test.checkTotal2
// 
//  1 FAILED TEST
// kyoupark@kit-debian64:~/works/t_ex_quote$ 


