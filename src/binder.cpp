#include <boost/algorithm/string.hpp>
#include <forward_index_builder.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

using pisa::Document_Record;
using pisa::Forward_Index_Builder;

static std::size_t const THREADS = 2;

void parse(std::string const &fwd, std::string const &format,
           std::size_t batch_size = 10'000) {
  pisa::Forward_Index_Builder fwd_builder;
  fwd_builder.build(
      std::cin, fwd,
      [](std::istream &in) -> std::optional<Document_Record> {
        std::string line;
        if (std::getline(in, line) && not line.empty()) {
          return std::make_optional<Document_Record>("id", "text", "");
        }
        return std::nullopt;
      },
      [](std::string &&term) -> std::string {
        boost::algorithm::to_lower(term);
        return std::move(term);
      },
      pisa::parse_plaintext_content, batch_size, THREADS);
}

PYBIND11_MODULE(pypisa, m) {
  m.def("parse", &parse);
}
