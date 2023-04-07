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
#include "order_book.h"

// Copied from cppreference page on std::visit
// Helper function to create a visitor from a set of lambdas
template <class... Ts>
struct make_visitor : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
make_visitor(Ts...) -> make_visitor<Ts...>;

// Allows us to manage orders to multiple order books at once, mapping from symbol to order book
class OrderBookManager
{
private:
    int depth;

    // Maps from symbol to corresponding order book
    using OrderBookMap = std::unordered_map<std::string, OrderBook>;
    OrderBookMap orderBookMap;

    OrderBook& findOrderBookEntry(std::string symbol);

public:
    explicit OrderBookManager(int _depth) : depth(_depth)
    {
    }

    template <typename T>
    bool handleEntry(T entry)
    {
        static_assert(std::is_same_v<T, OrderAdd> || std::is_same_v<T, OrderUpdate> || std::is_same_v<T, OrderDelete> ||
                          std::is_same_v<T, OrderTrade>,
                      "Invalid type passed to handleEntry");

        std::string symbol(entry.symbol, 3);
        auto& orderBook = findOrderBookEntry(symbol);

        make_visitor visitor = make_visitor{
            [&](OrderAdd order) { return orderBook.addOrder(order.order_id, order.side, order.size, order.price); },
            [&](OrderUpdate order)
            { return orderBook.updateOrder(order.order_id, order.side, order.size, order.price); },
            [&](OrderDelete order) { return orderBook.deleteOrder(order.order_id, order.side); },
            [&](OrderTrade order) { return orderBook.tradeOrder(order.order_id, order.side, order.volume); }};

        // match x with
        // | OrderAdd ->
        // | OrderUpdate ->
        // | OrderDelete ->
        // | OrderTrade ->

        return visitor(entry);
    }

    // Outputs a string representation of the order book information for a single ticker, in the following format
    // `symbol, [(bidPrice1, bidVolume1), ...], [(askPrice1, askVolume1), ...]`
    // The output order of the bid and ask arrays is strictly by competitiveness
    std::string outputTickerString(std::string& symbol, bool limitToDepth = false);
};
