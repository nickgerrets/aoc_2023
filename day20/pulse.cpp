#include "common.h"

#include <vector>
#include <unordered_map>
#include <sstream>
#include <numeric>

static auto const LOCALE = aoc::create_delimitor_locale<',', '-', '>'>();

enum Signal {
	LOW,
	HIGH
};

enum ModuleType {
	BROADCASTER,
	FLIPFLOP,
	CONJUNCTION
};

struct Module {
	using signals_t = std::vector<std::pair<Signal, std::string>>;

	Module(ModuleType t) : type(t) {}

	virtual ~Module() {}

	std::vector<std::string> destination;
	ModuleType type;

	virtual signals_t on_signal_recv(std::string const& from, Signal sig) = 0;

};

struct FlipFlop : public Module {
	FlipFlop() : Module(FLIPFLOP), is_on(false) {}
	bool is_on;

	signals_t on_signal_recv(std::string const& from, Signal sig) {
		(void)from;
		// update when receiving LOW signal, HIGH is ignored
		if (sig == HIGH) return {};

		sig = is_on ? LOW : HIGH;
		is_on = !is_on;

		signals_t send;
		for (auto const& d : destination) {
			send.emplace_back(sig, d);
		}
		return send;
	}
};

struct Conjunction : public Module {
	Conjunction() : Module(CONJUNCTION) {}
	std::unordered_map<std::string, Signal> inputs;

	signals_t on_signal_recv(std::string const& from, Signal sig) {
		// Update inputs and send signal if all are high
		inputs[from] = sig;
		sig = LOW;
		for (auto const& pair : inputs) {
			if (pair.second == LOW) {
				sig = HIGH;
			}
		}

		signals_t send;
		for (auto const& d : destination) {
			send.emplace_back(sig, d);
		}
		return send;
	}
};

// Class purely to capture signals for the conjunction to RX
struct ConjunctionToRX : public Conjunction {
	ConjunctionToRX() : Conjunction() {}

	std::unordered_map<std::string, size_t> cycle_counts;
	static size_t count;

	signals_t on_signal_recv(std::string const& from, Signal sig) {
		signals_t r = this->Conjunction::on_signal_recv(from, sig);

		if (sig == HIGH && cycle_counts.find(from) == cycle_counts.end()) {
			cycle_counts[from] = count;
		}

		return r;
	}
};

size_t ConjunctionToRX::count = 0;
static ConjunctionToRX* cj_to_rx = nullptr;

struct Broadcaster : public Module {
	Broadcaster() : Module(BROADCASTER) {}
	signals_t on_signal_recv(std::string const& from, Signal sig) {
		(void)from;
		// Send same signal to all connected destination modules
		signals_t send;
		for (auto const& d : destination) {
			send.emplace_back(sig, d);
		}
		return send;
	}
};

using modules_t = std::unordered_map<std::string, std::unique_ptr<Module>>;
modules_t parse_modules(std::istream& stream) {
	modules_t modules;

	for (auto const& line : aoc::Lines(stream)) {
		std::istringstream ss(line); ss.imbue(LOCALE);

		std::string key, str;
		ss >> key;

		std::vector<std::string> destination;
		while (ss >> str) {
			destination.push_back(str);
		}

		// special case for conjunction to rx
		if (str == "rx") {
			cj_to_rx = new ConjunctionToRX();
			modules.emplace(key.substr(1), std::unique_ptr<Module>(cj_to_rx));
			cj_to_rx->destination = destination;
			continue;
		}

		modules_t::iterator it;
		switch (key[0]) {
			case '%': it = modules.emplace(key.substr(1), std::unique_ptr<Module>(new FlipFlop())).first; break;
			case '&': it = modules.emplace(key.substr(1),std::unique_ptr<Module>(new Conjunction())).first; break;
			default : it = modules.emplace(key,std::unique_ptr<Module>(new Broadcaster())).first; break;
		}
		it->second->destination = destination;
	}

	// Setup Conjunction modules
	for (auto& p1 : modules) {
		if (p1.second->type == CONJUNCTION) {
			for (auto const& p2 : modules) {
				auto const& dst = p2.second->destination;
				if (std::find(dst.begin(), dst.end(), p1.first) != dst.end()) {
					static_cast<Conjunction*>(p1.second.get())->inputs[p2.first] = LOW;
				}
			}
		}
	}
	return std::move(modules);
}

struct Result {
	int64_t low;
	int64_t high;
};

Result press_button(modules_t& modules) {
	struct SignalSend {
		Signal sig;
		std::string to;
		std::string from;
	};
	static SignalSend const INITIAL_SIGNAL = {LOW, "broadcaster", "button"};

	int64_t low = 0, high = 0;
	std::vector<SignalSend> signals = { INITIAL_SIGNAL };
	while (!signals.empty()) {
		std::vector<SignalSend> new_signals;
		for (auto const& send : signals) {
			// count signals
			low += (send.sig == LOW);
			high += (send.sig == HIGH);

			// std::cout << send.from << " --" << send.sig << "--> " << send.to << std::endl;

			// Signal destination doesn't exist
			if (modules.find(send.to) == modules.end()) {
				continue ;
			}

			// Notify the module of the signal
			auto& mod = modules[send.to];
			auto to_insert = mod->on_signal_recv(send.from, send.sig);
			
			// Insert newly received signals
			for (auto const& pair : to_insert) {
				new_signals.push_back({pair.first, pair.second, send.to});
			}
		}
		signals.swap(new_signals);
	}
	return {low, high};
}

int main(int argc, char** argv) {
	auto input = aoc::get_input(argc, argv);

	auto modules = parse_modules(*input);

	int64_t low = 0, high = 0;
	for (size_t i = 0; i < 1000; ++i) {
		ConjunctionToRX::count += 1;
		auto r = press_button(modules);
		low += r.low;
		high += r.high;
	}

	std::cout << "(Part 1) LOW * HIGH after 1000 button presses: " << low * high << std::endl;

	// PART 2
	// There is a Conjunction connected to rx, so if all inputs to that
	// conjunction are HIGH, then LOW will be send to rx.

	// Track how many presses it takes for each individual input to do a cycle
	while (cj_to_rx->cycle_counts.size() != cj_to_rx->inputs.size()) {
		ConjunctionToRX::count += 1;
		press_button(modules);
	}

	// Now perfrom lcm on these cycle counts
	size_t lcm = 1;
	for (auto const& c : cj_to_rx->cycle_counts) {
		lcm = aoc::least_common_multiple(lcm, c.second);
	}

	std::cout << "(Part 2) Fewest button presses for LOW to rx:  " << lcm << std::endl;

	return (EXIT_SUCCESS);
}
