#include <boost/algorithm/string.hpp>
#include <compress.hpp>
#include <fmt/format.h>
#include <forward_index_builder.hpp>
#include <invert.hpp>
#include <scorer/scorer.hpp>
#include <pybind11/pybind11.h>
#include <query/term_processor.hpp>
#include <wand_data.hpp>
#include <wand_utils.hpp>

namespace py = pybind11;

using pisa::Document_Record;
using pisa::Forward_Index_Builder;

static std::size_t const THREADS = 1;

void _index(
    std::istream& is,
    std::string const& index_dir,
    std::string const& format,
    std::size_t batch_size = 10'000)
{
    std::optional<std::string> stemmer = std::nullopt;
    pisa::Forward_Index_Builder fwd_builder;
    fwd_builder.build(
        is,
        fmt::format("{}/fwd", index_dir),
        [](std::istream& in) -> std::optional<Document_Record> {
            std::string line;
            if (std::getline(in, line) && not line.empty()) {
                return std::make_optional<Document_Record>("id", "text", "");
            }
            return std::nullopt;
        },
        pisa::term_processor_builder(stemmer),
        pisa::parse_plaintext_content,
        batch_size,
        THREADS);
    auto term_lexicon_file = fmt::format("{}/termlex", index_dir);
    mio::mmap_source mfile(term_lexicon_file.c_str());
    auto lexicon = pisa::Payload_Vector<>::from(mfile);
    pisa::invert::invert_forward_index(
        fmt::format("{}/fwd", index_dir),
        fmt::format("{}/inv", index_dir),
        lexicon.size(),
        batch_size,
        THREADS);
}

void _compress(
    std::string const& index_dir,
    std::string const& index_encoding,
    std::string scorer_name,
    size_t block_size,
    bool quantize)
{
    auto scorer = ScorerParams(scorer_name);
    pisa::create_wand_data(
        fmt::format("{}/bmw", index_dir),
        fmt::format("{}/inv", index_dir),
        pisa::FixedBlock(block_size),
        scorer,
        false,
        false,
        quantize,
        {});
    pisa::compress(
        fmt::format("{}/inv", index_dir),
        fmt::format("{}/bmw", index_dir),
        index_encoding,
        fmt::format("{}/inv.{}", index_dir, index_encoding),
        scorer,
        quantize,
        false);
}

void _search() {}

PYBIND11_MODULE(pypisa, m)
{
    m.def("index", &_index);
    m.def("compress", &_compress);
    m.def("search", &_search);
}
