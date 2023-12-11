#ifndef COMMON_H
# define COMMON_H

# include <fstream>
# include <istream>
# include <iostream>
# include <limits>
# include <algorithm>
# include <memory>
# include <cassert>

namespace aoc {

/* -------------------------------------------------------------------------- */
/*                                Math Helpers                                */
/* -------------------------------------------------------------------------- */

template <typename T> constexpr
int sign(T const& x) {
	return int((x > 0) - (x < 0));
}

template <typename T>
T greatest_common_devisor(T a, T b) {
	static_assert(std::is_integral<T>(), "Type has to be integral");
	T gcd = std::min<T>(a, b);
	while (gcd > 0) {
		if (a % gcd == 0 && b % gcd == 0) {
			break ;
		}
		--gcd;
	}
	return gcd;
}

template <typename T>
T least_common_multiple(T a, T b) {
	static_assert(std::is_integral<T>(), "Type has to be integral");
	// first find greatest common devisor
	T gcd = greatest_common_devisor(a, b);
	return ((a / gcd) * b);
}

/* -------------------------------------------------------------------------- */
/*                               Stream Helpers                               */
/* -------------------------------------------------------------------------- */

// ignore stream until next digit (exclusive)
std::istream& next_digit(std::istream& in);

//	skips until next CHARACTER (inclusive)
template <char CHARACTER = '\n'>
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

// return input stream of input file if there's a file or stdin if there's none
std::unique_ptr<std::istream, InputDelete> get_input(int argc, char** argv);

/* -------------------------------------------------------------------------- */
/*                                 Aggregates                                 */
/* -------------------------------------------------------------------------- */
template <typename T = int64_t, typename ContainerT>
T sum(ContainerT const& container) {
	T n = 0;
	for (auto const& e : container) {
		n = n + e;
	}
	return n;
}

template <typename T = int64_t, typename ContainerT, typename _Method>
typename std::enable_if<
	std::is_member_function_pointer<_Method>::value, // case is a member function (method)
	T>::type
sum(ContainerT const& container, _Method method) {
	T n = 0;
	for (auto const& e : container) {
		n = n + (e.*method)();
	}
	return n;
}

template <typename T = int64_t, typename ContainerT, typename _Function>
typename std::enable_if<
	!std::is_member_function_pointer<_Function>::value, // case not a member function
	T>::type
sum(ContainerT const& container, _Function function) {
	T n = 0;
	for (auto const& e : container) {
		n = n + function(e);
	}
	return n;
}

template <typename T = int64_t, typename ContainerT>
T product(ContainerT const& container) {
	T n = 1;
	for (auto const& e : container) {
		n = n * e;
	}
	return n;
}

template <typename T = int64_t, typename ContainerT, typename _Method>
typename std::enable_if<
	std::is_member_function_pointer<_Method>::value, // case is a member function (method)
	T>::type
product(ContainerT const& container, _Method method) {
	T n = 1;
	for (auto const& e : container) {
		n = n * (e.*method)();
	}
	return n;
}

template <typename T = int64_t, typename ContainerT, typename _Function>
typename std::enable_if<
	!std::is_member_function_pointer<_Function>::value, // case not a member function
	T>::type
product(ContainerT const& container, _Function function) {
	T n = 1;
	for (auto const& e : container) {
		n = n * function(e);
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
	using parent_t = std::ctype<char>;

	DelimitorFacet(size_t refs = 0) : parent_t(custom_table, false, refs) {
		std::copy_n(parent_t::classic_table(), table_size, custom_table);
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

	parent_t::mask custom_table[table_size];
};

// Helper to create a delimitor locale
template <char C, char... Cs>
std::locale create_delimitor_locale(void) {
	return std::locale(std::locale::classic(), new DelimitorFacet<C, Cs...>);
}

} // namespace aoc

#endif // COMMON_H
