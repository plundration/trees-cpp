#pragma once
#include <vector>
#include <raylib.h>

using namespace std;

struct Branch {
    vector<Branch> branches{};
    int level = 0;
    Vector2 origin = {0,0};
    Vector2 dir = {0,0};
    float length = 0;
};

Branch generate_branch(int depth, int max, Vector2 origin, float rot, float length);
vector<Branch> generate_split_branches(int depth, int max, Vector2 origin, float rot, float length);
Branch generate_extension_branch(int depth, int max, Vector2 origin, float rot, float length);
Branch generate_tree(int max_recursion);

const float len_falloff = 0.62f;
const float split_falloff = 0.72f;
const float ext_falloff = 0.85f;
const float density = 0.00014f;
