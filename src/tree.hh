#pragma once
#include <cstdint>
#include <vector>
#include <raylib.h>
#include "vector.hh"

using namespace std;

struct Branch;
struct Segment;

struct Segment {
    Vector2 blp; // Bottom left  point
    Vector2 brp; // Bottom right point
    Vector2 tlp; // Top    left  point
    Vector2 trp; // Top    right point
};

struct Branch {
    vector<Branch> branches{};
    vector<Segment> segments{};
    int level = 0;
};

struct Tree {
    Branch root;
    uint32_t segmentCount;
};

class TreeGenerator {
    Branch getBranch(int depth, Vector2 origin, float width, float surfaceAngle, float directionRot, float startLength, float maxLength);
    static void addTriangles(Mesh& mesh, const Branch& branch, uint32_t& vertex);
    
public:
    float branchWidthFalloff = 0.65f;
    float branchLengthFalloff = 0.82f;
    float maxBranchLength = 900.0f;
    float branchDensity = 0.0064f;

    float segmentStartLength = 43.0f;
    float segmentStartWidth = 60.0f;
    float segmentWidthFalloff = 0.922f;
    float segmentLengthMultiplier = 1.09f;
    
    float rotationRandomness = 0.15f;

    int recursionLimit = 8;

    Tree getTree();
    static uint32_t countSegments(const Branch& branch);
    static Mesh generateMesh(const Tree& tree);
};

