/**
 * trivial benchmarking of function executions
 **/

#ifndef BENCHMARK_HPP__
#define BENCHMARK_HPP__

/// c++ includes
#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

/// our includes
#include "logging.h"

template <typename TimeT = std::chrono::milliseconds>
class Benchmark
{
    private:
	int num_iter_;
	int throw_away_;

	std::vector<typename TimeT::rep> num_times_;
	typename TimeT::rep mean_;
	typename TimeT::rep std_dev_;

    public:
	Benchmark(int num_iterations = 100, int throw_away = 0)
	    : num_iter_(num_iterations)
	    , throw_away_(throw_away)
	    , num_times_{}
	    , mean_{}
	    , std_dev_{}
	{
	}

	/// --------------------------------------------------------------------
	/// meta information about the benchmark itself
	std::string stringify() const
	{
		std::stringstream ss("");

		// clang-format off
                ss << "{"
                   << "iterations: "  << this->num_iter_  << ", "
                   << "throw-away: " << this->throw_away_ << ", "
                   << "num_times: "   << this->num_times_.size()
                   << "}";
		// clang-format on

		return ss.str();
	}

	/// --------------------------------------------------------------------
	/// execute the function to be benchmarked and collect the results
	template <typename Fun, typename... Args>
	std::vector<typename std::result_of<Fun(Args...)>::type> benchmark(Fun&& fun, Args&&... args)
	{
		using result_t = typename std::result_of<Fun(Args...)>::type;

		std::vector<result_t> results;
		num_times_.clear();
		auto n = num_iter_ + throw_away_;

		for (auto i = 0; i < n; i++) {
			/// ----------------------------------------------------
			/// log something otherwise it gets lonely
			LOG_DEBUG("running %04d / %04d", i + 1, n);

			auto pair = measure_execution(std::forward<Fun>(fun), std::forward<Args>(args)...);
			num_times_.push_back(pair.first);
			results.push_back(pair.second);
		}

		compute_mean();
		compute_st_dev();

		return results;
	}

	typename TimeT::rep mean() const
	{
		return mean_;
	}

	typename TimeT::rep standard_deviation() const
	{
		return std_dev_;
	}

    private:
	/// --------------------------------------------------------------------
	/// only private member functions from this point onwards

	/// --------------------------------------------------------------------
	/// measure the execution
	template <typename Fun, typename... Args>
	static std::pair<typename TimeT::rep, typename std::result_of<Fun(Args...)>::type>
	measure_execution(Fun&& fun, Args&&... args)
	{
		auto start_time = std::chrono::steady_clock::now();
		auto result     = std::forward<Fun>(fun)(std::forward<Args>(args)...);
		auto end_time   = std::chrono::steady_clock::now();

		auto time = std::chrono::duration_cast<TimeT>(end_time - start_time).count();
		return std::make_pair(time, result);
	}

	/// --------------------------------------------------------------------
	/// compute execution mean
	void compute_mean()
	{
		auto sum = std::accumulate(num_times_.begin() + throw_away_, num_times_.end(), 0);
		mean_    = sum / num_iter_;
	}

	/// --------------------------------------------------------------------
	/// compute execution standard deviation
	void compute_st_dev()
	{
		std::vector<typename TimeT::rep> diff(num_iter_);
		std::transform(num_times_.begin() + throw_away_, num_times_.end(), diff.begin(),
		               [this](typename TimeT::rep t) { return t - this->mean_; });

		auto sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0);
		std_dev_    = std::sqrt(sq_sum / num_iter_);
	}
};

#endif
