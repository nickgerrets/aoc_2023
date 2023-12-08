#include "common.h"

#include <vector>
#include <unordered_map>
#include <sstream>

static auto const LOCALE = aoc::create_delimitor_locale<'=', '(', ',', ')'>();

struct Node {
	std::string left;
	std::string right;
};

std::string parse_instructions(std::istream& stream) {
	std::string str;

	std::getline(stream, str);
	return str;
}

std::unordered_map<std::string, Node> parse_nodes(std::istream& stream) {
	std::unordered_map<std::string, Node> nodes;
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

uint64_t solve_single(std::unordered_map<std::string, Node> const& nodes, std::string const& instructions) {
	uint64_t count = 0;

	auto it_current = nodes.find("AAA");
	while (it_current->first != "ZZZ") {
		for (auto const& c : instructions) {
			++count;
			if (c == 'L') {
				it_current = nodes.find(it_current->second.left);
			} else if (c == 'R') {
				it_current = nodes.find(it_current->second.right);
			}
			if (it_current->first == "ZZZ") {
				return count;
			}
		}
	}
	return count;
}

using iter_t = std::unordered_map<std::string, Node>::const_iterator;
std::vector<iter_t> get_keys_end_with(std::unordered_map<std::string, Node> const& nodes, char c) {
	std::vector<iter_t> iterators;

	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		if (it->first.back() == c) {
			iterators.push_back(it);
		}
	}
	return iterators;
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

uint64_t solve_lcm(std::unordered_map<std::string, Node> const& nodes, std::string const& instructions) {
	std::vector<iter_t> iterators = get_keys_end_with(nodes, 'A');
	uint64_t lcm = 1;

	/* Least Common Multiple Approach
		The amount of steps it takes for ALL nodes to reach xxZ simultaniously
		is equal to the Least Common Multiple of ALL nodes.

		I go through each node one by one to see how many steps it takes to
		reach xxZ and then take the LCM of those steps.
	*/

	for (auto& it : iterators) {
		uint64_t count = 0;
		bool go = true;
		while (go) {
			go = true;
			for (auto const& c : instructions) {
				++count;
				if (c == 'L') {
					it = nodes.find(it->second.left);
				} else if (c == 'R') {
					it = nodes.find(it->second.right);
				}
				if (it->first.back() == 'Z') {
					lcm = least_common_multiple(lcm, count);
					go = false;
					break;
				}
			}
		}
	}
	return lcm;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto instructions = parse_instructions(*input);
	auto nodes = parse_nodes(*input);

	std::cout << "Steps required for node AAA to reach ZZZ:  " << solve_single(nodes, instructions) << std::endl;
	std::cout << "Steps required for all nodes to reach xxZ: " << solve_lcm(nodes, instructions) << std::endl;

	return (EXIT_SUCCESS);
}