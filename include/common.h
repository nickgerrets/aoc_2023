#ifndef COMMON_H
# define COMMON_H

# include <istream>
# include <limits>

# define SIGN(x) (x > 0) - (x < 0)

namespace aoc {

//	ignore stream until next digit
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

//	skips until (and including) next CHARACTER
template <char CHARACTER>
std::istream& skip(std::istream& in) {
	return (in.ignore(std::numeric_limits<std::streamsize>::max(), CHARACTER));
}

} // namespace aoc

#endif // COMMON_H
