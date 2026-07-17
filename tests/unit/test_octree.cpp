#include <iostream>

#include "octree.hpp"

int main()
{
    std::vector<Source> sources =
    {
        {{-7,-7,-7},1},
        {{-6,-6,-6},1},
        {{-5,-5,-5},1},

        {{ 6, 6, 6},1},
        {{ 7, 7, 7},1},

        {{-7, 7,-7},1},
        {{ 7,-7, 7},1},

        {{0.5,0.5,0.5},1},
        {{0.7,0.7,0.7},1},
        {{0.9,0.9,0.9},1}
    };

    Octree tree(
        sources,
        8.0,
        4,
        2.0
    );

    tree.print();
}