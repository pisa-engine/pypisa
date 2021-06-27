#include <memory>

#include "query_processor.hpp"

// #include <tbb/global_control.h>
// #include <tbb/parallel_for.h>

#include "app.hpp"

using namespace pisa;

void evaluate_queries(
    ProcFunc const& query_fun,
    const std::vector<Query>& queries,
    const std::optional<std::string>& thresholds_filename,
    std::string const& documents_filename,
    std::string const& run_id,
    std::string const& iteration)
{
    auto source = std::make_shared<mio::mmap_source>(documents_filename.c_str());
    auto docmap = Payload_Vector<>::from(*source);

    std::vector<std::vector<std::pair<float, uint64_t>>> raw_results(queries.size());
    auto start_batch = std::chrono::steady_clock::now();
    tbb::parallel_for(size_t(0), queries.size(), [&, query_fun](size_t query_idx) {
        raw_results[query_idx] = query_fun(queries[query_idx]);
    });
    auto end_batch = std::chrono::steady_clock::now();

    for (size_t query_idx = 0; query_idx < raw_results.size(); ++query_idx) {
        auto results = raw_results[query_idx];
        auto qid = queries[query_idx].id;
        std::size_t rank = 0;
        for (auto&& result: results) {
            std::cout << fmt::format(
                "{}\t{}\t{}\t{}\t{}\t{}\n",
                qid.value_or(std::to_string(query_idx)),
                iteration,
                docmap[result.second],
                rank,
                result.first,
                run_id);
            ++rank;
        }
    }
    auto end_print = std::chrono::steady_clock::now();
    double batch_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_batch - start_batch).count();
    double batch_with_print_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_print - start_batch).count();
    spdlog::info("Time taken to process queries: {}ms", batch_ms);
    spdlog::info("Time taken to process queries with printing: {}ms", batch_with_print_ms);
}

auto query()
{
    // tbb::global_control control(tbb::global_control::max_allowed_parallelism, app.threads() + 1);
    // spdlog::info("Number of worker threads: {}", app.threads());
    auto query_processor = QueryProcessor::load("", "", "");
    auto query_func = (*query_processor)("wand", ScorerParams("bm25"), 10);
    evaluate_queries(query_func, {}, "", "", "", "");
}
