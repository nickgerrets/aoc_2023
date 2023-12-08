#include "common.h"

#include <vector>
#include <unordered_map>
#include <sstream>

// LOCALE to filter input
static auto const LOCALE = aoc::create_delimitor_locale<'=', '(', ',', ')'>();

struct Node {
	std::string left;
	std::string right;
};

std::string parse_instructions(std::istream& stream) {
	std::string str; std::getline(stream, str); // just grab first line
	return str;
}

using map_t = std::unordered_map<std::string, Node>;

map_t parse_nodes(std::istream& stream) {
	map_t nodes;
	for (auto& line : aoc::Lines(stream)) {
		if (line.length() == 0) {
			continue;
		}

		std::istringstream ss(line); ss.imbue(LOCALE);

		Node node; std::string key;
		ss >> key >> node.left >> node.right;

		nodes.insert({key, node});
	}
	return nodes;
}

using iter_t = map_t::const_iterator;

uint64_t solve_single(map_t const& nodes, std::string const& instructions, iter_t start_it) {
	static char const END_CHAR = 'Z';
	uint64_t count = 0;

	// Simple enough, keep hopping through nodes until you find xxZ
	auto it_current = start_it;
	while (it_current->first.back() != END_CHAR) {
		for (auto const& c : instructions) {
			++count;
			if (c == 'L') {
				it_current = nodes.find(it_current->second.left);
			} else if (c == 'R') {
				it_current = nodes.find(it_current->second.right);
			}
			if (it_current->first.back() == END_CHAR) {
				return count;
			}
		}
	}
	return count;
}

template<typename T>
T least_common_multiple(T a, T b)
{
	// first find greatest common devisor
	T gcd = std::min<T>(a, b);
	while (gcd > 0)
	{
		if (a % gcd == 0 && b % gcd == 0) break ;
		--gcd;
	}
	return ((a / gcd) * b);
}

std::vector<iter_t> get_keys_end_with(map_t const& nodes, char c) {
	std::vector<iter_t> iterators;

	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		if (it->first.back() == c) {
			iterators.push_back(it);
		}
	}
	return iterators;
}

uint64_t solve_lcm(map_t const& nodes, std::string const& instructions) {
	std::vector<iter_t> iterators = get_keys_end_with(nodes, 'A');
	uint64_t lcm = 1;

	/* Least Common Multiple Approach
		The amount of steps it takes for ALL nodes to reach xxZ simultaniously
		is equal to the Least Common Multiple of ALL nodes.

		I go through each node one by one to see how many steps it takes to
		reach xxZ and then take the LCM of those steps.
	*/

	for (auto& it : iterators) {
		lcm = least_common_multiple(lcm, solve_single(nodes, instructions, it));
	}
	return lcm;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto instructions = parse_instructions(*input);
	auto nodes = parse_nodes(*input);

	std::cout << "(Part 1) Steps required for node AAA to reach ZZZ:  "
		<< solve_single(nodes, instructions, nodes.find("AAA"))
		<< std::endl;

	std::cout << "(Part 2) Steps required for all nodes to reach xxZ: "
		<< solve_lcm(nodes, instructions)
		<< std::endl;

	return (EXIT_SUCCESS);
}