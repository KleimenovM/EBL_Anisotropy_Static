#include <vector>
#include "galaxy/galaxy.hpp"
#include "cosmology/cosmology.hpp"

using GalaxyCatalog = std::vector<Galaxy>;


class GalaxyReader
{
public:
    explicit GalaxyReader(const Cosmology& cosmo)
        : cosmo(cosmo)
    {}

    virtual ~GalaxyReader() = default;

    virtual GalaxyCatalog read(const std::string& filename) const = 0;

protected:
    const Cosmology& cosmo;
};

#pragma once


class BiteauReader : public GalaxyReader
{    
public:

using GalaxyReader::GalaxyReader;

    GalaxyCatalog read(const std::string& filename) const override;

private:

    Galaxy parseLine(const std::string& line) const;

    static std::string trim(const std::string& value);
};
