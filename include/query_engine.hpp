#pragma once

#include <string_view>
#include <vector>

#include <query/queries.hpp>
#include <scorer/scorer.hpp>

namespace pypisa {

/// A single result, which is a pair of <score, docid>.
using QueryResult = std::pair<float, uint64_t>;

/// A function that takes a query, and returns results.
using QueryProcessor = std::function<std::vector<QueryResult>(pisa::Query)>;

/// The main piece of machinery for querying an index.
///
/// Under the hood, it loads a given index, and produces query processor functions
/// for given algorithm and scoring parameters.
class QueryEngine {
  public:
    /// Returns a query processor for the given algorithm, scorer parameters, and number of results.
    virtual auto processor(std::string_view algorithm, ScorerParams scorer_params, std::size_t k)
        -> QueryProcessor = 0;
    virtual ~QueryEngine() {}

    /// Loads a query engine from the given paths.
    ///
    /// At the moment, the index does not contain its own integer encoding type, and thus it must
    /// also be provided here.
    [[nodiscard]] static auto
    load(std::string_view encoding, std::string const& index_path, std::string const& wand_data_path)
        -> std::unique_ptr<QueryEngine>;
};

}  // namespace pypisa
