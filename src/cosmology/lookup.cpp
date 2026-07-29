#include "cosmology/lookup.hpp"

#include <algorithm>
#include <stdexcept>

Lookup::Lookup(std::vector<double> x,
               std::vector<double> y)
    : x_(std::move(x)),
      y_(std::move(y))
{
    if (x_.size() != y_.size())
        throw std::invalid_argument("Lookup: x and y have different sizes.");

    if (x_.size() < 2)
        throw std::invalid_argument("Lookup: at least two points are required.");

    for (std::size_t i = 1; i < x_.size(); ++i)
    {
        if (x_[i] <= x_[i - 1])
            throw std::invalid_argument("Lookup: x values must be strictly increasing.");
    }
}

double Lookup::operator()(double x) const
{
    if (x <= x_.front())
        return y_.front();

    if (x >= x_.back())
        return y_.back();

    auto it = std::lower_bound(x_.begin(), x_.end(), x);

    std::size_t i = std::distance(x_.begin(), it);

    double x0 = x_[i - 1];
    double x1 = x_[i];

    double y0 = y_[i - 1];
    double y1 = y_[i];

    double u = (x - x0) / (x1 - x0);

    return y0 + u * (y1 - y0);
}