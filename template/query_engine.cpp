#include "query_engine.hpp"

#include "simdbp_query_engine.hpp"

namespace pypisa {

[[nodiscard]] auto QueryEngine::load(
    std::string_view encoding, std::string const& index_path, std::string const& wand_data_path)
    -> std::unique_ptr<QueryEngine>
{
    if (encoding == "block_simdbp") {
        return std::make_unique<SimdBpQueryEngine>(index_path, wand_data_path);
    }
    return nullptr;
}

}  // namespace pypisa
