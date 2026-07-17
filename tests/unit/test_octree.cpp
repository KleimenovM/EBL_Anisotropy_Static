#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include "octree/octree.hpp"


int main()
{
    std::vector<Source> sources;

    std::string filePath = std::string(TEST_DATA_DIR) + "/test_octree_catalog.txt";
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Cannot open file " << filePath << std::endl;
        return 1;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);

        float x, y, z, m;

        ss >> x >> y >> z >> m;

        sources.push_back(
            {Vector3D(x, y, z), m}
        );
    }

    std::cout << "Number of sources: " << static_cast<int>(sources.size()) << std::endl;

    Octree tree(
        sources,
        8.0,
        2,
        0.25
    );

    // Write sources
    std::ofstream source_output(std::string(OUTPUT_DIR) + "/octree/test_sources.txt");
    tree.write_sources_visualization(source_output);
    source_output.close();

    // Write tree construction log
    std::ofstream print_output(std::string(OUTPUT_DIR) + "/octree/test_output.txt");
    tree.print(print_output);
    print_output.close();

    // Write tree data for visualization
    std::ofstream visualization_output(std::string(OUTPUT_DIR) + "/octree/test_data.txt");
    tree.write_visualization(visualization_output);
    visualization_output.close();

    std::cout << "Nodes: " << tree.size() << std::endl;

    return 0;
}