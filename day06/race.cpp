#include "common.h"

#include <vector>
#include <sstream>

struct Race {
	uint64_t time;
	uint64_t distance;

	Race() : time(0), distance(0) {}
	Race(uint64_t time, uint64_t distance) : time(time), distance(distance) {}

	uint64_t ways_to_beat_iterative() const {
		// Naive iterative way - My initial solution
		// I spend way too much time on attempting a quadratic solution until I
		// realised that the iterative is more than fast enough for a quick answer
		uint64_t count = 0;
		for (size_t speed = 0; speed < time; ++speed) {
			uint64_t time_left = time - speed;
			if (time_left * speed > distance) {
				++count;
			}
		}
		return count;
	}

	uint64_t ways_to_beat() const {
		/* Mathematical Solution
			Quadratic function for distance:
				f(x) = x^2 - tx
				Where t = max time and x is waiting time
			The maximum distance traveled can be gotten from the derivative:
				f'(x) = 2x - t
			And then set it to zero and solve for x:
				2x - t = 0;
				x = t / 2;
			Combine back into a function:
				f(t / 2) = (t / 2)^2 - t(t / 2)

			Now factor in the distance (d) we have to beat for our actual solution:
				f(x) = x^2 - tx + d
			If this function equates to 0, then x will equal to the waiting time of
			the person who set that distance.
				f(x) = x^2 - tx + d = 0
			We can find out the two possible values of x through the roots:
				x = 1/2 (t - sqrt(t^2 - 4 d))
				x = 1/2 (t + sqrt(t^2 - 4 d))
		*/

		double t = time;
		double d = distance;

		double sq = std::sqrt( t * t - 4.0 * d);
		double x1 = (t - sq) / 2.0;
		double x2 = (t + sq) / 2.0;

		/* Between these values x1 and x2, we will beat the distance
			example:
				for t = 7 and d = 9 (example on AOC):
				x1 will be 1.69;
				x2 will be 5.30;
				and the solution will be:
				ceil(x2) = 6;
				floor(x1) = 1;
				6 - 1 = 5;
				Now correct for rounding up x2
				5 - 1 = 4;
		*/
		return (std::ceil(x2) - std::floor(x1) - 1);
	}
};

// I personally usually prefer this over a std::pair
using result_t = struct {
	std::vector<Race> races;
	Race big_race;
};
result_t parse_races(std::istream& stream) {
	result_t result;
	std::string n;
	std::string concat;

	auto lines = aoc::Lines(stream).begin();
	// The first line is time
	std::stringstream ss(*lines);
	while (ss >> aoc::next_digit >> n) {
		result.races.emplace_back(std::stoull(n), 0);
		concat += n;
	}
	result.big_race.time = std::stoull(concat);
	concat.clear();

	// Second line is distance
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
	auto input = aoc::get_input(argc, argv);

	auto result = parse_races(*input);

	std::cout << "(Part 1) Product of ways to beat: " << product(result.races) << std::endl;
	std::cout << "(Part 2) ways to beat big race:   " << result.big_race.ways_to_beat() << std::endl;

	return (EXIT_SUCCESS);
}
