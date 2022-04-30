#include "tree.hh"

#include <cmath>
#include <cstdlib>
#include "vector.hh"

float rand_float(float min, float max) { return min + static_cast<float>(rand()/static_cast<float>(RAND_MAX)*(max - min)); }
float rand_rot() { return rand() % 2 == 0 ? rand_float(0.4f, 1.2f) : -rand_float(0.4f, 1.2f); }

Branch generate_branch(int depth, int max, Vector2 origin, float rot, float length) {
    Branch self = Branch();
    self.origin = origin;
    self.dir = {cos(rot), -sin(rot)};
    self.level = depth;
    self.length = length;

    if (depth + rand() % 2 >= max) return self;
    if (length/depth < 4.0f) return self;

    for (int i = 0; i < self.length * density * pow(1.03f, depth/8.8f) + 1; i++) {
        Vector2 b_origin = origin + self.dir * length * rand_float(0.1, 0.9);
        self.branches.push_back(
            generate_branch(depth+1, max, b_origin, rot+rand_rot(), length * len_falloff * rand_float(0.7f, 1.2f))
        );
    }
  
    if (length < 14.0f) return self;
    
    if (rand() % 2 == 0) {
        self.branches.push_back(
            generate_extension_branch(depth+1, max, self.origin + self.dir * self.length, rot, self.length)       
        );
    }
    else {
        vector<Branch> splits = 
            generate_split_branches(depth+1, max, self.origin + self.dir * self.length, rot, self.length);

        for (int i = 0; i < splits.size(); i++) {
            self.branches.push_back(splits[i]);
        }
    }

    return self;
}

vector<Branch> generate_split_branches(int depth, int max, Vector2 origin, float rot, float length) {
    auto branches = vector<Branch>();
    auto rand_angle = rand_float(0.3f, 0.8f);
    
    branches.push_back(generate_branch(depth, max, origin, rot+rand_angle, length * split_falloff * rand_float(0.9f, 1.1f)));
    branches.push_back(generate_branch(depth, max, origin, rot-rand_angle, length * split_falloff * rand_float(0.9f, 1.1f)));
    
    return branches;
}

Branch generate_extension_branch(int depth, int max, Vector2 origin, float rot, float length) {
    float rand_rot = rand() % 2 ? rand_float(0.2f, 0.6f) : -rand_float(0.2f, 0.6f);
    return generate_branch(depth, max, origin, rot + rand_rot, length * ext_falloff * rand_float(0.9f, 1.1f));
}

Branch generate_tree(int max) {
    Branch tree = Branch();
    tree.origin = {0, 500};
    float r = rand_float(M_PI/2.0f - 0.25f, M_PI/2.0f + 0.25f);
    tree.dir = {cos(r),-sin(r)};
    tree.length = 350.0f;
    
    if (rand() % 2 == 0) {
        tree.branches.push_back(
            generate_extension_branch(1, max, tree.origin + tree.dir * tree.length, r, tree.length * ext_falloff)
        );
    } else {
        vector<Branch> splits = 
            generate_split_branches(1, max, tree.origin + tree.dir * tree.length, r, tree.length * split_falloff);

        for (int i = 0; i < splits.size(); i++) {
            tree.branches.push_back(splits[i]);
        }
    }

    return tree;
}
