#include "common.h"

#include <vector>
#include <sstream>

struct Game {
	uint64_t red;
	uint64_t green;
	uint64_t blue;

	uint64_t* get_color(std::string const& str) {
		if (str.find("red") != std::string::npos) {
			return &red;
		} else if (str.find("green") != std::string::npos) {
			return &green;
		} else if (str.find("blue") != std::string::npos) {
			return &blue;
		}
		return nullptr;
	}

	uint64_t power(void) const {
		return red * green * blue;
	}
};

std::vector<Game> parse_games(std::istream& stream) {
	std::vector<Game> games;
	for (auto line : aoc::Lines(stream)) {
		std::istringstream ss(line);
		Game game {0, 0, 0};
		uint64_t x;
		std::string color;

		// Skip "Game x"
		ss >> aoc::next_digit >> x;

		// While we have a digit-color pair
		while (ss >> aoc::next_digit >> x >> color) {
			// get a ptr to the corresponding color-count integer within current game
			auto* c = game.get_color(color);
			if (!c) {
				throw std::runtime_error("bad color");
			}
			// Assign the color if it's the maximum
			*c = std::max(*c, x);
		}
		games.push_back(game);
	}
	return games;
}

uint64_t sum_games_id(std::vector<Game> const& games) {
	const Game max_game {
		.red = 12,
		.green = 13,
		.blue = 14
	};

	uint64_t sum = 0;
	for (size_t i = 1; i <= games.size(); ++i) {
		// Check impossible
		if (games[i - 1].red > max_game.red ||
			games[i - 1].green > max_game.green ||
			games[i - 1].blue > max_game.blue) {
				continue;
		}
		sum += i;
	}
	return sum;
}

uint64_t sum_games_power(std::vector<Game> const& games) {
	uint64_t power = 0;
	for (auto const& game : games) {
		power += game.power();
	}
	return power;
}

int main(int argc, char** argv) {
	auto file = aoc::get_input_file(argc, argv);

	std::vector<Game> games = parse_games(file);

	std::cout << "Sum of IDs of games: " << sum_games_id(games) << std::endl;
	std::cout << "Sum of power of games: " << sum_games_power(games) << std::endl;

	return (EXIT_SUCCESS);
}
