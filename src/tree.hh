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
    float branchDensity = 0.0071f;
    float maxBranchLength = 1000.0f;
    float branchWidthFalloff = 0.45f;
    float branchLengthFalloff = 0.52f;

    float segmentStartLength = 56.0f;
    float segmentStartWidth = 36.0f;
    float segmentWidthFalloff = 0.982f;
    float segmentLengthMultiplier = 1.008f;
    
    float rotationRandomness = 0.19f;
    
    float idealBranchAngle = 0.75f;
    float angleDistrib = 0.35f;

    int recursionLimit = 8;

    Tree getTree();
    static uint32_t countSegments(const Branch& branch);
    static Mesh generateMesh(const Tree& tree);
};

