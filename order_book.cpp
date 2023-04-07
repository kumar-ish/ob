#include "order_book.h"

bool OrderBook::addOrder(uint64_t order_id, Side side, uint64_t size, int32_t price)
{
    OrderMap& orderMap = getOrderMap(side);
    Order order{size, price};

    orderMap[order_id] = order;

    // We add the size of the order to the price map for the corresponding price
    PriceMap& priceMap = getPriceMap(side);
    priceMap[price] += size;

    return determineDepthChange(priceMap, price, price);
}

bool OrderBook::updateOrder(uint64_t order_id, Side side, uint64_t newSize, int32_t newPrice)
{
    OrderMap& orderMap = getOrderMap(side);
    Order existingOrder = orderMap[order_id];

    if (newSize != 0)
    {
        // We create a new order with the new size and price
        Order newOrder = {newSize, newPrice};
        orderMap[order_id] = newOrder;
    }
    else
    {
        orderMap.erase(order_id);
    }

    return updateOrderBook(side, existingOrder.size, existingOrder.price, newSize, newPrice);
}

bool OrderBook::deleteOrder(uint64_t order_id, Side side)
{
    OrderMap& orderMap = getOrderMap(side);
    Order order = orderMap[order_id];

    // We remove the entire volume on the order
    return updateOrder(order_id, side, 0, order.price);
}

bool OrderBook::tradeOrder(uint64_t order_id, Side side, uint64_t volume)
{
    OrderMap& orderMap = getOrderMap(side);
    Order order = orderMap[order_id];

    // Volume get traded out of the order, so we remove it from the existing size
    return updateOrder(order_id, side, order.size - volume, order.price);
}

std::string OrderBook::to_string(int depth)
{
    std::string out = "[" + bids.priceMapRepr(depth) + "], [" + asks.priceMapRepr(depth) + "]";

    return out;
}

std::string OrderBook::PriceMap::priceMapRepr(int depthTraversalLimit)
{
    std::string out;
    auto it = this->begin();
    // We stop at the minimum of the depth and the size of the orderbook if we're limiting to depth,
    // or otherwise output the entire price map
    for (int i = 0; i < depthTraversalLimit; it++) {
        out += "(" + std::to_string(it->first) + ", " + std::to_string(it->second) + ")";

        if (++i != depthTraversalLimit)
        {
            out += ", ";
        }
    }

    return out;
}

std::string OrderBook::PriceMap::priceMapRepr() {
    return priceMapRepr((int) this->size());
}

OrderBook::OrderMap& OrderBook::getOrderMap(Side side)
{
    return side == Side::BUY ? bidOrderMap : askOrderMap;
}

OrderBook::PriceMap& OrderBook::getPriceMap(Side side)
{
    return side == Side::BUY ? bids : asks;
}

// Time complexity of this is O(log(n) + d), where `n` is the number of discrete price points in
// the order book and `d` is the depth of the order book that we're concerned with outputting if
// it has changed
bool OrderBook::determineDepthChange(PriceMap& priceMap, int64_t oldPrice, int64_t newPrice)
{
    if ((int) priceMap.size() < depth)
    {
        return true;
    }

    // We want to get the more competitive price between the old and new price -- in order updates, it could go lower or higher, 
    // whereas for all other order types it will always be the same
    IntCompFn fn = priceMap.key_comp();
    // We get the price that our price map considers more competitive
    int64_t upperBoundPrice = fn(newPrice, oldPrice) ? newPrice : oldPrice;

    auto upperIt = priceMap.find(upperBoundPrice);

    // and then search if it's in the first `depth` elements
    for (auto [it, i] = std::tuple{priceMap.cbegin(), 0}; i < depth; i++, it++)
    {
        if (it == upperIt)
        {
            return true;
        }
    }

    return false;
}

bool OrderBook::updateOrderBook(Side side, uint64_t oldSize, int32_t oldPrice, uint64_t newSize, int32_t newPrice)
{
    PriceMap& priceMap = getPriceMap(side);

    if (newPrice == oldPrice)
    {
        auto change = newSize - oldSize;
        // If the price is the same, we just need to update the size
        // We don't need to worry about the price changing
        priceMap[newPrice] += change;
    }
    else
    {
        // We remove the size from the existing order
        priceMap[oldPrice] = priceMap[oldPrice] - oldSize;
        // We add the size to the new price
        priceMap[newPrice] += newSize;
    }

    // Call procedure before erasing (as you'd need the iterator to the greater / higher price,
    // and once it's erased you can't get it back)
    bool logDepthChange = determineDepthChange(priceMap, oldPrice, newPrice);

    // If the size corresponding to this price point in the orderbook has become 0, we remove it
    if (priceMap[oldPrice] == 0)
    {
        priceMap.erase(oldPrice);
    }

    return logDepthChange;
}