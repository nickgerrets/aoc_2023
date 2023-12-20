#include "common.h"

#include <vector>
#include <unordered_map>
#include <sstream>

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
		// update when receiving LOW signal
		if (sig == HIGH) {
			return {};
		}

		// Was on
		if (is_on) {
			sig = LOW;
		} else {
			sig = HIGH;
		}
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

		std::string str;
		ss >> str;

		modules_t::iterator it;
		switch (str[0]) {
			case '%': it = modules.emplace(str.substr(1), std::unique_ptr<Module>(new FlipFlop())).first; break;
			case '&': it = modules.emplace(str.substr(1),std::unique_ptr<Module>(new Conjunction())).first; break;
			default : it = modules.emplace(str,std::unique_ptr<Module>(new Broadcaster())).first; break;
		}

		while (ss >> str) {
			it->second->destination.push_back(str);
		}
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

struct SignalSend {
	Signal sig;
	std::string to;
	std::string from;
};

struct Result {
	int64_t low;
	int64_t high;
};

Result press_button(modules_t& modules) {
	static SignalSend const INITIAL_SIGNAL = {LOW, "broadcaster", "button"};

	int64_t low = 0;
	int64_t high = 0;

	std::vector<SignalSend> signals = { INITIAL_SIGNAL };
	while (!signals.empty()) {
		std::vector<SignalSend> new_signals;
		for (auto const& send : signals) {
			// count signals
			if (send.sig == LOW) {
				++low;
			} else {
				++high;
			}

			// std::cout << send.from << " --" << send.sig << "--> " << send.to << std::endl;

			if (modules.find(send.to) == modules.end()) {
				continue ;
			}

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

	int64_t low = 0;
	int64_t high = 0;

	for (size_t i = 0; i < 1000; ++i) {
		auto r = press_button(modules);
		low += r.low;
		high += r.high;
	}

	std::cout << low << " * " << high << " = " << low*high << std::endl;

	return (EXIT_SUCCESS);
}
