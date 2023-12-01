#include "common.h"

namespace aoc {

std::istream& next_digit(std::istream& in) {
	char c;
	while (in &&
		   (c = (in >> std::ws).peek()) != EOF &&
		   !(c >= '0' && c <= '9') &&
		   c != '-') {
		in.ignore();
	}
	return (in);
}

template <char CHARACTER>
std::istream& skip(std::istream& in) {
	return (in.ignore(std::numeric_limits<std::streamsize>::max(), CHARACTER));
}


std::ifstream get_input_file(int argc, char** argv) {
	if (argc < 2) {
		throw std::runtime_error(std::string("usage: ") + argv[0] + " <file>");
	}

	std::ifstream file(argv[1]);
	if (!file) {
		throw std::runtime_error(std::string("Can't open file \"") + argv[1] + '\"');
	}
	return std::move(file);
}

// === Line Iterator ===
LineIterator::LineIterator() : input_stream(nullptr) {}

LineIterator::LineIterator(std::istream& in) : input_stream(&in) {
	++(*this);
}

LineIterator::reference LineIterator::operator*() const {
	return stored_str;
}

LineIterator::pointer LineIterator::operator->() const {
	return &stored_str;
}

LineIterator& LineIterator::operator++() {
	if (!std::getline(*input_stream, stored_str)) {
		input_stream = nullptr;
	}
	return *this;
}

#pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wreturn-stack-address"
LineIterator& LineIterator::operator++(int) {
	LineIterator copy(*this);
	++(*this);
	return copy;
}
#pragma GCC diagnostic pop

bool LineIterator::operator==(LineIterator const& rhs) {
	return input_stream == rhs.input_stream;
}

bool LineIterator::operator!=(LineIterator const& rhs) {
	return input_stream != rhs.input_stream;
}

Lines::Lines(std::ifstream& in) : input_stream(in) {}

LineIterator Lines::begin() const {
	return LineIterator(input_stream);
}

LineIterator Lines::end() const {
	return LineIterator();
}


} // aoc
