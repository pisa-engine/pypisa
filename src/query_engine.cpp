#include "query_engine.hpp"

#include "ef_query_engine.hpp"
#include "single_query_engine.hpp"
#include "pefuniform_query_engine.hpp"
#include "pefopt_query_engine.hpp"
#include "block_optpfor_query_engine.hpp"
#include "block_varintg8iu_query_engine.hpp"
#include "block_streamvbyte_query_engine.hpp"
#include "block_maskedvbyte_query_engine.hpp"
#include "block_varintgb_query_engine.hpp"
#include "block_interpolative_query_engine.hpp"
#include "block_qmx_query_engine.hpp"
#include "block_simple8b_query_engine.hpp"
#include "block_simple16_query_engine.hpp"
#include "block_simdbp_query_engine.hpp"

namespace pypisa {

[[nodiscard]] auto QueryEngine::load(
    std::string_view encoding, std::string const& index_path, std::string const& wand_data_path)
    -> std::unique_ptr<QueryEngine>
{

    if (encoding == "ef") {
        return std::make_unique<EliasFanoQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "single") {
        return std::make_unique<SingleQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "pefuniform") {
        return std::make_unique<PartitionedEliasFanoUniformQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "pefopt") {
        return std::make_unique<PartitionedEliasFanoOptQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_optpfor") {
        return std::make_unique<OptPForQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_varintg8iu") {
        return std::make_unique<VarintG8IUQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_streamvbyte") {
        return std::make_unique<StreamVByteQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_maskedvbyte") {
        return std::make_unique<MaskedVByteQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_varintgb") {
        return std::make_unique<VarintGBQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_interpolative") {
        return std::make_unique<InterpolativeQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_qmx") {
        return std::make_unique<QmxQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_simple8b") {
        return std::make_unique<Simple8BQueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_simple16") {
        return std::make_unique<Simple16QueryEngine>(index_path, wand_data_path);
    }

    if (encoding == "block_simdbp") {
        return std::make_unique<SimdBpQueryEngine>(index_path, wand_data_path);
    }
    return nullptr;
}

}  // namespace pypisa
