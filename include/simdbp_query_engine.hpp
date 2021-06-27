#pragma once

#include <vector>

#include <index_types.hpp>
#include <wand_data.hpp>
#include <wand_data_raw.hpp>

#include "query_engine.hpp"

namespace pypisa {

class SimdBpQueryEngine: public QueryEngine {
  private:
    pisa::block_simdbp_index m_index;
    pisa::wand_data<pisa::wand_data_raw> m_wdata;

  public:
    SimdBpQueryEngine(std::string const& path, std::string const& wand_data_filename);
    auto processor(std::string_view algorithm, ScorerParams scorer_params, std::size_t k)
        -> QueryProcessor override;
    ~SimdBpQueryEngine();
};

}  // namespace pypisa
