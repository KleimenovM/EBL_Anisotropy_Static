#pragma once

#include <vector>

class Lookup
{
public:
    Lookup(std::vector<double> x, std::vector<double> y);

    double operator()(double x) const;

    std::size_t size() const { return x_.size(); }

    const std::vector<double>& x() const { return x_; }
    const std::vector<double>& y() const { return y_; }

private:
    std::vector<double> x_;
    std::vector<double> y_;
};