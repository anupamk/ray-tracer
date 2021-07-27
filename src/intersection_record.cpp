/*
 * implement raytracer intersection record
 **/

/// c++ includes
#include <ios>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

/// our includes
#include "intersection_record.hpp"
#include "utils.hpp"

namespace raytracer
{
	std::string intersection_record::stringify() const
	{
		std::stringstream ss("");

		// clang-format off
                ss << "position: " << this->where() << ", "
                   << "obj-addr: " << std::hex << this->what_object();
		// clang-format on

		return ss.str();
	}

	std::ostream& operator<<(std::ostream& os, intersection_record const& I)
	{
		return os << I.stringify();
	}

	/// --------------------------------------------------------------------
	/// compare two intersection records, and return true iff both point and
	/// object of intersection is same.
	///
	/// ofcourse point-of-intersection must be epsilon equal
	bool operator==(intersection_record const& lhs, intersection_record const& rhs)
	{
		auto xs_where_eps_equal = epsilon_equal(lhs.where(), rhs.where());
		auto xs_what_equal      = (lhs.what_object() == rhs.what_object());

		return (xs_where_eps_equal && xs_what_equal);
	}

	/// --------------------------------------------------------------------
	/// this function is called to compare two intersection records. it
	/// returns true if lhs < rhs, false otherwise.
	bool operator<(intersection_record const& lhs, intersection_record const& rhs)
	{
		return (lhs.where() < rhs.where());
	}

	/// --------------------------------------------------------------------
	/// this function is called to find the visible intersection (if it
	/// exists[*]) from a set of intersection records.
	///
	/// [*] a visible intersection is the lowest non-zero
	/// intersection-record from the list of _sorted_ intersection records
	std::optional<intersection_record> visible_intersection(intersection_records const& ixns_list)
	{
		uint32_t idx = 0;

		for (auto xs : ixns_list) {
			if (xs.where() >= 0.0) {
				xs.index(idx);
				return xs;
			}

			idx += 1;
		}

		/// no visible intersections
		return std::nullopt;
	}

} // namespace raytracer
