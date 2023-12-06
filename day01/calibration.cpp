#include "common.h"

#include <unordered_map>
#include <numeric>

// This map will help connect a value to the words and will also provide
// a handy 'list' to iterate through
static std::unordered_map<std::string, uint64_t> digit_map {
	{"zero", 0},
	{"one", 1},
	{"two", 2},
	{"three", 3},
	{"four", 4},
	{"five", 5},
	{"six", 6},
	{"seven", 7},
	{"eight", 8},
	{"nine", 9},
};

uint64_t get_first_digit(std::string const& line) {
	uint64_t digit = 0;
	size_t pos = std::string::npos;

	// first occurence of an actual digit
	for (size_t i = 0; i < line.length(); ++i) {
		if (std::isdigit(line[i])) {
			digit = line[i] - '0';
			pos = i;
			break ;
		}
	}

	// first occurence of a word that represents a digit
	// taking into account it needs to have a lower position than the digit did
	for (auto const& pair : digit_map) {
		size_t found = line.find(pair.first);
		if (found < pos) {
			pos = found;
			digit = pair.second;
		}
	}
	return digit;
}

uint64_t get_last_digit(std::string const& line) {
	// Very similar to get_first_digit, just reversed
	uint64_t digit = 0;
	size_t pos = 0;

	// the length + 1 and [i - 1] are to make sure we don't have to make the conditon 'i >= 0'
	// which would cause an underflow and an infinite loop
	for (size_t i = line.length() + 1; i > 0; --i) {
		if (std::isdigit(line[i - 1])) {
			digit = line[i - 1] - '0';
			pos = i - 1;
			break ;
		}
	}

	// simply use rfind instread of find and '>' instead of '<'
	// did have to specifically exclude npos, because that's always higher and returned on 'not found'
	for (auto const& pair : digit_map) {
		size_t found = line.rfind(pair.first);
		if (found != std::string::npos && found > pos) {
			pos = found;
			digit = pair.second;
		}
	}
	return digit;
}

uint64_t get_sum(std::istream& stream) {
	// Iterate through the entire document line by line and sum up
	// their calibration values

#if (0)
	// A relatively simple way to iterate over lines in a stream
	uint64_t sum = 0;
	for (std::string line; std::getline(stream, line); ) {
		sum += get_first_digit(line) * 10 + get_last_digit(line);
	}
	return sum;
#elif (0)
	// A range-based approach with a custom iterator to iterate of the lines
	// this approach is more "C++"-esque
	uint64_t sum = 0;
	for (auto& line : aoc::Lines(stream)) {
		sum += get_first_digit(line) * 10 + get_last_digit(line);
	}
	return sum;
#else
	// Arguably the most "C++"-esque approach.
	// Custom iterator for lines.
	// A lamba to define how to sum the values.
	// And the use of the STL function std::accumulate.
	// However, this is imo quite overkill for simple problems.
	auto add = [](uint64_t n, std::string line) {
		return n + get_first_digit(line) * 10 + get_last_digit(line);
	};

	aoc::Lines l(stream);
	return std::accumulate(l.begin(), l.end(), uint64_t(0), add);
#endif
}

int	main(int argc, char **argv) {
	auto input = aoc::get_input(argc, argv);
	uint64_t sum = get_sum(*input);

	std::cout << "Sum of all calibration values: " << sum << std::endl;
	
	return (EXIT_SUCCESS);
}
