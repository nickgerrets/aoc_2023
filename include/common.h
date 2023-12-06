#ifndef COMMON_H
# define COMMON_H

# include <fstream>
# include <istream>
# include <iostream>
# include <limits>
# include <algorithm>

namespace aoc {

template <typename T> constexpr
int sign(T const& x) {
	return int((x > 0) - (x < 0));
}

// ignore stream until next digit
std::istream& next_digit(std::istream& in);

//	skips until (and including) next CHARACTER
template <char CHARACTER = ' '>
std::istream& skip(std::istream& in) {
	return (in.ignore(std::numeric_limits<std::streamsize>::max(), CHARACTER));
}

// return input file stream of input file or throw exception
std::ifstream get_input_file(int argc, char** argv);


struct InputDelete {
	void operator()(std::istream* p) const {
		if (&std::cin == p) {
			return ;
		}
		delete p;
	}
};

std::unique_ptr<std::istream, InputDelete> get_input(int argc, char** argv);

/* -------------------------------------------------------------------------- */
/*                                 Aggregates                                 */
/* -------------------------------------------------------------------------- */
template <typename T, typename ContainerT>
T sum(ContainerT const& container) {
	T n = 0;
	for (auto const& e : container) {
		n = n + e;
	}
	return n;
}

template <typename T, typename ContainerT, typename _Method>
T sum(ContainerT const& container, _Method method) {
	T n = 0;
	for (auto const& e : container) {
		n = n + (e.*method)();
	}
	return n;
}

template <typename T, typename ContainerT>
T product(ContainerT const& container) {
	T n = 1;
	for (auto const& e : container) {
		n = n * e;
	}
	return n;
}

template <typename T, typename ContainerT, typename _Method>
T product(ContainerT const& container, _Method method) {
	T n = 1;
	for (auto const& e : container) {
		n = n * (e.*method)();
	}
	return n;
}

/* -------------------------------------------------------------------------- */
/*                                Line Iterator                               */
/* -------------------------------------------------------------------------- */
// for iterating through lines in a (file)stream
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

/* -------------------------------------------------------------------------- */
/*                                   Locale                                   */
/* -------------------------------------------------------------------------- */
// custom facets for locales, allowing easier parsing through streams
template <char C, char... Args>
struct DelimitorFacet : public std::ctype<char> {
	DelimitorFacet(size_t refs = 0) : std::ctype<char>(custom_table, false, refs) {
		std::copy_n(classic_table(), table_size, custom_table);
		set_characters<C, Args...>();
	}

	private:
	// Variadic unpacking base case
	template <char _First>
	void set_characters(void) {
		custom_table[_First] = space;
	}

	// Variadic unpacking recurse 
	template <char _First, char _Next, char... _Rest>
	void set_characters(void) {
		set_characters<_Next, _Rest...>();
		custom_table[_First] = space;
	}

	mask custom_table[table_size];
};

// Helper to create a delimitor locale
template <char C, char... Cs>
std::locale create_delimitor_locale(void) {
	return std::locale(std::locale::classic(), new DelimitorFacet<C, Cs...>);
}

} // namespace aoc

#endif // COMMON_H
