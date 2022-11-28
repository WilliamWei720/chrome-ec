/* Copyright 2022 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Benchmark utility functions.
 */

#ifndef __CROS_EC_BENCHMARK_H
#define __CROS_EC_BENCHMARK_H

#include <array>
#include <optional>
#include <functional>
#include <stdint.h>
#include <string_view>

extern "C" {
#include "console.h"
#include "timer.h"
#include "util.h"
#include "clock.h"
#include "watchdog.h"
}

/* Benchmark execution options */
struct BenchmarkOptions {
	/* Number of test iterations */
	int num_iterations = 10;
	/* Whether to reload the watchdog between executions of f() */
	bool reload_watchdog = true;
	/* Whether to enable fast CPU clock during the test (when supported) */
	bool use_fast_cpu = true;
};

/* The result of a benchmark run with various timing metrics.
 * All time measurements are in micro seconds, except the average that
 * is captured in nanoseconds for increased resolution.
 */
struct BenchmarkResult {
	/* Name of the test, used when printing results */
	std::string_view name;
	/* Total elapsed time (us) for all iterations */
	uint32_t elapsed_time;
	/* Average elapsed time (ns) for a single iteration */
	uint32_t average_time;
	/* Minimum elapsed time (us) for a single iteration */
	uint32_t min_time;
	/* Maximum elapsed time (us) for a single iteration */
	uint32_t max_time;
};

/* Benchmark main class responsible for running the experiments and
 * collecting/printing the results.
 * Note that the implementation intentionally avoid dynamic memory allocations
 * and stores up to MAX_NUM_RESULTS results into a std::array.
 */
template <int MAX_NUM_RESULTS = 5> class Benchmark {
    public:
	explicit Benchmark(const BenchmarkOptions &options = BenchmarkOptions())
		: options_(options){};

	/* Run benchmark of the function f().
	 *
	 * TODO(b/253099367): replace std::optional with StatusOr
	 */
	std::optional<BenchmarkResult>
	run(const std::string_view benchmark_name, std::function<void()> f)
	{
		if (benchmark_name.empty()) {
			ccprintf("%s: benchmark_name cannot be empty\n",
				 __func__);
			return {};
		}
		if (num_results_ >= MAX_NUM_RESULTS) {
			ccprintf("%s: cannot store new BenchmarkResults\n",
				 __func__);
			return {};
		}

		BenchmarkResult &result = results_[num_results_++];
		result.name = benchmark_name;
		result.elapsed_time = 0;

		if (options_.use_fast_cpu)
			clock_enable_module(MODULE_FAST_CPU, 1);

		bool valid_min_max = false;
		for (int i = 0; i < options_.num_iterations; ++i) {
			timestamp_t start_time = get_time();
			f();
			uint32_t iteration_time = time_since32(start_time);

			if (options_.reload_watchdog)
				watchdog_reload();

			if (valid_min_max) {
				result.max_time =
					MAX(result.max_time, iteration_time);
				result.min_time =
					MIN(result.min_time, iteration_time);
			} else {
				result.max_time = iteration_time;
				result.min_time = iteration_time;
				valid_min_max = true;
			}
			result.elapsed_time += iteration_time;
		}

		if (options_.use_fast_cpu)
			clock_enable_module(MODULE_FAST_CPU, 0);

		result.average_time =
			(result.elapsed_time) / options_.num_iterations;

		return result;
	}

	void print_results() const
	{
		print_header();
		for (int i = 0; i < num_results_; ++i)
			print_result(results_[i]);
	}

    private:
	const BenchmarkOptions options_;
	std::array<BenchmarkResult, MAX_NUM_RESULTS> results_;
	int num_results_ = 0;

	/* Print table header with column names */
	void print_header() const
	{
		constexpr char kSeparator[] = "--------------------------";

		ccprintf("%16s | %15s | %13s | %13s | %13s | %13s\n",
			 "Test Name", "Num Iterations", "Elapsed (us)",
			 "Min (us)", "Max (us)", "Avg (us)");
		ccprintf("%.*s | %.*s | %.*s | %.*s | %.*s | %.*s\n", 16,
			 kSeparator, 15, kSeparator, 13, kSeparator, 13,
			 kSeparator, 13, kSeparator, 13, kSeparator);
		cflush();
	}

	/* Print a single benchmark result */
	int print_result(const BenchmarkResult &result) const
	{
		ccprintf("%16s | %15u | %13u | %13u | %13u | %13u\n",
			 result.name.data(), options_.num_iterations,
			 result.elapsed_time, result.min_time, result.max_time,
			 result.average_time);
		cflush();
		return EC_SUCCESS;
	}
};

#endif /* __CROS_EC_BENCHMARK_H */
