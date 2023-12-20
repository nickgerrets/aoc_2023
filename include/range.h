#ifndef RANGE_H
# define RANGE_H

# include <algorithm>

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

#endif // RANGE_H
