#ifndef VEC2_H
# define VEC2_H

#include <cmath>
#include <string>

template <typename T>
struct Vector2D {
	using value_t = T;

	value_t x;
	value_t y;

/* -------------------------------------------------------------------------- */
/*                          Constructors/Destructors                          */
/* -------------------------------------------------------------------------- */

	Vector2D() : x(0), y(0) {}
	Vector2D(value_t x, value_t y) : x(x), y(y) {}

/* -------------------------------------------------------------------------- */
/*                                  Operators                                 */
/* -------------------------------------------------------------------------- */

	Vector2D operator*(Vector2D const& rhs) const {
		return {x * rhs.x, y * rhs.y};
	}

	Vector2D operator*(value_t rhs) const {
		return {x * rhs, y * rhs};
	}

	Vector2D operator/(Vector2D const& rhs) const {
		return {x / rhs.x, y / rhs.y};
	}

	Vector2D operator/(value_t rhs) const {
		return {x / rhs, y / rhs};
	}

	Vector2D operator+(Vector2D const& rhs) const {
		return {x + rhs.x, y + rhs.y};
	}

	Vector2D operator+(value_t rhs) const {
		return {x + rhs, y + rhs};
	}

	Vector2D operator-(Vector2D const& rhs) const {
		return {x - rhs.x, y - rhs.y};
	}

	Vector2D operator-(value_t rhs) const {
		return {x - rhs, y - rhs};
	}

	Vector2D operator-() const {
		return {-x, -y};
	}

	// Comparison

	bool operator==(Vector2D const& rhs) const {
		return (x == rhs.x && y == rhs.y);
	}

	bool operator!=(Vector2D const& rhs) const {
		return (x != rhs.x || y != rhs.y);
	}

	bool operator<(Vector2D const& rhs) const {
		if (y == rhs.y) {
			return (x < rhs.x);
		}
		return (y < rhs.y);
	}

	bool operator>(Vector2D const& rhs) const {
		if (y == rhs.y) {
			return (x > rhs.x);
		}
		return (y > rhs.y);
	}

/* -------------------------------------------------------------------------- */
/*                                   Methods                                  */
/* -------------------------------------------------------------------------- */

	value_t distance(Vector2D const& other) const {
		value_t rx = x - other.x;
		value_t ry = y - other.y;
		return (std::sqrt(rx * rx + ry * ry));
	}

	value_t distance(void) const {
		return distance({0, 0});
	}

	value_t manhattan(Vector2D const& other) const {
		value_t rx = x - other.x;
		value_t ry = y - other.y;
		return (std::abs(rx) + std::abs(ry));
	}

	value_t manhattan(void) const {
		return manhattan({0, 0});
	}

	bool is_within_bounds(Vector2D const& upper_left, Vector2D const& lower_right) const{
		return (x >= upper_left.x && x <= lower_right.x && y >= upper_left.y && y <= lower_right.y);
	}

	std::string to_string(void) const {
		return ("(" + std::to_string(x) + ',' + std::to_string(y) + ')');
	}

	static Vector2D right(void) {
		return {1, 0};
	}

	static Vector2D left(void) {
		return {-1, 0};
	}

	static Vector2D up(void) {
		return {0, -1};
	}

	static Vector2D down(void) {
		return {0, 1};
	}

};

using Vec2 = Vector2D<int64_t>;

template <>
struct std::hash<Vec2> {
	size_t operator()(Vec2 const& v) const {
		size_t seed = std::hash<int64_t>()(v.x);
		return std::hash<int64_t>()(v.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
};

#endif
