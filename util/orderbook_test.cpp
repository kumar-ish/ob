#include <cassert>
#include <iostream>

#include "../message.h"
#include "../manager.h"
#include "../order_book.h"


class MessageCreator
{
private:
    int counter = 1;
public:
    MsgWithHeader createMsg(EventType type, std::any order);
};

MsgWithHeader MessageCreator::createMsg(EventType type, std::any order) {
    return MsgWithHeader{ .header={.seq_num=counter++, .msg_type=type}, .msg=order};
}


int main() 
{
    OrderBookManager manager(2);

    std::string symbol("VIV", 3);

    // ADDS
    // Buys, so bids, so should sorted descending -- so orders that are added after the second one should be rejected
    OrderAdd addOrder = { .symbol={'V', 'I', 'V'}, .order_id=1, .side=Side::SELL, .size=100, .price=100 };
    OrderAdd addOrder1 = { .symbol={'V', 'I', 'V'}, .order_id=2, .side=Side::SELL, .size=100, .price=99 };
    OrderAdd addOrder2 = { .symbol={'V', 'I', 'V'}, .order_id=3, .side=Side::SELL, .size=100, .price=98 };
    OrderAdd addOrder3 = { .symbol={'V', 'I', 'V'}, .order_id=4, .side=Side::SELL, .size=100, .price=97 };

    assert(manager.handleEntry(addOrder));
    assert(manager.handleEntry(addOrder1));
    assert(manager.handleEntry(addOrder2));
    assert(manager.handleEntry(addOrder3));
    std::cout << manager.outputTickerString(symbol, true) << std::endl;

    // DELETES
    OrderDelete delOrder = { .symbol={'V', 'I', 'V'}, .order_id=1, .side=Side::SELL };
    assert(!manager.handleEntry(delOrder)); // should now be 99, 98, 97 -- and it should be displayed because the depth was affected
    std::cout << manager.outputTickerString(symbol, true) << std::endl;

    OrderDelete delOrder1 = { .symbol={'V', 'I', 'V'}, .order_id=4, .side=Side::SELL };
    assert(manager.handleEntry(delOrder1)); // should now be 99, 98 -- it should not be displayed because the depth was not affected

    // Lets add them back
    assert(!manager.handleEntry(addOrder));
    assert(manager.handleEntry(addOrder3));

    // UPDATES
    OrderUpdate updateOrder = { .symbol={'V', 'I', 'V'}, .order_id=1, .side=Side::SELL, .size=50, .price=101 };
    assert(!manager.handleEntry(updateOrder)); // should now be 101, 99, 98, 97 -- but we can't see 98/97 because depth is 2

    OrderUpdate updateOrder1 = { .symbol={'V', 'I', 'V'}, .order_id=4, .side=Side::SELL, .size=100, .price=90 };
    assert(manager.handleEntry(updateOrder1)); // should now be 101, 99, 98, 90 -- but we can't see 98/90 because depth is 2
    std::cout << manager.outputTickerString(symbol, true) << std::endl;

    OrderUpdate updateOrder2 = { .symbol={'V', 'I', 'V'}, .order_id=3, .side=Side::SELL, .size=10, .price=100 };
    assert(manager.handleEntry(updateOrder2)); // should now be 101, 100, 99, 90 -- but we can't see 98/90 because depth is 2
    std::cout << manager.outputTickerString(symbol, true) << std::endl;

    // TRADES
    OrderTrade tradeOrder = { .symbol={'V', 'I', 'V'}, .order_id=4, .side=Side::SELL, .volume=100 };
    assert(manager.handleEntry(tradeOrder)); // should now be 101, 100, 99 -- but we can't see 98/90 because depth is 2
    std::cout << manager.outputTickerString(symbol, true) << std::endl;

    OrderUpdate updateOrder3 = { .symbol={'V', 'I', 'V'}, .order_id=1, .side=Side::SELL, .size=10, .price=100 };
    assert(manager.handleEntry(updateOrder3)); // should now be 100, 99 -- but we can't see 98/90 because depth is 2
    std::cout << manager.outputTickerString(symbol, true) << std::endl;

    OrderAdd addOrder4 = { .symbol={'V', 'I', 'V'}, .order_id=5, .side=Side::SELL, .size=110, .price=105 };
    assert(!manager.handleEntry(addOrder4)); // should now be 100, 99 -- but we can't see 98/90 because depth is 2
    std::cout << manager.outputTickerString(symbol, true) << std::endl;

    return 0;
}