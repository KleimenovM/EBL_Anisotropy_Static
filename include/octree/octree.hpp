#pragma once

#include <array>
#include <vector>

#include "geometry/vector3d.hpp"


struct Source
{
    Vector3D position;  // position of the source
    float mass;        // weight in the octree
};


struct OctreeNode
{
    // Geometrical properties
    Vector3D center;    // Node center
    float half_size;   // Half side length

    // Physical properties
    float total_mass;      // Total mass
    Vector3D mass_center;   // Mass center

    // Tree structure
    bool is_leaf;                     // True if the node has no children
    std::array<int,8> children;       // Indices of child nodes

    // Sources stored in leaf nodes
    std::vector<int> source_indices;

    OctreeNode(
        const Vector3D& c,
        float h
    );
};


class Octree
{
public:

    /*
     * Construct an octree from a source catalog.
     *
     * Parameters:
     *   sources        : input source catalog
     *   half_size      : [Mpc] octree half-size
     *   max_mass       : maximal mass per leaf
     *   min_size       : [Mpc] minimal size of the leaf
     */
    Octree(
        const std::vector<Source>& sources,
        float half_size,
        float max_mass,
        float min_size
    );

    // Compute radiation field contribution at a point.
    // The opening_angle controls the Barnes-Hut accuracy.
    float evaluate(
        const Vector3D& position,
        float opening_angle
    ) const;


    // Return number of nodes in the tree.
    size_t size() const;

    // Tree debugging function
    void print() const;

    void print(
        std::ofstream& output
    ) const;

    // Visualization writer.
    void write_visualization(
        std::ofstream& output
    ) const;

    void write_sources_visualization(
        std::ofstream& output
    ) const;

private:

    // Original source catalog. 
    // Nodes store only indices into this array.
    std::vector<Source> sources_;

    // Storage of all tree nodes.
    // Nodes refer to each other by indices.
    std::vector<OctreeNode> nodes_;

    // Index of the root node.
    int root_index_;

    // Half-size of the root cube [Mpc]
    float half_size_;

    // Maximum total mass allowed in a leaf
    int max_mass_;

    // Minimal size allowed for a leaf [Mpc]
    int min_size_;

private:

    // Create the initial root cube.
    void create_root();

    // Insert one source recursively.
    void insert_recursive(
        int node_index,
        int source_index
    );

    // Subdivide the existing node into 8 child nodes.
    void subdivide(
        int node_index
    );

    // Determine the child containing a position.
    int get_child_index(
        const OctreeNode& node,
        const Vector3D& position
    ) const;

    // Update mass information of a node.
    void update_node(
        OctreeNode& node,
        const Source& source
    );

    // Recursive Barnes-Hut evaluation.
    float evaluate_recursive(
        int node_index,
        const Vector3D& position,
        float opening_angle
    ) const;

    // Distance between two points.
    float distance(
        const Vector3D& a,
        const Vector3D& b
    ) const;

    // Recursive tree printer.
    void print_recursive(
        int node_index,
        int depth,
        std::ostream& output
    ) const;

    // Recursive visualization writer.
    void write_visualization_recursive(
        std::ofstream& output,
        int node_index,
        int parent_index
    ) const;
};