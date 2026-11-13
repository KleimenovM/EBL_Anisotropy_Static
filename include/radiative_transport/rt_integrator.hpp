#include "geometry/vector3d.hpp"
#include "geometry/SphericalHarmonics.hpp"
#include "octree/octree.hpp"
#include "cosmology/cosmology.hpp"


class RadiativeTransferIntegrator
{
public:

    RadiativeTransferIntegrator(
        const Octree& tree,
        const Cosmology& cosmology,
        const SphericalHarmonics& sph
    );

    void compute(
        const Vector3D& observer
    );

    float monopole() const;

    const std::vector<double>& alm() const;

private:

    void addNode(const OctreeNode& node);

    const Octree& tree_;
    const Cosmology& cosmology_;
    const SphericalHarmonics& sph_;

    Vector3D observer_;

    unsigned lmax_;

    std::vector<double> alm_;
};