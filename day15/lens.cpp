#include "common.h"

#include <vector>
#include <list>
#include <algorithm>

static auto const LOCALE = aoc::create_delimitor_locale<',', '=', '-'>();

int64_t hash(std::string const& str) {
	int64_t const MULTIPLIER = 17;
	int64_t const MOD = 256;

	int64_t h = 0;
	for (auto const& c : str) {
		h += c;
		h *= MULTIPLIER;
		h = h % MOD;
	}
	return h;
}

struct Lens {
	std::string key;
	int64_t value;
};

using box_t = std::list<Lens>;

struct Instruction {
	Lens lens;
	char op;
};
std::vector<Instruction> parse_instructions(std::istream& stream) {
	std::vector<Instruction> instructions;

	stream.imbue(LOCALE);
	std::string s;
	int64_t x;

	while (stream >> s) {
		Instruction in;

		in.lens.key = s;
		in.op = stream.get();

		if (in.op == '=') {
			stream >> in.lens.value;
		}

		instructions.push_back(in);
	}
	return instructions;
}

void insert_lenses(std::vector<box_t>& boxes, std::vector<Instruction> const& instructions) {
	for (auto const& inst : instructions) {
		box_t& box = boxes[hash(inst.lens.key)];

		auto const& l = inst.lens;
		auto it = std::find_if(box.begin(), box.end(), [l](Lens const& a) {
			return a.key == l.key;
		});

		if (it != box.end()) {
			if (inst.op == '-') {
				box.erase(it);
			} else {
				it->value = l.value;
			}
		} else if (inst.op == '=') {
			box.push_back(l);
		}
	}
}

int64_t calculate_focus_power(std::vector<box_t> const& boxes) {
	int64_t sum = 0;
	int64_t i = 0;
	for (auto const& b : boxes) {
		++i;
		int64_t j = 0;
		for (auto const& l : b) {
			++j;
			sum += i * j * l.value;
		}
	}
	return sum;
}


int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto instructions = parse_instructions(*input);

	std::cout << "(Part 1) Sum of results: "
		<< aoc::sum(instructions, [](Instruction const& i) {
			std::string v;
			if (i.op == '=') {
				v = std::to_string(i.lens.value);
			}
			return hash(i.lens.key + i.op + v);
		})
		<< std::endl;

	// Part 2
	std::vector<box_t> boxes(256, box_t());
	insert_lenses(boxes, instructions);

	std::cout << "(Part 2) Resulting focus power: "
		<< calculate_focus_power(boxes)
		<< std::endl;

	return (EXIT_SUCCESS);
}