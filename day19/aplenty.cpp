#include "common.h"
#include "vec2.h"

#include <vector>
#include <unordered_map>
#include <sstream>

static auto const LOCALE = aoc::create_delimitor_locale<'{','}', ',', '='>();

struct Rule {
	char c;
	char op;
	int64_t value;
	std::string key;

	void parse(std::string const& str) {
		c = str[0];
		op = str[1];
		value = std::stoll(str.substr(2, str.find_first_of(':') - 2));
		key = str.substr(str.find_first_of(':') + 1);
	}
};

using workflows_t = std::unordered_map<std::string, std::vector<Rule>>;
workflows_t parse_workflows(std::istream& stream) {
	workflows_t workflows;
	for (auto const& l : aoc::Lines(stream)) {

		if (l.empty()) {
			return workflows;
		}

		std::istringstream ss(l); ss.imbue(LOCALE);

		std::string key, tmp;
		ss >> key;

		workflows.insert({key, {}});
		auto& rules = workflows.at(key);
		while (ss >> tmp) {
			Rule r;
			if (tmp.find(':') != std::string::npos) {
				r.parse(tmp);
			} else {
				r.c = 0;
				r.op = 0;
				r.value = 0;
				r.key = tmp;
			}
			rules.push_back(r);
		}
	}
	return workflows;
}

struct Rating {
	int64_t x, m, a, s;

	int64_t sum(void) const {
		return (x + m + a + s);
	}
};

std::vector<Rating> parse_ratings(std::istream& stream) {
	std::vector<Rating> ratings;
	for (auto const& l : aoc::Lines(stream)) {
		Rating r;

		std::cout << l << std::endl;

		std::istringstream ss(l); ss.imbue(LOCALE);

		char tmp;
		ss >> tmp >> r.x >> tmp >> r.m >> tmp >> r.a >> tmp >> r.s;

		ratings.push_back(r);
	}
	return ratings;
}

bool less_then(int64_t x, int64_t n) {
	return x < n;
}

bool greater_then(int64_t x, int64_t n) {
	return x > n;
}

std::string next_key(std::vector<Rule> const& rules, Rating const& r) {
	for (auto const& rule : rules) {
		if (rule.c == 0) {
			return rule.key;
		}

		auto func = less_then;
		if (rule.op == '>') {
			func = greater_then;
		}

		switch (rule.c) {
			case 'x': if (func(r.x, rule.value)) { return rule.key; } break ;
			case 'm': if (func(r.m, rule.value)) { return rule.key; } break ;
			case 'a': if (func(r.a, rule.value)) { return rule.key; } break ;
			case 's': if (func(r.s, rule.value)) { return rule.key; } break ;
		}
	}
	return {};
}

std::vector<Rating> trace_ratings(workflows_t const& workflows, std::vector<Rating> const& ratings) {
	std::string const START_KEY = "in";

	std::vector<Rating> accepted;
	for (auto const& r : ratings) {
		std::string key = START_KEY;
		while (key != "A" && key != "R") {
			key = next_key(workflows.at(key), r);
		}
		if (key == "A") {
			accepted.push_back(r);
			continue ;
		}
	}
	return accepted;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto workflows = parse_workflows(*input);

	auto ratings = parse_ratings(*input);

	auto accepted = trace_ratings(workflows, ratings);

	std::cout << "(Part 1) Sum of ratings of accepted parts: "
		<< aoc::sum(accepted, &Rating::sum)
		<< std::endl;
	

	

	return EXIT_SUCCESS;
}
