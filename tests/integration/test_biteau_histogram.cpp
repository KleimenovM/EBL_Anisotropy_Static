#include "galaxy/galaxyReader.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>


struct Histogram
{
    std::vector<double> bins;
    std::vector<size_t> counts;
};


Histogram makeHistogram(
    const std::vector<float>& values,
    size_t n_bins
)
{
    float min_value =
        *std::min_element(values.begin(), values.end());

    float max_value =
        *std::max_element(values.begin(), values.end());


    float width =
        (max_value - min_value) / n_bins;


    Histogram hist;

    hist.bins.resize(n_bins + 1);
    hist.counts.resize(n_bins, 0);


    for (size_t i = 0; i <= n_bins; i++)
    {
        hist.bins[i] =
            min_value + i * width;
    }


    for (float value : values)
    {
        size_t index =
            static_cast<size_t>(
                (value - min_value) / width
            );


        if (index >= n_bins)
            index = n_bins - 1;


        hist.counts[index]++;
    }


    return hist;
}


void saveHistogram(
    const Histogram& hist,
    const std::string& filename
)
{
    std::ofstream file(filename);

    for (size_t i = 0; i < hist.counts.size(); i++)
    {
        double center =
            0.5 * (hist.bins[i] + hist.bins[i + 1]);

        file
            << center
            << " "
            << hist.counts[i]
            << "\n";
    }

    std::cout << "Histogram saved to" << filename << std::endl;
}


int main()
{
    BiteauReader reader;


    auto catalog =
        reader.read(
            std::string(CATALOG_DIR) + "/table5_biteau2021.txt"
        );


    std::cout
        << "Loaded galaxies: "
        << catalog.size()
        << "\n";


    std::vector<float> masses;
    std::vector<float> sfrs;

    masses.reserve(catalog.size());
    sfrs.reserve(catalog.size());


    for (const auto& galaxy : catalog)
    {
        masses.push_back(galaxy.logMstar);
        sfrs.push_back(galaxy.logSFR);
    }


    constexpr size_t N_BINS = 100;


    auto mass_hist =
        makeHistogram(masses, N_BINS);

    auto sfr_hist =
        makeHistogram(sfrs, N_BINS);


    saveHistogram(
        mass_hist, std::string(OUTPUT_DIR) + "/histograms/mass_distribution.txt"
    );

    saveHistogram(
        sfr_hist, std::string(OUTPUT_DIR) + "/histograms/sfr_distribution.txt"
    );

    return 0;
}