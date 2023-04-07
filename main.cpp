#include "message.h"
#include "manager.h"

bool debug = false;

int readDepth(char* depthRepr)
{
	auto depthWrapper = std::string(depthRepr);
	int depth;

	try
	{
		depth = std::stoi(depthWrapper);
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << "The provided depth argument (\"" << depthRepr << "\") could not be parsed as a number" << std::endl;
		std::exit(2);
	}

	if (depth < 1)
	{
		// positive integer \equiv non-negative && non-zero integer in this context
		std::cerr << "The provided depth argument (\"" << depthRepr << "\") must be a positive integer" << std::endl;
		std::exit(3);
	}

	return depth;
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <depth>" << std::endl;
		std::exit(1);
	}

	int depth = readDepth(argv[1]);

	char* isDebug = std::getenv("DEBUG");
	if (isDebug != nullptr && tolower(isDebug[0]) != 'f')
	{
		debug = true;
	}

	OrderBookManager manager{depth};

	auto opt = MessageReaderWriter::ReadNext(std::cin);
	for (; opt.has_value(); opt = MessageReaderWriter::ReadNext(std::cin))
	{
		auto& mh = opt.value();
		std::string str;
		std::string ticker;

		// This variable signals whether we should print out the order book for this ticker
		// -- we print out the orderbook if one of the first `depth` levels on either the bid or the ask side
		// 	  was impacted by the message we just processed
		bool printOrderBook = false;

		switch(mh.header.msg_type)
		{
		case EventType::ADD:
			printOrderBook = manager.handleEntry(std::any_cast<OrderAdd>(mh.msg));
			ticker = std::string(std::any_cast<OrderAdd>(mh.msg).symbol, 3);
			str = std::any_cast<OrderAdd>(mh.msg).String();
			break;
		case EventType::UPDATE:
			printOrderBook = manager.handleEntry(std::any_cast<OrderUpdate>(mh.msg));
			ticker = std::string(std::any_cast<OrderUpdate>(mh.msg).symbol, 3);
			str = std::any_cast<OrderUpdate>(mh.msg).String();
			break;
		case EventType::DELETE:
			printOrderBook = manager.handleEntry(std::any_cast<OrderDelete>(mh.msg));
			ticker = std::string(std::any_cast<OrderDelete>(mh.msg).symbol, 3);
			str = std::any_cast<OrderDelete>(mh.msg).String();
			break;
		case EventType::TRADED:
			printOrderBook = manager.handleEntry(std::any_cast<OrderTrade>(mh.msg));
			ticker = std::string(std::any_cast<OrderTrade>(mh.msg).symbol, 3);
			str = std::any_cast<OrderTrade>(mh.msg).String();
			break;
		default:
			str = "UNKNOWN MESSAGE";
		}

		if (debug) {
			std::cerr << mh.header.seq_num << "/" << static_cast<char>(mh.header.msg_type) << "/" << str << std::endl;
		}
		if (printOrderBook)
		{
			std::cout << mh.header.seq_num << ", " << manager.outputTickerString(ticker, true) << std::endl;
		}
	}

	return 0;
}
