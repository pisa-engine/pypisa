#include <index_types.hpp>
#include <memory_source.hpp>

#include "simdbp_query_engine.hpp"

#include "resolve_query_processor.hpp"

namespace pypisa {

SimdBpQueryEngine::SimdBpQueryEngine(std::string const& path, std::string const& wand_data_filename)
    : m_index(pisa::MemorySource::mapped_file(path)),
      m_wdata(pisa::MemorySource::mapped_file(wand_data_filename))
{}

auto SimdBpQueryEngine::processor(std::string_view algorithm, ScorerParams scorer_params, std::size_t k)
    -> QueryProcessor
{
    return resolve_query_processor(m_index, m_wdata, algorithm, scorer_params, k);
}

SimdBpQueryEngine::~SimdBpQueryEngine() {}

}  // namespace pypisa
