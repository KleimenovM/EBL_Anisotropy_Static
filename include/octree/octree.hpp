#pragma once

#include <array>
#include <iosfwd>
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
     *   max_mass       : mass threshold for splitting a populated leaf
     *   min_size       : [Mpc] leaves at or below this half-size stop splitting
     */
    Octree(
        const std::vector<Source>& sources,
        float half_size,
        float max_mass,
        float min_size
    );

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

    // Write the OCT1 format to a stream opened in binary mode.
    // The 16-byte little-endian header holds the ASCII magic OCT1, uint32
    // split-node count, uint32 source count, and float32 root half-size.
    // The root center is (0, 0, 0). Each split node contributes one byte in
    // preorder, with bit i set if child i also splits. The child index uses
    // bits 0, 1, and 2 for the positive x, y, and z sides of the parent,
    // including ties on the split planes. A zero split-node count denotes a
    // leaf root. Source records follow in original catalog order, with four
    // little-endian float32 values per record (x, y, z, mass).
    void write_binary(std::ostream& output) const;

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

    // Mass threshold for splitting a populated leaf.
    float max_mass_;

    // Half-size cutoff for splitting a leaf [Mpc].
    float min_size_;

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

    void write_split_masks(std::ostream& output, int node_index) const;
};
