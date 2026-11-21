#include <cmath>
#include <algorithm>

#include <climits>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

#include "octree/octree.hpp"
#include "geometry/vector3d.hpp"

namespace
{
static_assert(CHAR_BIT == 8 && sizeof(float) == 4 &&
              std::numeric_limits<float>::is_iec559,
              "OCT1 requires 8-bit bytes and IEEE 754 binary32 floats");

void store_uint32_le(unsigned char* bytes, std::uint32_t value)
{
    for (int i = 0; i < 4; ++i)
    {
        bytes[i] = static_cast<unsigned char>(value >> (8 * i));
    }
}

void store_float32_le(unsigned char* bytes, float value)
{
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    store_uint32_le(bytes, bits);
}
}


OctreeNode::OctreeNode(
    const Vector3D& c,
    float h
)
    :
    center(c),
    half_size(h),
    total_mass(0.0),
    mass_center(0.0, 0.0, 0.0),
    is_leaf(true),
    children({-1, -1, -1, -1,
              -1, -1, -1, -1}),
    source_indices()
{
}


Octree::Octree(
    const std::vector<Source>& sources,
    float half_size,
    float max_mass,
    float min_size
)
    :
    sources_(sources),
    nodes_(),
    root_index_(0),
    half_size_(half_size),
    max_mass_(max_mass),
    min_size_(min_size)
{
    create_root();

    for (int i = 0; i < static_cast<int>(sources_.size()); i++)
    {
        insert_recursive(root_index_, i);
    }
}

size_t Octree::size() const
{
    return nodes_.size();
}

float Octree::distance(
    const Vector3D& a,
    const Vector3D& b
) const
{
    return (a - b).norm();
}

void Octree::create_root()
{
    Vector3D center(0.0, 0.0, 0.0);
    nodes_.emplace_back(center,half_size_);
    root_index_ = 0;
}

void Octree::insert_recursive(
    int node_index,
    int source_index
)
{
    const Source& source = sources_[source_index];

    // increase the node mass and shift the mass center
    update_node(nodes_[node_index],source);

    // std::cout << "Node " << node_index << " is a leaf: " << nodes_[node_index].is_leaf << std::endl;

    // Leaf node
    if (nodes_[node_index].is_leaf)
    {
        // Leaf accepts another source
        if (nodes_[node_index].source_indices.empty() ||      // empty leaf
            nodes_[node_index].total_mass <= max_mass_ ||     // light enough leaf
            nodes_[node_index].half_size <= min_size_)        // minimal possible leaf
        {
            nodes_[node_index].source_indices.push_back(source_index);
            // std::cout << "Node " << node_index << " accepts source " << source_index << "\n";
            return;
        }

        // Leaf cannot accept another source => Split leaf
        // std::cout << "Node " << node_index << " is overfilled! Split the leaf" << "\n";
        auto old_indices = nodes_[node_index].source_indices;
        nodes_[node_index].source_indices.clear();
        subdivide(node_index);

        // std::cout << "Produces children # ";
        // for (int i = 0; i < 8; i++){
        //     std::cout << nodes_[node_index].children[i] << " ";
        // }
        // std::cout << std::endl;

        // Redistribute old sources
        for (int old_index : old_indices)
        {
            insert_recursive(
                get_child_index(nodes_[node_index], sources_[old_index].position),
                old_index
            );
        }
    }

    // Internal node: propagate source downward
    insert_recursive(
        get_child_index(nodes_[node_index], source.position),
        source_index
    );
}

/*
    Subdivide the existing node into 8 child nodes.
    Child numbering
        0: (-,-,-), 1: (+,-,-), 2: (-,+,-), 3: (+,+,-)
        4: (-,-,+), 5: (+,-,+), 6: (-,+,+), 7: (+,+,+)
*/
void Octree::subdivide(
    int node_index
)
{
    float h = nodes_[node_index].half_size / 2.0;
    Vector3D parent_center = nodes_[node_index].center;

    for (int i = 0; i < 8; i++)
    {
        Vector3D center = parent_center;
        
        // condition ? value_if_true : value_if_false
        center.x += (i & 1) ? h : -h;  // first bit defines +- x
        center.y += (i & 2) ? h : -h;  // central bit +- y
        center.z += (i & 4) ? h : -h;  // last bit +- z
        
        int child_index = nodes_.size();
        
        nodes_.emplace_back(center, h);     // add the child to all nodes
        nodes_[node_index].children[i] = child_index;   // Store the index of the newly created child
    }

    nodes_[node_index].is_leaf = false;
}

