/**
 * trivial benchmarking of function executions
 **/
#pragma once

/// c++ includes
#include <algorithm>
#include <chrono>
#include <functional>
#include <numeric>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

/// our includes
#include "common/include/logging.h"

template <typename TimeT = std::chrono::milliseconds>
class benchmark_t
{
    private:
        uint32_t num_iter_;
        uint32_t throw_away_;

        std::vector<typename TimeT::rep> num_times_;
        typename TimeT::rep mean_;
        typename TimeT::rep std_dev_;

        std::string const user_msg_;

    public:
        benchmark_t(std::string user_message, uint32_t num_iterations = 1, uint32_t throw_away = 0)
            : num_iter_(num_iterations)
            , throw_away_(throw_away)
            , num_times_{}
            , mean_{}
            , std_dev_{}
            , user_msg_(std::move(user_message))
        {
        }

        /// --------------------------------------------------------------------
        /// meta information about the benchmark itself
        std::string stringify() const
        {
                std::stringstream ss("");

                // clang-format off
                ss << "{"
                   << "iterations: "  << num_iter_  << ", "
                   << "throw-away: "  << throw_away_ << ", "
                   << "num_times: "   << num_times_.size()
                   << "}";
                // clang-format on

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// this function is called to dump statistics of render that was
        /// performed.
        void show_stats() const
        {
                LOG_INFO("%s, results: {mean (ms): '%05zu', standard-deviation (ms): '%05zu'}",
                         user_msg_.c_str(),     /// user-string
                         mean(),                /// mean-usec
                         standard_deviation()); /// stddev-usec
        }

        /// --------------------------------------------------------------------
        /// execute the function to be benchmarked and collect the results
        template <typename Fun, typename... Args>
        typename std::result_of<Fun(Args...)>::type benchmark(Fun&& fun, Args&&... args)
        {
                using result_t = typename std::invoke_result<Fun, Args...>::type;
                std::vector<result_t> results;

                num_times_.clear();
                uint32_t const n = num_iter_ + throw_away_;

                for (uint32_t i = 0; i < n; i++) {
                        LOG_INFO("running %04d / %04d", i + 1, n);

                        /// measure execution
                        auto start_time = std::chrono::steady_clock::now();
                        auto result_i   = std::invoke(std::forward<Fun>(fun), std::forward<Args>(args)...);
                        auto end_time   = std::chrono::steady_clock::now();
                        auto time       = std::chrono::duration_cast<TimeT>(end_time - start_time).count();

                        num_times_.push_back(time);
                        results.push_back(result_i);

                        LOG_INFO("----------------------------------------------------------------");
                }

                compute_mean();
                compute_st_dev();

                return results[0];
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
