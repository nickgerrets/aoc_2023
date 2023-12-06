#include "common.h"

#include <set>
#include <vector>
#include <sstream>

struct Scratchcard {
	uint64_t amount;

	// Arrays (vectors) would've probably done just as well,
	// but sets make it easier to match and the numbers are unique
	std::set<uint64_t> owned;
	std::set<uint64_t> winning;

	uint64_t calculate_matches() const {
		uint64_t matches = 0;
		// Simply check the owned set against the winning set to get your matches
		for (uint64_t const& o : owned) {
			matches += winning.count(o);
		}
		return matches;
	}

	uint64_t get_amount() const {
		return amount;
	}

	uint64_t calculate_value() const {
		// bitshift power-of-two
		return (uint64_t(1) << calculate_matches()) >> 1;
	}
};

std::vector<Scratchcard> parse_cards(std::istream& stream) {
	std::vector<Scratchcard> cards;
	for (auto& line : aoc::Lines(stream)) {
		std::istringstream ss(line);

		Scratchcard card;
		card.amount = 1;

		std::string word;
		// skip "Card x"
		ss >> aoc::next_digit >> word;

		while (ss >> word && word[0] != '|') {
			card.winning.emplace(std::stoull(word));
		}
		while (ss >> word) {
			card.owned.emplace(std::stoull(word));
		}
		cards.emplace_back(std::move(card));
	}
	return cards;
}

void process_copies(std::vector<Scratchcard>& cards) {
	for (size_t i = 0; i < cards.size(); ++i) {
		// The way you process how many copies you get is quite simple.
		// You just add your own card amount to the amount of the next x cards
		// where x is how many numbers match.
		uint64_t matches = cards[i].calculate_matches();
		for (size_t j = i + 1; j < cards.size() && j <= i + matches; ++j) {
			cards[j].amount = cards[j].amount + cards[i].amount;
		}
	}
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	// While for part 1 you really don't need to store the parsed cards,
	// part 2 becomes a lot easier if you do.
	std::vector<Scratchcard> cards = parse_cards(*input);

	std::cout << "(Part 1) Sum of scratchcard values:  "
		<< aoc::sum<uint64_t>(cards, &Scratchcard::calculate_value)
		<< std::endl;

	process_copies(cards);
	std::cout << "(Part 2) Sum of scratchcard amounts: "
		<< aoc::sum<uint64_t>(cards, &Scratchcard::get_amount)
		<< std::endl;

	return (EXIT_SUCCESS);
}