int Octree::get_child_index(
    const OctreeNode& node,
    const Vector3D& position
) const
{
    int child = 0;

    if (position.x >= node.center.x)
        child |= 1;  // set the first bit

    if (position.y >= node.center.y)
        child |= 2;

    if (position.z >= node.center.z)
        child |= 4;

    return node.children[child];
}


void Octree::update_node(
    OctreeNode& node,
    const Source& source
)
{   
    float new_mass = node.total_mass + source.mass;

    // shift the center of mass
    if (new_mass > 0.0){
        node.mass_center =
            (node.mass_center * node.total_mass +
             source.position * source.mass) / new_mass;
    }

    // change the mass
    node.total_mass = new_mass;
}


void Octree::print() const
{
    print_recursive(
        root_index_,
        0,
        std::cout
    );
}


void Octree::print(
    std::ofstream& output
) const
{
    print_recursive(
        root_index_,
        0,
        output
    );
}

void Octree::print_recursive(
    int node_index,
    int depth,
    std::ostream& output
) const
{   
    const OctreeNode& node = nodes_[node_index];

    std::string indent(depth * 2, ' ');

    output << indent << "Node " << node_index << "\n";

    output << indent << "center = " << node.center << "\n";

    output << indent << "half_size = " << node.half_size << "\n";

    output << indent << "mass = " << node.total_mass << "\n";

    output << indent << "mass_center = " << node.mass_center << "\n";

    output << indent << "leaf = " << (node.is_leaf ? "true" : "false") << "\n";

    if (node.is_leaf){
        output << indent << "sources: ";

        for (int index : node.source_indices){
            output << index << " ";
        }
        output << "\n";
    }
    output << "\n";

    if (!node.is_leaf){
        for (int i = 0; i < 8; i++){
            if (node.children[i] != -1){
                print_recursive(node.children[i], depth + 1, output);
            }
        }
    }
}

void Octree::write_visualization(
    std::ofstream& output
) const
{
    output << "# node_id parent_id cx cy cz mass half_size leaf\n";

    write_visualization_recursive(
        output,
        root_index_,
        -1
    );
}


void Octree::write_visualization_recursive(
    std::ofstream& output,
    int node_index,
    int parent_index
) const
{
    const OctreeNode& node = nodes_[node_index];

    output
        << node_index << " "
        << parent_index << " "
        << node.center.x << " "
        << node.center.y << " "
        << node.center.z << " "
        << node.total_mass << " "
        << node.half_size << " "
        << (node.is_leaf ? 1 : 0)
        << "\n";


    if (!node.is_leaf)
    {
        for (int i = 0; i < 8; i++)
        {
            if (node.children[i] != -1)
            {
                write_visualization_recursive(
                    output,
                    node.children[i],
                    node_index
                );
            }
        }
    }
}

void Octree::write_sources_visualization(
    std::ofstream& output
) const
{
    output << "# x y z mass\n";

    for (const Source& source : sources_)
    {
        output
            << source.position.x << " "
            << source.position.y << " "
            << source.position.z << " "
            << source.mass
            << "\n";
    }
}


void Octree::write_binary(std::ostream& output) const
{
    // Every split adds exactly eight nodes to the root's initial count of one.
    const auto split_count = (nodes_.size() - 1) / 8;
    const auto max_count = std::numeric_limits<std::uint32_t>::max();
    if (split_count > max_count || sources_.size() > max_count)
    {
        throw std::length_error("Octree exceeds the OCT1 count limits");
    }

    std::array<unsigned char, 16> bytes{'O', 'C', 'T', '1'};
    store_uint32_le(bytes.data() + 4, static_cast<std::uint32_t>(split_count));
    store_uint32_le(bytes.data() + 8, static_cast<std::uint32_t>(sources_.size()));
    store_float32_le(bytes.data() + 12, nodes_[root_index_].half_size);
    output.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

    if (split_count != 0)
    {
        write_split_masks(output, root_index_);
    }

    for (const Source& source : sources_)
    {
        store_float32_le(bytes.data(), source.position.x);
        store_float32_le(bytes.data() + 4, source.position.y);
        store_float32_le(bytes.data() + 8, source.position.z);
        store_float32_le(bytes.data() + 12, source.mass);
        output.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    }

    if (!output)
    {
        throw std::ios_base::failure("Could not write OCT1 octree");
    }
}


void Octree::write_split_masks(std::ostream& output, int node_index) const
{
    const auto& children = nodes_[node_index].children;
    unsigned char mask = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (!nodes_[children[i]].is_leaf)
        {
            mask |= static_cast<unsigned char>(1u << i);
        }
    }
    output.write(reinterpret_cast<const char*>(&mask), sizeof(mask));

    for (int i = 0; i < 8; ++i)
    {
        if (mask & (1u << i))
        {
            write_split_masks(output, children[i]);
        }
    }
}
