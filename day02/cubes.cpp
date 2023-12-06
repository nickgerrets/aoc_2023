#include "common.h"

#include <vector>
#include <sstream>
#include <unordered_map>

// Easier parsing by creating a locale
static auto const LOCALE = aoc::create_delimitor_locale<':', ',', ';'>();

struct Game {
	// Could've gone with a hash-map based approach (I didn't bother)
	uint64_t red;
	uint64_t green;
	uint64_t blue;

	uint64_t* get_color(std::string const& str) {
		// For some reason I did go with a tiny hash-map here.
		// Overhead is way too big for it's benefits, could easily just do 3 if-statements
		static std::unordered_map<std::string, uint64_t&> const COLOR_BINDS {{
			{"red", red},
			{"green", green},
			{"blue", blue}
		}};

		auto it = COLOR_BINDS.find(str);
		if (it != COLOR_BINDS.end()) {
			return &(it->second);
		}
		return nullptr; // unknown color
	}

	uint64_t power(void) const {
		return red * green * blue;
	}
};

std::vector<Game> parse_games(std::istream& stream) {
	std::vector<Game> games;
	for (auto line : aoc::Lines(stream)) {

		// Imbuing streams with a customized locale is pretty cool
		std::istringstream ss(line); ss.imbue(LOCALE);
		
		Game game {0, 0, 0};
		uint64_t x;
		std::string color;

		// Skip "Game x"
		ss >> aoc::next_digit >> x;

		// While we have a number-color pair
		while (ss >> x >> color) {
			// get a ptr to the corresponding color-count integer within current game
			auto* c = game.get_color(color);
			if (!c) {
				// Some bad error handling
				throw std::runtime_error("bad color");
			}
			// We only care about the biggest number
			*c = std::max(*c, x);
		}
		games.push_back(game);
	}
	return games;
}

uint64_t sum_games_id(std::vector<Game> const& games) {
	static Game const max_game {
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

int main(int argc, char** argv) {
	auto file = aoc::get_input_file(argc, argv);

	std::vector<Game> games = parse_games(file);

	std::cout << "(part 1) Sum of IDs of games:   "
		<< sum_games_id(games)
		<< std::endl;
	std::cout << "(part 2) Sum of power of games: "
		<< aoc::sum<uint64_t>(games, &Game::power)
		<< std::endl;

	return (EXIT_SUCCESS);
}
