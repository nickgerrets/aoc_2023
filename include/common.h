#ifndef COMMON_H
# define COMMON_H

# include <fstream>
# include <istream>
# include <iostream>
# include <limits>

namespace aoc {

template <typename T> constexpr
int sign(T const& x) {
	return int((x > 0) - (x < 0));
}

// ignore stream until next digit
std::istream& next_digit(std::istream& in);

//	skips until (and including) next CHARACTER
template <char CHARACTER>
std::istream& skip(std::istream& in);

// return input file stream of input file or throw exception
std::ifstream get_input_file(int argc, char** argv);



// Line Iterator for iterating through lines in a (file)stream
struct LineIterator {
	using iterator_category = std::input_iterator_tag;
	using value_type = std::string;
	using difference_type = std::ptrdiff_t;
	using reference = value_type const&;
	using pointer = value_type const*;

	LineIterator();
	LineIterator(std::istream& in);

	reference operator*() const;
	pointer operator->() const;

	LineIterator& operator++();
	LineIterator& operator++(int);

	bool operator==(LineIterator const& rhs);
	bool operator!=(LineIterator const& rhs);

	private:
	std::istream* input_stream;
	std::string stored_str;
};

// handy for range-based loops
struct Lines {
	Lines(std::istream& in);
	LineIterator begin() const;
	LineIterator end() const;

	private:
	std::istream& input_stream;
};

} // namespace aoc

#endif // COMMON_H
