#include "common.h"

#include <vector>
#include <unordered_map>
#include <sstream>
#include <deque>

struct Record {
	std::string row;
	std::vector<uint64_t> groups;
};

std::vector<Record> parse_records(std::istream& stream) {
	std::vector<Record> records;
	for (auto const& line : aoc::Lines(stream)) {
		Record r;

		std::istringstream ss(line);

		ss >> r.row;

		uint64_t n;
		while (ss >> aoc::next_digit >> n) {
			r.groups.push_back(n);
		}
		records.push_back(r);
	}
	return records;
}

void unfold_records(std::vector<Record>& records, size_t repeat) {
	for (auto& r : records) {
		Record new_record;
		for (size_t i = 0; i < repeat; ++i) {
			new_record.row += r.row;
			new_record.groups.insert(new_record.groups.end(), r.groups.begin(), r.groups.end());
			if (i != repeat - 1) {
				new_record.row += '?';
			}
		}
		r.row.swap(new_record.row);
		r.groups.swap(new_record.groups);
	}
}

uint64_t hash(uint64_t i, uint64_t j, uint64_t k) {
	return (i + j * 1e3 + k * 1e6);
}

uint64_t arrangements(std::string const& row, std::vector<uint64_t> const& groups) {
	std::unordered_map<uint64_t, uint64_t> cache;
	cache[hash(0, 0, 0)] = 1; // this hash presumes we don't go above 1000 char length rows

	for (size_t i = 0; i < row.length(); ++i) { // Index within string
		for (size_t g = 0; g < groups.size() + 1; ++g) { // Index within group
			for (size_t c = 0; c < row.length() + 1; ++c) { // contiguous length
				auto it = cache.find(hash(i, g, c));
				if (it == cache.end()) {
					continue;
				}

				bool is_con = (c == 0 || (g > 0 && c == groups[g - 1]));
				switch (row[i]) {
					case '.':
						is_con ? cache[hash(i + 1, g, 0)] += it->second : 0;
						break;
					case '#':
						cache[hash(i + 1, g + !c, c + 1)] += it->second;
						break ;
					case '?':
						is_con ? cache[hash(i + 1, g, 0)] += it->second : 0;
						cache[hash(i + 1, g + !c, c + 1)] += it->second;
						break;
				}
			}
		}
	}
	return cache.at(hash(row.length(), groups.size(), 0));
}

uint64_t solve(std::vector<Record> const& records) {
	uint64_t result = 0;
	for (auto const& r : records) {
		auto row = r.row + '.';
		auto groups = r.groups;

		size_t r_len = row.length();
		size_t g_len = groups.size();

		result += arrangements(row, r.groups);
	}
	return result;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto records = parse_records(*input);

	std::cout << "(Part 1) Sum of arrangements: " << solve(records) << std::endl;

	unfold_records(records, 5);
	std::cout << "(Part 2) Sum of arrangements: " << solve(records) << std::endl;

	return EXIT_SUCCESS;
}
