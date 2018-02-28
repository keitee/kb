#include <iostream>
#include <set>
#include <memory>

using namespace std;

// base class
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
        //  *cpp-const-this* if not use `const` on this, 
        //  see *cpp-const-to-nonconst-error* when use
        //
        // void add_item( const Quote& sale )
        // { items.insert( shared_ptr<Quote>( sale.clone())); }
        //
        // since sale is const object and error to call nonconst version on
        // const object. *cpp-remember* always makes a call on `object`
        //
        // virtual Quote *clone() const &
        // { 
        //     cout << "quote::clone::copy" << endl;
        //     return new Quote(*this); 
        // }

        // virtual copy, move version
        //  uses *reference-qualifier* and *cpp-overload-move*.
        //  if there is no move version, copy version is used when move version
        //  of add_item() is called.
        //
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
        // virtual Bulk_quote *clone() const &
        // { 
        //     cout << "bulk::clone::copy" << endl;
        //     return new Bulk_quote(*this); 
        // }

        // virtual copy, move version
        virtual Bulk_quote *clone() const &&
        { 
            cout << "bulk::clone::move" << endl;
            return new Bulk_quote(std::move(*this)); 
        }
};

// user class
class Basket
{
    public:
        Basket() : items(compare) {}

        void add_item(const shared_ptr<Quote> &item);

        // virtual copy, copy version
        // which signals to use lvalue object, lvalue reference qualifier
        // void add_item(const Quote &item)
        // {
        //     cout << "basket::add_item::copy version" << endl;
        //     items.insert(shared_ptr<Quote>(item.clone()));
        // }

        // virtual copy, move version
        // which signals to use rvalue object, rvalue reference qualifier
        void add_item(Quote &&item)
        {
            cout << "basket::add_item::move version" << endl;
            items.insert(shared_ptr<Quote>(std::move(item).clone()));
        }


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
    for (auto iter = items.cbegin(); iter != items.cend();
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

    // // Quote sales which has no discount. 45*3 = 135
    // sale.add_item(shared_ptr<Quote>(new Quote("123", 45)));
    // sale.add_item(shared_ptr<Quote>(new Quote("123", 45)));
    // sale.add_item(make_shared<Quote>("123", 45));

    // // minimum 3 and 15% discount. no discount 45*2 = 90
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("345", 45, 3, .15)));

    // // Bulk_quote sales which has discount: minimum 3 and 15% discount
    // // 35*4*(1-.15) = 119
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("678", 35, 3, .15)));

    // // Bulk_quote sales which has discount: minimum 5 and 25% discount
    // // 35*6*(1-.25) = 157.5
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));
    // sale.add_item(shared_ptr<Quote>(new Bulk_quote("912", 35, 5, .25)));

    // Quote sales which has no discount. 45*3 = 135
    sale.add_item(Quote("123", 45));
    sale.add_item(Quote("123", 45));
    sale.add_item(Quote("123", 45));

    // minimum 3 and 15% discount. no discount 45*2 = 90
    sale.add_item(Bulk_quote("345", 45, 3, .15));
    sale.add_item(Bulk_quote("345", 45, 3, .15));

    // Bulk_quote sales which has discount: minimum 3 and 15% discount
    // 35*4*(1-.15) = 119
    sale.add_item(Bulk_quote("678", 35, 3, .15));
    sale.add_item(Bulk_quote("678", 35, 3, .15));
    sale.add_item(Bulk_quote("678", 35, 3, .15));
    sale.add_item(Bulk_quote("678", 35, 3, .15));

    // Bulk_quote sales which has discount: minimum 5 and 25% discount
    // 35*6*(1-.25) = 157.5
    sale.add_item(Bulk_quote("912", 35, 5, .25));
    sale.add_item(Bulk_quote("912", 35, 5, .25));
    sale.add_item(Bulk_quote("912", 35, 5, .25));
    sale.add_item(Bulk_quote("912", 35, 5, .25));
    sale.add_item(Bulk_quote("912", 35, 5, .25));
    sale.add_item(Bulk_quote("912", 35, 5, .25));
 
    sale.total_receipt(cout);

    // => basket size: 15
    // 123 occurs, 3 times, net price: 135
    // 345 occurs, 2 times, net price: 90
    // 678 occurs, 4 times, net price: 119
    // 912 occurs, 6 times, net price: 157.5
    // isbn: 123, sold: 3, total due: 135
    // isbn: 345, sold: 2, total due: 90
    // isbn: 678, sold: 4, total due: 119
    // isbn: 912, sold: 6, total due: 157.5
    // => total sale price: 2.11371e-314
}

