#include <iostream>
#include <fstream>

#include "../message.h"

void deserializeAndReserialize(std::istream& in, std::ostream& out) {
    auto opt = MessageReaderWriter::ReadNext(in);
	for (; opt.has_value(); opt = MessageReaderWriter::ReadNext(in))
	{
		auto& mh = opt.value();

        MessageReaderWriter::Write(out, mh);
	}
}

int main() {
    // We read in input1 and input2 -- getting the deserialized, in-memory version of it -- 
    // and then serialize it back again to input1.stream.test and input2.stream.test.
    // That way, we can verify that the serialization process works (empirically) and we can
    // make our test cases in-memory.
    std::ifstream input1("input1.stream");
    std::ofstream output1("input1.test.stream");
    std::ifstream input2("input2.stream");
    std::ofstream output2("input2.test.stream");

    deserializeAndReserialize(input1, output1);
    deserializeAndReserialize(input2, output2);
}