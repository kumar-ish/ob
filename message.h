#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <any>
#include <optional>

enum class EventType : uint8_t
{
	UNKNOWN = '\0',
	ADD = 'A',
	UPDATE = 'U',
	DELETE = 'D',
	TRADED = 'E',
};

enum class Side : uint8_t
{
	UNKNOWN = '\0',
	BUY = 'B',
	SELL = 'S',
};

#pragma pack(push, 1)
struct  Header
{
	int32_t seq_num;
	int32_t msg_size;
	EventType msg_type;

	std::string String()
	{
		return std::to_string(seq_num) + "/" + static_cast<char>(msg_type);
	}

};

struct OrderAdd
{
	char symbol[3];
	uint64_t order_id;
	Side side;
	int8_t _reserved1[3];
	uint64_t size;
	int32_t price;
	int8_t _reserved2[4];

	std::string String()
	{
		return "{'symbol': '" + std::string(symbol, 3) + "', 'order_id': " + std::to_string(order_id) + ", 'side': '" + static_cast<char>(side) + "', " +
				+ "'size': " + std::to_string(size) + ", 'price': " + std::to_string(price) + "}";
	}
};

struct OrderUpdate
{
	char symbol[3];
	uint64_t order_id;
	Side side;
	int8_t _reserved1[3];
	uint64_t size;
	int32_t price;
	int8_t _reserved2[4];

	std::string String()
	{
		return "{'symbol': '" + std::string(symbol, 3) + "', 'order_id': " + std::to_string(order_id) + ", 'side': '" + static_cast<char>(side) + "', " +
				+ "'size': " + std::to_string(size) + ", 'price': " + std::to_string(price) + "}";
	}
};

struct OrderDelete
{
	char symbol[3];
	uint64_t order_id;
	Side side;
	int8_t _reserved1[3];

	std::string String()
	{
		return "{'symbol': '" + std::string(symbol, 3) + "', 'order_id': " + std::to_string(order_id) + ", 'side': '" + static_cast<char>(side) + "'}";
	}
};

struct OrderTrade
{
	char symbol[3];
	uint64_t order_id;
	Side side;
	int8_t _reserved1[3];
	uint64_t volume;

	std::string String()
	{
		return "{'symbol': '" + std::string(symbol, 3) + "', 'order_id': " + std::to_string(order_id) + ", 'side': '" + static_cast<char>(side) + "', 'volume': " + std::to_string(volume) + "}";
	}
};
#pragma pack(pop) // pack(push, 1)

struct MsgWithHeader
{
	Header header;
	std::any msg;
};

class MessageReaderWriter
{
public:
	static std::optional<MsgWithHeader> ReadNext(std::istream& s)
	{
		if (s.eof())
			return std::nullopt;

		MsgWithHeader msg;
		msg.header = ReadStruct<Header>(s);
		if (s.bad())
			return std::nullopt;

		switch (msg.header.msg_type)
		{
		case EventType::ADD:
			msg.msg = ReadStruct<OrderAdd>(s);
			break;
		case EventType::UPDATE:
			msg.msg = ReadStruct<OrderUpdate>(s);
			break;
		case EventType::DELETE:
			msg.msg = ReadStruct<OrderDelete>(s);
			break;
		case EventType::TRADED:
			msg.msg = ReadStruct<OrderTrade>(s);
			break;
		default:
			return std::nullopt;
		}

		if (s.bad())
			return std::nullopt;

		return msg;
	}

	static void Write(std::ostream& s, const MsgWithHeader& msg)
	{
		WriteStruct(s, msg.header);
		switch (msg.header.msg_type)
		{
		case EventType::ADD:
			WriteStruct(s, std::any_cast<OrderAdd>(msg.msg));
			break;
		case EventType::UPDATE:
			WriteStruct(s, std::any_cast<OrderUpdate>(msg.msg));
			break;
		case EventType::DELETE:
			WriteStruct(s, std::any_cast<OrderDelete>(msg.msg));
			break;
		case EventType::TRADED:
			WriteStruct(s, std::any_cast<OrderTrade>(msg.msg));
			break;
		default:
			break;
		}
	}
private:
	template<typename T>
	static T ReadStruct(std::istream& s)
	{
		T t;
		s.read(reinterpret_cast<char*>(&t), sizeof(T));
		return t;
	}

	template<typename T>
	static void WriteStruct(std::ostream& s, const T& t)
	{
		s.write(reinterpret_cast<const char*>(&t), sizeof(T));
	}
};

