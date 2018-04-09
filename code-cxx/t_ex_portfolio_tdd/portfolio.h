//
// TDD, C6, Portfolio
//

#include <string>
#include <vector>
#include <unordered_map>
#include "boost/date_time/gregorian/gregorian_types.hpp"

using namespace boost::gregorian;

class InvalidPurchaseException : public std::exception
{
    public:
        const char *what() const noexcept override
        {
            return "Invalid Purchase";
        }
};

class InvalidSellException : public std::exception
{
    public:
        const char *what() const noexcept override
        {
            return "Invlid Sell";
        }
};

struct PurchaseRecord
{
    PurchaseRecord(unsigned int shareCount, const date &datePurchase)
        : count_(shareCount), date_(datePurchase)
    {}

    unsigned int count_;
    date date_;
};

class Portfolio
{
    public:
        static const date FIXED_PURCHASE_DATE;

        Portfolio()
        {}

        bool IsEmpty()
        {
            return 0 == holdings_.size();
        }

        void Purchase(const std::string &symbol, unsigned int shareCount,
                const date &transactionDate = FIXED_PURCHASE_DATE)
        {
            // Our definition of empty is whether  the portfolio contains any
            // symbols. So, is that empty? Or should we disallow such purchases?
            // For the purposes of moving forward, we choose the latter and
            // write a test named ThrowsOnPurchaseOfZeroShares.

            if (!shareCount)
                throw InvalidPurchaseException();

            holdings_[symbol] += shareCount;

            purchases_.push_back(PurchaseRecord(shareCount, transactionDate));

            // *TN* do not work since insert fails when element is already
            // exist.
            //
            // const auto &share = holdings_.find(symbol);
            // if( share == holdings_.end() )
            //     holdings_.insert( {symbol, shareCount} );
            // else
            //     holdings_.insert( {symbol, share->second + shareCount} );
        }

        std::vector<PurchaseRecord> Purchases(const std::string &symbol) const
        {
            return purchases_;
        }

        void Sell(const std::string &symbol, unsigned int shareCount)
        {
            auto count = ShareCount(symbol);

            if (shareCount > count)
                throw InvalidSellException();

            holdings_[symbol] = count - shareCount;

            // * duplicate in find() code
            // * fails when sells more than purchased.
            //
            // const auto &share = holdings_.find(symbol);
            // 
            // if (share == holdings_.end())
            //    throw InvalidSellException(); 
            //
            // holdings_[symbol] -= shareCount;
        }

        unsigned int ShareCount(const std::string &symbol)
        {
            // error
            // auto &share = holdings_.find(symbol); 
            //
            // ok
            // auto share = holdings_.find(symbol); 

            // ok
            const auto &share = holdings_.find(symbol); 

            if( share == holdings_.end() )
                return 0u;

            return share->second;
        }

    private:
        std::unordered_map<std::string, unsigned int> holdings_;
        std::vector<PurchaseRecord> purchases_;

        // date FIXED_PURCHASE_DATE{date(2014, Jan, 1)};
};

// const boost::gregorian::date Portfolio::FIXED_PURCHASE_DATE(date(2014, Jan, 1));
const date Portfolio::FIXED_PURCHASE_DATE(date(2014, Jan, 1));

