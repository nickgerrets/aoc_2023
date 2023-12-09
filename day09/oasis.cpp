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

seq_t calculate_differences(seq_t const& sequence) {
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

int64_t extrapolate_next(seq_t const& sequence) {
	seq_t seq = calculate_differences(sequence);
	if (seq.back() == 0) {
		return sequence.back();
	}
	return extrapolate_next(seq) + sequence.back();
}

int64_t extrapolate_previous(seq_t const& sequence) {
	seq_t seq = calculate_differences(sequence);
	if (seq.back() == 0) {
		return sequence[0];
	}
	return sequence[0] - extrapolate_previous(seq);
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto sequences = parse_sequences(*input);

	std::cout << "Sum of extrapolated next values:     "
		<< aoc::sum<int64_t>(sequences, extrapolate_next)
		<< std::endl;
	std::cout << "Sum of extrapolated previous values: "
		<< aoc::sum<int64_t>(sequences, extrapolate_previous)
		<< std::endl;

	return (EXIT_SUCCESS);
}
