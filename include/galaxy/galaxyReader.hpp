#include <vector>
#include "galaxy/galaxy.hpp"

using GalaxyCatalog = std::vector<Galaxy>;


class GalaxyReader
{
public:
    virtual ~GalaxyReader() = default;

    virtual GalaxyCatalog read(const std::string& filename) const = 0;
};

#pragma once


class BiteauReader : public GalaxyReader
{
public:

    GalaxyCatalog read(const std::string& filename) const override;

private:

    Galaxy parseLine(const std::string& line) const;

    static std::string trim(const std::string& value);
};
