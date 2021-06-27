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

// using ef_index = freq_index<compact_elias_fano, positive_sequence<strict_elias_fano>>;

// using single_index = freq_index<indexed_sequence, positive_sequence<>>;

// using pefuniform_index =
//     freq_index<uniform_partitioned_sequence<>,
//     positive_sequence<uniform_partitioned_sequence<strict_sequence>>>;

// using pefopt_index =
//     freq_index<partitioned_sequence<>, positive_sequence<partitioned_sequence<strict_sequence>>>;

// using block_optpfor_index = block_freq_index<pisa::optpfor_block>;
// using block_varintg8iu_index = block_freq_index<pisa::varint_G8IU_block>;
// using block_streamvbyte_index = block_freq_index<pisa::streamvbyte_block>;
// using block_maskedvbyte_index = block_freq_index<pisa::maskedvbyte_block>;
// using block_varintgb_index = block_freq_index<pisa::varintgb_block>;
// using block_interpolative_index = block_freq_index<pisa::interpolative_block>;
// using block_qmx_index = block_freq_index<pisa::qmx_block>;
// using block_simple8b_index = block_freq_index<pisa::simple8b_block>;
// using block_simple16_index = block_freq_index<pisa::simple16_block>;
// using block_simdbp_index = block_freq_index<pisa::simdbp_block>;
