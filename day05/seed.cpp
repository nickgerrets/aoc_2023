#include "common.h"

#include <vector>
#include <vector>
#include <sstream>

struct Range {
	uint64_t begin;
	uint64_t end;

	Range() : begin(0), end(0) {}

	Range(uint64_t begin, uint64_t end) : begin(begin), end(end) {}

	operator bool() const {
		return end > begin;
	}

	bool operator<(Range const& rhs) {
		return begin < rhs.begin;
	}

	bool includes(uint64_t value) const {
		if (value >= begin && value < end) {
			return true;
		}
		return false;
	}

	Range intersect(Range const& other) const {
		return Range(
			std::max(begin, other.begin),
			std::min(end, other.end)
		);
	}
};

struct Entry {
	Range range;
	uint64_t destination;
};

struct Map {
	std::vector<Entry> entries;

	uint64_t process(uint64_t n) const {
		for (auto const& e : entries) {
			if (e.range.includes(n)) {
				return e.destination + n - e.range.begin;
			}
		}
		return n;
	}
};

std::vector<Range> parse_seeds(std::istream& stream) {
	std::vector<Range> seeds;
	auto line = aoc::Lines(stream).begin();

	std::stringstream ss(*line);
	Range seed_range;
	while (ss >> aoc::next_digit >> seed_range.begin >> seed_range.end) {
		seed_range.end += seed_range.begin;
		seeds.push_back(seed_range);
	}
	return seeds;
}

std::vector<Map> parse_maps(std::istream& stream) {
	std::vector<Map> maps;

	Map map;
	stream >> aoc::skip<'\n'> >> aoc::skip<'\n'>;
	for (auto const& line : aoc::Lines(stream)) {
		std::stringstream ss(line);
		if (line.length() == 0) {
			continue;
		}

		// Only push if we are at the next map
		if (!std::isdigit(ss.peek())) {
			maps.push_back(map);
			map = Map();
			continue; // skip
		}

		Entry entry;
		ss >> entry.destination >> entry.range.begin >> entry.range.end;
		entry.range.end += entry.range.begin;
		map.entries.push_back(entry);
	}
	maps.push_back(map);
	return maps;
}

std::vector<uint64_t> calculate_locations(std::vector<Range> const& seeds, std::vector<Map> const& maps) {
	std::vector<uint64_t> locations;
	for (auto const& s : seeds) {
		locations.emplace_back(s.begin);
		locations.emplace_back(s.end - s.begin);
	}

	// This pretty much just traces each individual value to it's end locations
	for (auto const& m : maps) {
		for (auto& l : locations) {
			l = m.process(l);
		}
	}
	return locations;
}

void process_seeds(std::vector<Range>& seeds, std::vector<Map> const& maps) {
	for (auto& m : maps) {
		std::vector<Range> next_seeds;
		while (seeds.size() > 0) {
			Range s(seeds.back()); seeds.pop_back();
			bool used = false;
			for (auto& e : m.entries) {
				Range intersection(e.range.intersect(s));
				if (intersection) {
					// offset and add the intersection to the new seeds
					next_seeds.emplace_back(
						intersection.begin - e.range.begin + e.destination,
						intersection.end - e.range.begin + e.destination
					);
					// Split off the left side of the intersection and process it again
					if (intersection.begin > s.begin) {
						seeds.emplace_back(s.begin, intersection.begin);
					}
					// Split off the right side of the intersection and process it again
					if (s.end > intersection.end) {
						seeds.emplace_back(intersection.end, s.end);
					}
					// The seed has been processed
					used = true;
					break ;
				}
			}
			// There is only need to add the seed-range back into the pool if it has not been processed
			if (!used) {
				next_seeds.push_back(s);
			}
		}
		// next seeds become the new seeds
		seeds.swap(next_seeds);
	}
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	std::vector<Range> seeds = parse_seeds(*input);
	std::vector<Map> maps = parse_maps(*input);

	auto locations = calculate_locations(seeds, maps);
	std::cout << "(Part 1) Lowest location number: " <<
		*std::min_element(locations.begin(), locations.end()) << std::endl;

	process_seeds(seeds, maps);
	std::cout << "(Part 2) Lowest location number: " <<
		std::min_element(seeds.begin(), seeds.end())->begin << std::endl;

	return (EXIT_SUCCESS);
}
