#include <cmath>
#include <algorithm>

#include <iostream>
#include <string>

#include "octree/octree.hpp"
#include "geometry/vector3d.hpp"

namespace
{
    constexpr float FOUR_PI = 4.0f * static_cast<float>(M_PI);
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

    std::cout << "Node " << node_index << " is a leaf: " << nodes_[node_index].is_leaf << std::endl;

    // Leaf node
    if (nodes_[node_index].is_leaf)
    {
        // Leaf accepts another source
        if (nodes_[node_index].source_indices.empty() ||      // empty leaf
            nodes_[node_index].total_mass <= max_mass_ ||     // light enough leaf
            nodes_[node_index].half_size <= min_size_)        // minimal possible leaf
        {
            nodes_[node_index].source_indices.push_back(source_index);
            std::cout << "Node " << node_index << " accepts source " << source_index << "\n";
            return;
        }

        // Leaf cannot accept another source => Split leaf
        std::cout << "Node " << node_index << " is overfilled! Split the leaf" << "\n";
        auto old_indices = nodes_[node_index].source_indices;
        nodes_[node_index].source_indices.clear();
        subdivide(node_index);

        std::cout << "Produces children # ";
        for (int i = 0; i < 8; i++){
            std::cout << nodes_[node_index].children[i] << " ";
        }
        std::cout << std::endl;

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


float Octree::evaluate_recursive(
    int node_index,
    const Vector3D& position,
    float opening_angle
) const
{
    const OctreeNode& node = nodes_[node_index];

    // Empty node
    if (node.total_mass == 0.0)
        return 0.0;

    float d = distance(node.mass_center, position);

    // Avoid self-contribution
    if (d == 0.0)
        return 0.0;

    // Calculate IF
    if (node.is_leaf ||                                  // node is a leaf
        (2.0 * node.half_size / d) < opening_angle)      // node angular size is below the limit
    {
        return node.total_mass / (FOUR_PI * d * d);
    }

    // ELSE open the node
    float value = 0.0;

    // and add contributions from all children
    for (int i = 0; i < 8; i++)
    {
        if (node.children[i] != -1)
        {
            value += evaluate_recursive(
                node.children[i],
                position,
                opening_angle
            );
        }
    }

    return value;
}


float Octree::evaluate(
    const Vector3D& position,
    float opening_angle
) const
{
    return evaluate_recursive(
        root_index_,
        position,
        opening_angle
    );
}


void Octree::print() const
{   
    print_recursive(root_index_, 0);
}

void Octree::print_recursive(
    int node_index,
    int depth
) const
{   
    const OctreeNode& node = nodes_[node_index];

    std::string indent(depth * 2, ' ');

    std::cout << indent << "Node " << node_index << "\n";

    std::cout << indent << "center = " << node.center << "\n";

    std::cout << indent << "half_size = " << node.half_size << "\n";

    std::cout << indent << "mass = " << node.total_mass << "\n";

    std::cout << indent << "mass_center = " << node.mass_center << "\n";

    std::cout << indent << "leaf = " << (node.is_leaf ? "true" : "false") << "\n";

    if (node.is_leaf){
        std::cout << indent << "sources: ";

        for (int index : node.source_indices){
            std::cout << index << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    if (!node.is_leaf){
        for (int i = 0; i < 8; i++){
            if (node.children[i] != -1){
                print_recursive(node.children[i], depth + 1);
            }
        }
    }
}
