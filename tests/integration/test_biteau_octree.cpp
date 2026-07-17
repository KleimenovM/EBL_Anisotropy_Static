#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

#include "octree/octree.hpp"
#include "galaxy/galaxyReader.hpp"


int main()
{
    BiteauReader reader;

    auto catalog = reader.read(std::string(CATALOG_DIR) + "/table5_biteau2021.txt");

    // reduce the size of the catalog
    std::vector<Source> catalog_reduced;

    for (int i = 0; i < static_cast<int>(catalog.size()); i = i+4000)
    {   
        // Define the quantity stored in the octree
        float mass = powf(10.0f, catalog[i].logMstar);  // mass (in M_sun)
        catalog_reduced.push_back({catalog[i].position, mass});
    }

    std::cout << "Number of sources: " << static_cast<int>(catalog.size()) << std::endl;

    Octree tree(
        catalog_reduced,
        400.0,
        1e12,
        5.0
    );

    // Write sources
    std::ofstream source_output(std::string(OUTPUT_DIR) + "/octree/biteau_sources.txt");
    tree.write_sources_visualization(source_output);
    source_output.close();

    // Write tree construction log
    std::ofstream print_output(std::string(OUTPUT_DIR) + "/octree/biteau_output.txt");
    tree.print(print_output);
    print_output.close();

    // Write tree data for visualization
    std::ofstream visualization_output(std::string(OUTPUT_DIR) + "/octree/biteau_data.txt");
    tree.write_visualization(visualization_output);
    visualization_output.close();

    std::cout << "Nodes: " << tree.size() << std::endl;

    return 0;
}