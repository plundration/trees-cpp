#include "tree.hh"

#include <cmath>
#include <cstdlib>
#include "vector.hh"

float rand_float(float min, float max) {
    return min + static_cast<float>(rand()/static_cast<float>(RAND_MAX)*(max - min));
}

float rand_rot() {
    return rand() % 2 == 0 ? rand_float(-0.6f, -0.2f) : rand_float(0.2f, 0.6f);
}

Branch generate_branch(int depth, int max, Vector2 origin, float rot, float length) {
    Branch self = Branch();
    self.origin = origin;
    self.dir = {cos(rot), -sin(rot)};
    self.level = depth;
    self.length = length;
    
    if (depth == max) return self;


    for (int i = 0; i < self.length * density * pow(1.8f, depth/2); i++) {
        Vector2 b_origin = origin + self.dir * length * rand_float(0.0, 1.0);
        self.branches.push_back(generate_branch(depth+1, max, b_origin, rot+rand_rot(), length * len_falloff * rand_float(0.7f, 1.3f)));
    }

    return self;
}

Branch generate_tree(int max_recursion) {
    Branch tree = Branch();
    tree.origin = {0, 500};
    tree.dir = {0,-1};
    tree.length = 400.0f;

    for (int i = 0; i < 4; i++) {
        tree.branches.push_back(generate_branch(1, max_recursion, tree.dir*tree.length, 3.1415/2.0f + rand_rot(), tree.length * len_falloff * rand_float(0.7f, 1.3f)));
    }

    return tree;
}
