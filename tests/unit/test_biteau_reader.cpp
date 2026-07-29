#include "galaxy/galaxyReader.hpp"
#include "geometry/spherical.hpp"
#include "cosmology/cosmology.hpp"

#include <iostream>
#include <cstdio>


constexpr float RAD2DEG = 180.0f / static_cast<float>(M_PI);


int main()
{   
    BiteauReader reader(standardFlatCosmology());

    std::string filename = std::string(TEST_DATA_DIR) + "/test_biteau_catalog.txt";

    GalaxyCatalog catalog =
        reader.read(filename);

    std::cout
        << "Number of galaxies: "
        << catalog.size()
        << std::endl;


    for (const auto& galaxy : catalog)
    {
        std::cout
            << "Position: "
            << galaxy.position.x << " "
            << galaxy.position.y << " "
            << galaxy.position.z
            << std::endl;

        std::cout
            << "logMstar = "
            << galaxy.logMstar
            << ", logSFR = "
            << galaxy.logSFR
            << std::endl;
    }


    // Simple checks
    if (catalog.size() != 2)
    {
        std::cerr << "ERROR: Wrong number of galaxies\n";
        return 1;
    }

    if (std::abs(catalog[0].logMstar - 10.76f) > 1e-5){
        std::cerr << "ERROR: Wrong stellar mass " << catalog[0].logMstar << std::endl;
        return 1;
    }

    if (std::abs(catalog[0].logSFR - 0.26f) > 1e-5){
        std::cerr << "ERROR: Wrong star formation rate " << catalog[0].logSFR << std::endl;
        return 1;
    }

    auto eq0 = toEquatorial(catalog[0].position);

    if (std::abs(eq0.ra * RAD2DEG - 11.893f) > 1e-5){
        std::cerr << "ERROR: Wrong RA value" << eq0.ra * RAD2DEG << " " << 11.893 << std::endl;
        return 1;
    }

    if (std::abs(eq0.dec * RAD2DEG - (-25.292)) > 1e-5){
        std::cerr << "ERROR: Wrong RA value" << eq0.dec * RAD2DEG << " " << -25.292 << std::endl;
        return 1;
    }

    if (std::abs(catalog[0].position.norm() - 3.70) > 1e-5){
        std::cerr << "ERROR: Wrong distance value" << catalog[0].position.norm() << " " << 3.70 << std::endl;
        return 1;
    }


    std::cout << "Test passed\n";

    return 0;
}