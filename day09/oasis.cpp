#include "common.h"

#include <vector>
#include <sstream>

using seq_t = std::vector<int64_t>;
using sequences_t = std::vector<seq_t>;

sequences_t parse_sequences(std::istream& input) {
	sequences_t sequences;

	for (auto const& line : aoc::Lines(input)) {
		sequences.emplace_back();
		seq_t& curr_seq = sequences.back();

		std::istringstream ss(line);

		int64_t n ;
		while (ss >> n) {
			curr_seq.push_back(n);
		}
	}
	return sequences;
}

seq_t calculate_difference(seq_t const& sequence) {
	seq_t diff;

	for (auto it = sequence.begin(); it != sequence.end(); ++it) {
		auto it2 = it + 1;

		if (it2 == sequence.end()) {
			return diff;
		}

		diff.push_back(*it2 - *it);
	}
	return diff;
}

int64_t calculate_next(seq_t const& sequence) {
	seq_t seq = calculate_difference(sequence);
	if (seq.back() == 0) {
		return sequence.back();
	}
	return calculate_next(seq) + sequence.back();
}

int64_t calculate_prev(seq_t const& sequence) {
	seq_t seq = calculate_difference(sequence);
	if (seq.back() == 0) {
		return sequence[0];
	}
	return sequence[0] - calculate_prev(seq);
}

int64_t extrapolate_next(sequences_t const& sequences) {
	int64_t sum = 0;
	for (auto const& s : sequences) {
		sum += calculate_next(s);
	}
	return sum;
}

int64_t extrapolate_previous(sequences_t const& sequences) {
	int64_t sum = 0;
	for (auto const& s : sequences) {
		sum += calculate_prev(s);
	}
	return sum;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto sequences = parse_sequences(*input);

	std::cout << "Sum of extrapolated next values:     " << extrapolate_next(sequences) << std::endl;
	std::cout << "Sum of extrapolated previous values: " << extrapolate_previous(sequences) << std::endl;

	return (EXIT_SUCCESS);
}
