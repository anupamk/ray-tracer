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

        ///
        /// compare two intersection records, and return true iff both point and
        /// object of intersection is same.
        ///
        bool operator==(intersection_record const& lhs, intersection_record const& rhs)
        {
                // clang-format off
                return ((lhs.where() == rhs.where()) &&
                        (lhs.what_object() == rhs.what_object()));
                // clang-format on
        }

        ///
        /// this function is called to compare two intersection records. it
        /// returns true if lhs < rhs, false otherwise.
        ///
        bool operator<(intersection_record const& lhs, intersection_record const& rhs)
        {
                return (lhs.where() < rhs.where());
        }

        ///
        /// this function is called to find the visible intersection (if it
        /// exists[*]) from a set of intersection records.
        ///
        /// [*] a visible intersection is the lowest non-zero
        /// intersection-record from the list of _sorted_ intersection records
        ///
        std::optional<intersection_record> visible_intersection(intersection_records const& ixns_list)
        {
                for (auto const& xs : ixns_list) {
                        if (xs.where() >= 0.0) {
                                return xs;
                        }
                }

                /// no visible intersections
                return std::nullopt;
        }

} // namespace raytracer