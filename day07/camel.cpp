#include "common.h"

#include <map>
#include <set>
#include <vector>
#include <sstream>

static std::map<char, uint64_t> const PART1_VALUE_MAP = {
	{'A', 0},
	{'K', 1},
	{'Q', 2},
	{'J', 3},
	{'T', 4},
	{'9', 5},
	{'8', 6},
	{'7', 7},
	{'6', 8},
	{'5', 9},
	{'4', 10},
	{'3', 11},
	{'2', 12}
};

static std::map<char, uint64_t> const PART2_VALUE_MAP = {
	{'A', 0},
	{'K', 1},
	{'Q', 2},
	{'T', 4},
	{'9', 5},
	{'8', 6},
	{'7', 7},
	{'6', 8},
	{'5', 9},
	{'4', 10},
	{'3', 11},
	{'2', 12},
	{'J', 13} // Joker now weakest value
};

static std::map<char, uint64_t> const* CARD_VALUE_MAP = &PART1_VALUE_MAP;

enum class Type {
	FIVE_OF_A_KIND = 0,
	FOUR_OF_A_KIND,
	FULL_HOUSE,
	THREE_OF_A_KIND,
	TWO_PAIR,
	ONE_PAIR,
	HIGH_CARD
};

template <bool PART2>
Type get_hand_type(std::string const& str) {
	std::multiset<char> as_set(str.begin(), str.end());

	uint64_t jokers = 0;

	std::map<char, size_t> as_map;
	for (auto const& e : as_set) {
		if (PART2 && e == 'J') {
			jokers = as_set.count(e);
			continue ;
		}
		as_map.insert({e, as_set.count(e)});
	}
	std::vector<size_t> counts;
	for (auto const& e : as_map) {
		counts.push_back(e.second);
	}
	std::sort(counts.rbegin(), counts.rend());

	if (PART2) {
		if (counts.size() == 0) { // case where only jokers
			return Type::FIVE_OF_A_KIND;
		}
		counts[0] += jokers; // factor in joker amount
	}

	switch (counts[0]) {
		case 5: return Type::FIVE_OF_A_KIND;
		case 4: return Type::FOUR_OF_A_KIND;
		case 3:
			if (counts[1] == 2) {
				return Type::FULL_HOUSE;
			}
			return Type::THREE_OF_A_KIND;
		case 2:
			if (counts[1] == 2) {
				return Type::TWO_PAIR;
			}
			return Type::ONE_PAIR;
	}
	return (Type::HIGH_CARD);
}

struct Hand {
	uint64_t bid;
	std::string cards;
	Type type;

	bool operator<(Hand const& rhs) const {
		if (type == rhs.type) {
			for (size_t i = 0; i < cards.length(); ++i) {
				int64_t diff = CARD_VALUE_MAP->at(cards[i]) - CARD_VALUE_MAP->at(rhs.cards[i]);
				if (diff < 0) {
					return true;
				// Was stuck on this for 20 minutes because I put >=
				} else if (diff > 0) {
					return false;
				}
			}
			return false;
		}
		return type < rhs.type;
	}
};

std::vector<Hand> parse_hands(std::istream& stream) {
	std::vector<Hand> hands;
	for (auto& line : aoc::Lines(stream)) {
		Hand h;

		std::istringstream ss(line);
		ss >> h.cards >> h.bid;
		h.type = get_hand_type<false>(h.cards);
		hands.push_back(h);
	}
	return hands;
}

uint64_t sum_winnings(std::vector<Hand> const& hands) {
	uint64_t i = 0;
	uint64_t sum = 0;
	for (auto const& h : hands) {
		uint64_t rank = hands.size() - i;
		sum += h.bid * rank;
		++i;
	}
	return sum;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto hands = parse_hands(*input);
	std::sort(hands.begin(), hands.end());

	std::cout << "(Part1) Sum of winnings: " << sum_winnings(hands) << std::endl;

	// PART 2
	CARD_VALUE_MAP = &PART2_VALUE_MAP;
	for (auto& h : hands) {
		h.type = get_hand_type<true>(h.cards);
	}
	std::sort(hands.begin(), hands.end());
	std::cout << "(Part2) Sum of winnings: " << sum_winnings(hands) << std::endl;

	return (EXIT_SUCCESS);
}
