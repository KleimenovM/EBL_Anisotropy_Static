#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "octree/octree.hpp"

namespace
{
float read_threshold(const std::string& text)
{
    std::size_t end;
    const float value = std::stof(text, &end);
    if (end != text.size() || !std::isfinite(value) || value < 0.0f)
    {
        throw std::invalid_argument("Expected a finite nonnegative value: " + text);
    }
    return value;
}

std::vector<Source> read_sources(const std::string& path, float half_size)
{
    std::ifstream input(path);
    if (!input)
    {
        throw std::runtime_error("Could not open catalog: " + path);
    }

    std::vector<Source> sources;
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line))
    {
        ++line_number;
        const auto first = line.find_first_not_of(" \t\r");
        if (first == std::string::npos || line[first] == '#')
        {
            continue;
        }

        std::istringstream row(line);
        Source source;
        auto& point = source.position;
        if (!(row >> point.x >> point.y >> point.z >> source.mass) ||
            !(row >> std::ws).eof() ||
            !(std::abs(point.x) <= half_size && std::abs(point.y) <= half_size &&
              std::abs(point.z) <= half_size) ||
            !std::isfinite(source.mass) || source.mass < 0.0f)
        {
            throw std::runtime_error("Invalid catalog row " + std::to_string(line_number));
        }
        sources.push_back(source);
    }
    if (input.bad())
    {
        throw std::runtime_error("Could not read catalog: " + path);
    }
    return sources;
}
}

int main(int argc, char** argv)
{
    if (argc != 6)
    {
        std::cerr << "Usage: export_octree CATALOG.txt TREE.bin HALF_SIZE MAX_MASS MIN_HALF_SIZE\n";
        return 1;
    }

    try
    {
        const float half_size = read_threshold(argv[3]);
        const float max_mass = read_threshold(argv[4]);
        const float min_half_size = read_threshold(argv[5]);
        if (half_size == 0.0f)
        {
            throw std::invalid_argument("HALF_SIZE must be positive");
        }

        const Octree tree(read_sources(argv[1], half_size), half_size, max_mass, min_half_size);
        std::ofstream output(argv[2], std::ios::binary);
        if (!output)
        {
            throw std::runtime_error(std::string("Could not open output: ") + argv[2]);
        }
        tree.write_binary(output);
        output.close();
        if (!output)
        {
            throw std::runtime_error(std::string("Could not finish writing: ") + argv[2]);
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
