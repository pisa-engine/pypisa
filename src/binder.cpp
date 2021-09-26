#include <compress.hpp>
#include <fmt/format.h>
#include <forward_index_builder.hpp>
#include <invert.hpp>
#include <io.hpp>
#include <parser.hpp>
#include <payload_vector.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <query/queries.hpp>
#include <query/term_processor.hpp>
#include <scorer/scorer.hpp>
#include <wand_data.hpp>
#include <wand_utils.hpp>

#include <query_engine.hpp>

namespace py = pybind11;

using pisa::Document_Record;
using pisa::Forward_Index_Builder;

static std::size_t const THREADS = 1;

void _index(
    std::string const& input_file,
    std::string const& index_dir,
    std::string const& format,
    std::size_t batch_size = 10'000)
{
    std::ifstream is(input_file);
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
        THREADS + 1);
    auto term_lexicon_file = fmt::format("{}/fwd.termlex", index_dir);
    mio::mmap_source mfile(term_lexicon_file.c_str());
    auto lexicon = pisa::Payload_Vector<>::from(mfile);
    pisa::invert::invert_forward_index(
        fmt::format("{}/fwd", index_dir),
        fmt::format("{}/inv", index_dir),
        batch_size,
        THREADS,
        lexicon.size());
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

void _compress_from_basename(
    std::string const& index_basename,
    std::string const& output_basename,
    std::string const& index_encoding,
    std::string scorer_name,
    size_t block_size,
    bool quantize)
{
    auto scorer = ScorerParams(scorer_name);
    pisa::create_wand_data(
        fmt::format("{}.bmw", output_basename),
        fmt::format("{}", index_basename),
        pisa::FixedBlock(block_size),
        scorer,
        false,
        false,
        quantize,
        {});
    pisa::compress(
        fmt::format("{}", index_basename),
        fmt::format("{}.bmw", output_basename),
        index_encoding,
        fmt::format("{}.{}.idx", output_basename, index_encoding),
        scorer,
        quantize,
        false);
}

void _build_lexicon(std::string const& in_file, std::string const& out_file)
{
    std::ifstream ifs(in_file);
    pisa::encode_payload_vector(
        std::istream_iterator<pisa::io::Line>(ifs), std::istream_iterator<pisa::io::Line>())
        .to_file(out_file);
}

auto _search(
    std::string const& index_path,
    std::string const& encoding,
    std::vector<std::string> const& queries,
    std::string const& algorithm,
    std::size_t k,
    std::optional<std::string> const& wand_data_path,
    bool wand_data_compessed,
    std::optional<std::string> const& scorer,
    std::string const& term_lexicon,
    std::optional<std::string> const& stemmer) -> std::vector<std::vector<pypisa::QueryResult>>
{
    if (!scorer) {
        throw std::invalid_argument("queries without scoring are not currently supported");
    }
    if (scorer && !wand_data_path) {
        throw std::invalid_argument("WAND data file is required for scored queries");
    }
    auto engine = pypisa::QueryEngine::load(encoding, index_path, wand_data_path.value());
    if (!engine) {
        throw std::invalid_argument("invalid encoding: " + encoding);
    }
    auto proc = engine->processor(algorithm, ScorerParams(scorer.value()), k);

    std::vector<::pisa::Query> parsed_queries;
    auto parse_query = resolve_query_parser(parsed_queries, term_lexicon, std::nullopt, stemmer);
    std::for_each(queries.begin(), queries.end(), parse_query);

    std::vector<std::vector<pypisa::QueryResult>> results;
    std::transform(
        parsed_queries.begin(),
        parsed_queries.end(),
        std::back_inserter(results),
        [&proc](auto&& query) { return proc(query); });

    return results;
}

PYBIND11_MODULE(pypisa, m)
{
    m.def("index", &_index);
    m.def("compress", &_compress);
    m.def("compress_from_basename", &_compress_from_basename);
    m.def("build_lexicon", &_build_lexicon);
    m.def(
        "search",
        &_search,
        "Executes queries.",
        py::arg("index_path"),
        py::arg("encoding"),
        py::arg("queries"),
        py::arg("algorithm"),
        py::arg("k"),
        py::arg("wand_data_path"),
        py::arg("wand_data_compessed"),
        py::arg("scorer"),
        py::arg("term_lexicon"),
        py::arg("stemmer"));
}
