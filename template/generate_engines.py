import os
from pathlib import Path

def load_index_list(path):
    with open(path) as file:
        content = file.read()
    return [tuple(l.strip().split()) for l in content.splitlines()]


def load(path) -> str:
    with open(path) as file:
        return file.read()


def generate(header_template, cpp_template, *, index, cls, output_dir):
    header = header_template.replace("@class@", cls).replace("@index@", index)
    cpp = cpp_template.replace("@class@", cls).replace("@index@", index)
    header_file = output_dir / "include" / f"{index}_query_engine.hpp"
    cpp_file = output_dir / "src" / f"{index}_query_engine.cpp"
    with open(header_file, "w") as file:
        file.write(header)
    with open(cpp_file, "w") as file:
        file.write(cpp)


def includes(indexes) -> str:
    return "\n".join([f"#include \"{index}_query_engine.hpp\"" for index, _ in indexes])


def body(indexes) -> str:
    ifstmt = """
    if (encoding == "@index@") {
        return std::make_unique<@class@QueryEngine>(index_path, wand_data_path);
    }"""
    return "\n".join([
        ifstmt.replace("@class@", cls).replace("@index@", index) for index, cls in indexes
    ])

def generate_engines():
    input_dir = Path(os.path.dirname(__file__))

    header_template = load(input_dir / "query_engine_impl.hpp.in")
    cpp_template = load(input_dir / "query_engine_impl.cpp.in")

    indexes = load_index_list(input_dir / "index-types.txt")

    output_dir = input_dir / ".."
    for index, cls in indexes:
        generate(header_template, cpp_template, cls=cls, index=index, output_dir=output_dir)

    query_engine_template = load(input_dir / "query_engine.cpp.in")
    engine = (
        query_engine_template
        .replace("@includes@", includes(indexes))
        .replace("@body@", body(indexes))
    )
    with open(output_dir / "src" / "query_engine.cpp", "w") as file:
        file.write(engine)

    with open(output_dir / "include" / "indexes.hpp", "w") as file:
        file.write(includes(indexes))


if __name__ == "__main__":
    generate_engines()
