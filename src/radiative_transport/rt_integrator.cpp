#include "geometry/vector3d.hpp"
#include "geometry/SphericalHarmonics.hpp"
#include "octree/octree.hpp"
#include "cosmology/cosmology.hpp"
#include "radiative_transport/rt_integrator.hpp" 

#include <cmath>


void RadiativeTransferIntegrator::addNode(
    const OctreeNode& node)
{
    Vector3D r = node.mass_center - observer_;

    double chi = r.norm();

    double weight =
        cosmology_.tau2a(chi)
      * 1 // TODO! EVOLUTION FUNCTION H(T)
      * node.total_mass;

    double theta = std::atan2(r.y, r.x);  // TODO!! CHANGE REALIZATION TO SIN, COS AND SPH HARMONICS SIMILARLY
    double phi   = std::asin(r.z / chi);

    // compute multipole momenta
    for (unsigned l = 0; l <= lmax_; ++l)
    {
        for (int m = -int(l); m <= int(l); ++m)
        {
            alm_[sph_.index(l,m)] += weight * sph_.Ylm(l,m,theta,phi);
        }
    }
}