#pragma once
#include <vector>
#include <raylib.h>
#include "vector.hh"

using namespace std;

struct Branch;
struct Segment;

struct Triangle {
    Vector2 points[3];
};

struct Segment {
    Vector2 blp; // Bottom left  point
    Vector2 brp; // Bottom right point
    Vector2 tlp; // Top    left  point
    Vector2 trp; // Top    right point

    pair<Vector2, Vector2> getPointsOnLeft();
    float getLength();   
};

struct Branch {
    vector<Branch> branches{};
    vector<Segment> segments{};
    int level = 0;
    Vector2 origin = {0,0};
    Vector2 dir = {0,0};
    float length = 0;
    
    
    // 
    
    Triangle getSegmentTriangles(int i);
};

class TreeGenerator {
    Branch getBranch(int depth, Vector2 origin, float width, float surfaceAngle, float directionRot, float startLength, float maxLength);
    void extendBranch(Branch& branch);
    
public:
    float branchWidthFalloff = 0.69f;
    float branchLengthFalloff = 0.62f;
    float maxBranchLength = 1000.0f;
    float branchDensity = 0.00014f;

    float segmentStartLength = 38.0f;
    float segmentStartWidth = 60.0f;
    float segmentWidthFalloff = 0.882f;
    float segmentLengthMultiplier = 1.29f;
    
    float rotationRandomness = 0.15f;

    int recursionLimit = 8;

    Branch getTree();
};


Branch generate_branch(int depth, int max, Vector2 origin, float rot, float length);
vector<Branch> generate_split_branches(int depth, int max, Vector2 origin, float rot, float length);
Branch generate_extension_branch(int depth, int max, Vector2 origin, float rot, float length);
Branch generate_tree(int max_recursion);

const float len_falloff = 0.62f;
const float split_falloff = 0.72f;
const float ext_falloff = 0.85f;
const float density = 0.2f;
