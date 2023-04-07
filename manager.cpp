
#include <any>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "manager.h"

OrderBook& OrderBookManager::findOrderBookEntry(std::string symbol)
{
    auto it = orderBookMap.find(symbol);

    if (it != orderBookMap.end())
    {
        return it->second;
    }

    return orderBookMap.insert({symbol, OrderBook{depth}}).first->second;
}

std::string OrderBookManager::outputTickerString(std::string& symbol, bool limitToDepth)
{
    auto orderBook = findOrderBookEntry(symbol);
    return symbol + ", " + orderBook.to_string(limitToDepth);
}
