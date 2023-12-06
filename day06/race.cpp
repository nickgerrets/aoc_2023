#include "common.h"

#include <vector>
#include <sstream>

struct Race {
	uint64_t time;
	uint64_t distance;

	Race() : time(0), distance(0) {}
	Race(uint64_t time, uint64_t distance) : time(time), distance(distance) {}

	uint64_t ways_to_beat() const {
		uint64_t count = 0;
		for (size_t speed = 0; speed < time; ++speed) {
			uint64_t time_left = time - speed;
			if (time_left * speed > distance) {
				++count;
			}
		}
		return count;
	}
};

struct Result {
	std::vector<Race> races;
	Race big_race;
};

Result parse_races(std::istream& stream) {
	Result result;

	auto lines = aoc::Lines(stream).begin();

	std::stringstream ss(*lines);
	std::string n;
	std::string concat;
	while (ss >> aoc::next_digit >> n) {
		result.races.emplace_back(std::stoull(n), 0);
		concat += n;
	}
	result.big_race.time = std::stoull(concat);
	concat.clear();

	++lines;
	ss = std::stringstream(*lines);
	for (auto& r : result.races) {
		ss >>  aoc::next_digit >> n;
		r.distance = std::stoull(n);
		concat += n;
	}
	result.big_race.distance = std::stoull(concat);

	return result;
}

uint64_t product(std::vector<Race> const& races) {
	uint64_t product = 1;
	for (auto& r : races) {
		product *= r.ways_to_beat();
	}
	return product;
}

int main(int argc, char** argv) {
	auto file = aoc::get_input_file(argc, argv);

	auto result = parse_races(file);

	std::cout << "(Part 1) Product of ways to beat: " << product(result.races) << std::endl;
	std::cout << "(Part 2) ways to beat big race: " << result.big_race.ways_to_beat() << std::endl;

	return (EXIT_SUCCESS);
}
