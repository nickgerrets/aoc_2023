#include "common.h"
#include "range.h"

#include <vector>
#include <unordered_map>
#include <sstream>

static auto const LOCALE = aoc::create_delimitor_locale<'{','}', ',', '='>();

enum RatingEnum {
	X = 0,
	M,
	A,
	S,
	NONE
};

RatingEnum rating_from_c(char c) {
	switch (c) {
		case 'x': return X;
		case 'm': return M;
		case 'a': return A;
		case 's': return S;
	}
	return X;
}

struct Rule {
	RatingEnum c;
	char op;
	int64_t value;
	std::string key;

	void parse(std::string const& str) {
		c = rating_from_c(str[0]);
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
				r.c = NONE;
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
	int64_t r[4];

	int64_t sum(void) const {
		return (r[X] + r[M] + r[A] + r[S]);
	}
};

std::vector<Rating> parse_ratings(std::istream& stream) {
	std::vector<Rating> ratings;
	for (auto const& l : aoc::Lines(stream)) {
		Rating r;

		std::istringstream ss(l); ss.imbue(LOCALE);

		char tmp;
		ss >> tmp >> r.r[X] >> tmp >> r.r[M] >> tmp >> r.r[A] >> tmp >> r.r[S];

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
		if (rule.c == NONE) {
			return rule.key;
		}

		auto func = less_then;
		if (rule.op == '>') {
			func = greater_then;
		}

		if (func(r.r[rule.c], rule.value)) {
			return rule.key;
		}
	}
	return {};
}

std::vector<Rating> trace_ratings(workflows_t const& workflows, std::vector<Rating> const& ratings) {
	static std::string const START_KEY = "in";

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

using ranges_t = std::unordered_map<RatingEnum, Range>;

void merge_ranges(ranges_t& merge, ranges_t const& other) {
	for (auto const& pair : other) {
		merge[pair.first] = pair.second;
	}
}

std::vector<ranges_t> trace_ranges(workflows_t const& workflows, ranges_t ranges, std::string key) {
	if (key == "R") return {};
	if (key == "A") return {ranges};

	std::vector<ranges_t> accepted {};
	for (auto const& rule : workflows.at(key)) {
		if (rule.c == NONE) {
			auto add = trace_ranges(workflows, ranges, workflows.at(key).back().key);
			accepted.insert(accepted.end(), add.begin(), add.end());
			continue ;
		}

		Range r = ranges[rule.c];
		switch (rule.op) {
			case '<': {
				ranges_t copy(ranges);
				copy[rule.c] = Range(r.begin, rule.value - 1);
				auto add = trace_ranges(workflows, copy, rule.key);
				accepted.insert(accepted.end(), add.begin(), add.end());
				ranges[rule.c]= Range(rule.value, r.end);
			} break ;
			case '>': {
				ranges_t copy(ranges);
				copy[rule.c] = Range(rule.value + 1, r.end);
				auto add = trace_ranges(workflows, copy, rule.key);
				accepted.insert(accepted.end(), add.begin(), add.end());
				ranges[rule.c] = Range(r.begin, rule.value);
			} break ;
		}
	}
	return accepted;
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto workflows = parse_workflows(*input);
	auto ratings = parse_ratings(*input);

	// PART 1
	auto accepted = trace_ratings(workflows, ratings);
	std::cout << "(Part 1) Sum of ratings of accepted parts: "
		<< aoc::sum(accepted, &Rating::sum)
		<< std::endl;
	
	// PART 2
	static std::string const START_KEY = "in";
	static uint64_t const MIN = 1, MAX = 4000;
	ranges_t ranges {
		{X, {MIN, MAX}},
		{M, {MIN, MAX}},
		{A, {MIN, MAX}},
		{S, {MIN, MAX}}
	};

	auto accepted_ranges = trace_ranges(workflows, ranges, START_KEY);
	std::cout << "(Part 2) Distinct combinations of ratings accepted: "
		<< aoc::sum(accepted_ranges, [](ranges_t const& ranges) {
			return aoc::product(ranges, [](ranges_t::value_type const& pair) {
				return pair.second.end - pair.second.begin + 1;
			});
		})
		<< std::endl;

	return EXIT_SUCCESS;
}
