#pragma once

#include <any>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "message.h"


// Information related to a single order
struct Order
{
    uint64_t size;
    int32_t price;
};

// Order books contain both a bid and an ask side
// This class allows us to modify the state of the orderbook and get a string representation of the orderbook
class OrderBook
{
public:
    explicit OrderBook(int _depth) : depth(_depth) {};

    // These allow us to modify the state of our orderbook by processing information related to the orders
    bool addOrder(uint64_t order_id, Side side, uint64_t size, int32_t price);
    bool updateOrder(uint64_t order_id, Side side, uint64_t newSize, int32_t newPrice);
    bool deleteOrder(uint64_t order_id, Side side);
    bool tradeOrder(uint64_t order_id, Side side, uint64_t volume);

    // Outputs a string representation of the order book, in the following format
    // `[(bidPrice1, bidVolume1), ...], [(askPrice1, askVolume1), ...]`
    // The output order of the bid and ask arrays is strictly by competitiveness
    std::string to_string(int depth);
private:
    // Maps from order_id to individual order information
    using OrderMap = std::unordered_map<uint64_t, Order>;

    // Maps from price to the size on that price in the orderbook
    // We use to a comparison function so that we can handle the cases for both bids and asks generically
    using IntCompFn = std::function<bool(int64_t, int64_t)>;
    class PriceMap : public std::map<int64_t, uint64_t, IntCompFn> {
    public:
        explicit PriceMap(IntCompFn compFn) : std::map<int64_t, uint64_t, IntCompFn>(compFn) {}

        // Outputs a string representation of a singular price map
        // Bids must have the highest bid price first, asks show the lowest ask price first
        std::string priceMapRepr(int depth);
        std::string priceMapRepr();
    };

    OrderMap bidOrderMap{};
    OrderMap askOrderMap{};

    PriceMap asks{IntCompFn{std::less<int64_t>()}};
    PriceMap bids{IntCompFn{std::greater<int64_t>()}};
    // The depth of the orderbook up to which we should optionally display
    int depth;

    // The bid and the ask order/price maps are different to one another, so this fetches the corresponding one
    OrderMap& getOrderMap(Side side);
    PriceMap& getPriceMap(Side side);

    // Returns whether a change in the first `depth` levels is warranted given the old and new price
    bool determineDepthChange(PriceMap& priceMap, int64_t oldPrice, int64_t newPrice);

    // Returns whether a depth change is given the old and new price
    bool updateOrderBook(Side side, uint64_t oldSize, int32_t oldPrice, uint64_t newSize, int32_t newPrice);
};
