#include "galaxy/galaxyReader.hpp"
#include "geometry/spherical.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>


namespace
{
    constexpr float DEG2RAD = static_cast<float>(M_PI) / 180.0f;

    template<typename T>
    T parseField(const std::string& line,
                 size_t start,
                 size_t length)
    {
        std::stringstream ss(
            line.substr(start, length)
        );

        T value{};
        ss >> value;

        return value;
    }
}


GalaxyCatalog BiteauReader::read(
    const std::string& filename
) const
{
    GalaxyCatalog catalog;

    std::ifstream file(filename);

    if (!file)
        throw std::runtime_error(
            "Cannot open Biteau catalog: " + filename
        );


    std::string line;

    while (std::getline(file, line))
    {
        // Data lines start with galaxy name,
        // header lines start with spaces or text
        if (line.empty() || line[0] == ' ')
            continue;

        if (line.size() < 138)
            continue;

        catalog.push_back(parseLine(line));
    }


    return catalog;
}


Galaxy BiteauReader::parseLine(
    const std::string& line
) const
{
    // RA and Dec are stored in degrees
    float ra_deg = parseField<float>(line, 30, 7);

    float dec_deg = parseField<float>(line, 38, 7);


    // Luminosity distance in Mpc
    float distance = parseField<float>(line, 64, 8);


    // Stellar mass and SFR are already log10 values
    float logMstar = parseField<float>(line, 75, 5);

    float logSFR = parseField<float>(line, 89, 5);


    Equatorial eq;

    eq.ra = ra_deg * DEG2RAD;
    eq.dec = dec_deg * DEG2RAD;
    eq.distance = distance;


    Galaxy galaxy;

    galaxy.position = toCartesian(eq);

    galaxy.logMstar = logMstar;
    galaxy.logSFR   = logSFR;

    return galaxy;
}


std::string BiteauReader::trim(
    const std::string& value
)
{
    size_t first =
        value.find_first_not_of(' ');

    if (first == std::string::npos)
        return "";

    size_t last =
        value.find_last_not_of(' ');

    return value.substr(
        first,
        last - first + 1
    );
}