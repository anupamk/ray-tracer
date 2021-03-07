#ifndef RAYTRACER_INTERSECTION_RECORD_HPP__
#define RAYTRACER_INTERSECTION_RECORD_HPP__

/// c++ includes
#include <algorithm>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace raytracer
{
	/// forward declaration
	class shape_interface;

	/*
	 * an intersection_record describes a single intersection that a ray
	 * makes with an object in the scene
	 **/
	class intersection_record
	{
	    private:
		/// ------------------------------------------------------------
		/// where intersection happened
		double where_;

		/// ------------------------------------------------------------
		/// what object was intersected
		std::shared_ptr<shape_interface const> what_;

		/// ------------------------------------------------------------
		/// HACK: index of this record in the intersection-list
		uint32_t index_;

	    public:
		intersection_record(double t, std::shared_ptr<shape_interface const> a_shape)
		    : where_(t)
		    , what_(a_shape)
		    , index_(0)
		{
		}

	    public:
		constexpr double where() const
		{
			return this->where_;
		}

		std::shared_ptr<shape_interface const> what_object() const
		{
			return this->what_;
		}

	    public:
		/// ------------------------------------------------------------
		/// HACK HACK HACK
		constexpr uint32_t index() const
		{
			return this->index_;
		}

		void index(uint32_t idx)
		{
			this->index_ = idx;
		}

		/// stringified representation of an intersection record
		std::string stringify() const;
	};

	bool operator==(intersection_record const& lhs, intersection_record const& rhs);
	bool operator<(intersection_record const& lhs, intersection_record const& rhs);
	std::ostream& operator<<(std::ostream& os, intersection_record const& intersection);

	/*
	 * a vector of intersection_record describe the result of a ray
	 * intersecting shape(s) in a scene at possibly multiple points.
	 *
	 * since vector's canonical comparison operators do the right thing for
	 * intersection_records as well. nothing special is required.
	 **/
	using intersection_records = std::vector<intersection_record>;

	/// --------------------------------------------------------------------
	/// this function is called create an intersection_records
	/// from a bunch of intersection record instances.
	template <typename... T>
	intersection_records create_intersections(T const&... t)
	{
		intersection_records intersections{t...};
		std::sort(intersections.begin(), intersections.end());

		return intersections;
	}

	/// --------------------------------------------------------------------
	/// this function is called to find the visible intersection (if it
	/// exists) from a set of intersection records.
	std::optional<intersection_record> visible_intersection(intersection_records const& ixns_list);

} // namespace raytracer

#endif /// RAYTRACER_INTERSECTION_RECORD_HPP__
