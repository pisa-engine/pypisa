#pragma once

#include "query_engine.hpp"

#include <string_view>
#include <vector>

#include <accumulator/lazy_accumulator.hpp>
#include <accumulator/simple_accumulator.hpp>
#include <cursor/block_max_scored_cursor.hpp>
#include <cursor/max_scored_cursor.hpp>
#include <cursor/scored_cursor.hpp>
#include <query/algorithm.hpp>
#include <query/queries.hpp>
#include <scorer/scorer.hpp>
#include <topk_queue.hpp>

namespace pypisa {

/// This function template resolves a query processor once the Index and Wand types are known.
/// This is meant to be used from inside any subclass of `QueryEngine`.
template <typename Index, typename Wand>
auto resolve_query_processor(
    Index&& index, Wand&& wdata, std::string_view algorithm, ScorerParams scorer_params, std::size_t k)
    -> QueryProcessor
{
    auto scorer = pisa::scorer::from_params(scorer_params, wdata);
    QueryProcessor query_fun;

    if (algorithm == "wand") {
        query_fun = [&](pisa::Query query) {
            pisa::topk_queue topk(k);
            pisa::wand_query wand_q(topk);
            wand_q(make_max_scored_cursors(index, wdata, *scorer, query), index.num_docs());
            topk.finalize();
            return topk.topk();
        };
    } else if (algorithm == "block_max_wand") {
        query_fun = [&](pisa::Query query) {
            pisa::topk_queue topk(k);
            pisa::block_max_wand_query block_max_wand_q(topk);
            block_max_wand_q(
                make_block_max_scored_cursors(index, wdata, *scorer, query), index.num_docs());
            topk.finalize();
            return topk.topk();
        };
    } else if (algorithm == "block_max_maxscore") {
        query_fun = [&](pisa::Query query) {
            pisa::topk_queue topk(k);
            pisa::block_max_maxscore_query block_max_maxscore_q(topk);
            block_max_maxscore_q(
                make_block_max_scored_cursors(index, wdata, *scorer, query), index.num_docs());
            topk.finalize();
            return topk.topk();
        };
    } else if (algorithm == "block_max_ranked_and") {
        query_fun = [&](pisa::Query query) {
            pisa::topk_queue topk(k);
            pisa::block_max_ranked_and_query block_max_ranked_and_q(topk);
            block_max_ranked_and_q(
                make_block_max_scored_cursors(index, wdata, *scorer, query), index.num_docs());
            topk.finalize();
            return topk.topk();
        };
    } else if (algorithm == "ranked_and") {
        query_fun = [&](pisa::Query query) {
            pisa::topk_queue topk(k);
            pisa::ranked_and_query ranked_and_q(topk);
            ranked_and_q(make_scored_cursors(index, *scorer, query), index.num_docs());
            topk.finalize();
            return topk.topk();
        };
    } else if (algorithm == "ranked_or") {
        query_fun = [&](pisa::Query query) {
            pisa::topk_queue topk(k);
            pisa::ranked_or_query ranked_or_q(topk);
            ranked_or_q(make_scored_cursors(index, *scorer, query), index.num_docs());
            topk.finalize();
            return topk.topk();
        };
    } else if (algorithm == "maxscore") {
        query_fun = [&](pisa::Query query) {
            pisa::topk_queue topk(k);
            pisa::maxscore_query maxscore_q(topk);
            maxscore_q(make_max_scored_cursors(index, wdata, *scorer, query), index.num_docs());
            topk.finalize();
            return topk.topk();
        };
    } else if (algorithm == "ranked_or_taat") {
        query_fun = [&, accumulator = pisa::Simple_Accumulator(index.num_docs())](
                        pisa::Query query) mutable {
            pisa::topk_queue topk(k);
            pisa::ranked_or_taat_query ranked_or_taat_q(topk);
            ranked_or_taat_q(
                make_scored_cursors(index, *scorer, query), index.num_docs(), accumulator);
            topk.finalize();
            return topk.topk();
        };
    } else if (algorithm == "ranked_or_taat_lazy") {
        query_fun = [&, accumulator = pisa::Lazy_Accumulator<4>(index.num_docs())](
                        pisa::Query query) mutable {
            pisa::topk_queue topk(k);
            pisa::ranked_or_taat_query ranked_or_taat_q(topk);
            ranked_or_taat_q(
                make_scored_cursors(index, *scorer, query), index.num_docs(), accumulator);
            topk.finalize();
            return topk.topk();
        };
    } else {
        spdlog::error("Unsupported query type: {}", algorithm);
    }
    return query_fun;
}

}  // namespace pypisa
