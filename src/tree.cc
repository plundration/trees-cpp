#include "tree.hh"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include "vector.hh"

float randF(float min, float max) { return min + static_cast<float>(rand()/static_cast<float>(RAND_MAX)*(max - min)); }
int randI(int min, int max) { return min + (rand() % (max-min+1)); }
float randRotation() { return rand() % 2 == 0 ? randF(0.4f, 1.2f) : -randF(0.4f, 1.2f); }
float randDistribution(float extrema) { return randF(-extrema, extrema); }
bool randChoice() { return rand()%2 == 0; }

Branch TreeGenerator::getBranch(int depth, Vector2 origin, float width, float surfaceAngle, float segmentRot, float length, float maxLength) {
    Branch branch = Branch();
    branch.level = depth;
    
    Segment segment;
    
    float topWidth = width * segmentWidthFalloff * sin(segmentRot);
    
    float baseRot = surfaceAngle;
    float topRot = baseRot + segmentRot - M_PI_2 + randDistribution(rotationRandomness);
    
    Vector2 bottomTang = {cos(baseRot), -sin(baseRot)};
    Vector2 topTang = {cos(topRot), -sin(topRot)};
    
    Vector2 dir = {cos(baseRot + segmentRot), -sin(baseRot + segmentRot)};
    Vector2 topMidp = origin + dir * length;
    
    // Assign new points to segment
    segment.blp = origin - bottomTang * width / 2.0f;
    segment.brp = origin + bottomTang * width / 2.0f;
    segment.tlp = topMidp - topTang * topWidth / 2.0f;
    segment.trp = topMidp + topTang * topWidth / 2.0f;
    // Push segment to array
    branch.segments.push_back(std::move(segment));

    float totalLength = 0.0f;
    totalLength += length;
    if (totalLength > maxLength || topWidth < 2.0f) return std::move(branch);
    
    // Extending the branch
    while (totalLength < maxLength && topWidth > 2.0f) {
        float ratio = totalLength/maxLength;
        
        width = std::move(topWidth);
        topWidth = width * segmentWidthFalloff * randF(0.98f, 1.02f) * exp(-ratio/1.5f);
        
        baseRot = std::move(topRot);
        segmentRot = M_PI_2 + randDistribution(rotationRandomness*(1-ratio));
        topRot = baseRot + segmentRot - M_PI_2 + randDistribution(rotationRandomness*(1-ratio));
        
        bottomTang = std::move(topTang);
        topTang = {cos(topRot), -sin(topRot)};
        
        origin = std::move(topMidp);
        dir = {cos(baseRot + segmentRot), -sin(baseRot + segmentRot)};
        topMidp = origin + dir * length;

        // Assign new points to segment
        segment.blp = origin - bottomTang * width / 2.0f;
        segment.brp = origin + bottomTang * width / 2.0f;
        segment.tlp = topMidp - topTang * topWidth / 2.0f;
        segment.trp = topMidp + topTang * topWidth / 2.0f;
        // Push segment to array
        branch.segments.push_back(std::move(segment));
        
        // Should get longer when totalLength is close to maxLength
        length *= segmentLengthMultiplier * randF(0.92f, 1.08f);
        totalLength += length;
    }
    
    if (depth + rand() % 2 >= recursionLimit) return std::move(branch);
    
    // ADD Branches
    int segmentCount = branch.segments.size();
    int branchCount = totalLength * branchDensity + 1;
    for (int i = 0; i < branchCount; i++) {
        int segmentI = randI(segmentCount/7, segmentCount-1);
        Segment segment = branch.segments[segmentI];
        
        float mainBranchWidth = mag(segment.blp - segment.brp);
        float newWidth = mainBranchWidth * branchWidthFalloff;
        
        Vector2 b, t, to;
        float angle, dirRot;
        
        if (randChoice()) { // Left side
            b = segment.blp;
            t = segment.tlp;
            to = t - b;
            angle = atan(-to.y/to.x);
            if (to.x < 0) { angle += M_PI; }
            dirRot = randF(0.4f, 1.4f);
        } else { // Right side
            b = segment.brp;
            t = segment.trp;
            to = t - b;
            angle = atan(-to.y/to.x);
            if (to.x > 0) { angle += M_PI; }
            dirRot = M_PI - randF(0.2f, 1.1f);
        }
        
        float dist = mag(to);
        newWidth /= sin(dirRot);
        newWidth = min(min(dist,mainBranchWidth), newWidth);
        newWidth *= randF(0.9f, 1.0f);
        float offset = newWidth/2.0f/dist;
        Vector2 point = b + to * randF(offset, 1.0f-offset);
        
        length = mag((segment.blp+segment.brp)*0.5f-(segment.tlp+segment.trp)*0.5f) * segmentLengthMultiplier;
        float maxWidth = maxLength * branchLengthFalloff * (1-segmentI/((float)segmentCount));

        branch.branches.push_back(
            getBranch(depth+1, point, newWidth, angle, dirRot, length, maxWidth)
        );
    }
    
    return std::move(branch);    
}

Tree TreeGenerator::getTree() {
    Tree tree;
    
    tree.root = getBranch(0, {0,500}, segmentStartWidth, 0.0f, M_PI_2, segmentStartLength, maxBranchLength);
    tree.segmentCount = countSegments(tree.root);
    
    return std::move(tree);
}


uint32_t TreeGenerator::countSegments(const Branch& branch) {
    uint32_t bc = 0;
    for (const auto& b : branch.branches) {
        bc += countSegments(b);
    }
    return branch.segments.size() + bc;
}

void TreeGenerator::addTriangles(Mesh& mesh, const Branch& branch, uint32_t& vertex) {
    for (const auto& s : branch.segments) {
        // 0,0
        mesh.vertices[3*vertex] = s.blp.x;
        mesh.vertices[3*vertex+1] = s.blp.y;
        mesh.vertices[3*vertex+2] = 0;
        mesh.normals[3*vertex] = 0;
        mesh.normals[3*vertex+1] = 0;
        mesh.normals[3*vertex+2] = 1;
        mesh.texcoords[2*vertex] = 0;
        mesh.texcoords[2*vertex+1] = 0;
       
        vertex += 1;
        
        // 1,0
        mesh.vertices[3*vertex] = s.brp.x;
        mesh.vertices[3*vertex+1] = s.brp.y;
        mesh.vertices[3*vertex+2] = 0;
        mesh.normals[3*vertex] = 0;
        mesh.normals[3*vertex+1] = 0;
        mesh.normals[3*vertex+2] = 1;
        mesh.texcoords[2*vertex] = 1;
        mesh.texcoords[2*vertex+1] = 0;        

        vertex += 1;
        
        // 1,1
        mesh.vertices[3*vertex] = s.trp.x;
        mesh.vertices[3*vertex+1] = s.trp.y;
        mesh.vertices[3*vertex+2] = 0;
        mesh.normals[3*vertex] = 0;
        mesh.normals[3*vertex+1] = 0;
        mesh.normals[3*vertex+2] = 1;
        mesh.texcoords[2*vertex] = 1;
        mesh.texcoords[2*vertex+1] = 1;        
        
        vertex += 1;

        // 0,0
        mesh.vertices[3*vertex] = s.blp.x;
        mesh.vertices[3*vertex+1] = s.blp.y;
        mesh.vertices[3*vertex+2] = 0;
        mesh.normals[3*vertex] = 0;
        mesh.normals[3*vertex+1] = 0;
        mesh.normals[3*vertex+2] = 1;
        mesh.texcoords[2*vertex] = 0;
        mesh.texcoords[2*vertex+1] = 0;
        
        vertex += 1;
        
        // 1,1
        mesh.vertices[3*vertex] = s.trp.x;
        mesh.vertices[3*vertex+1] = s.trp.y;
        mesh.vertices[3*vertex+2] = 0;
        mesh.normals[3*vertex] = 0;
        mesh.normals[3*vertex+1] = 0;
        mesh.normals[3*vertex+2] = 1;
        mesh.texcoords[2*vertex] = 1;
        mesh.texcoords[2*vertex+1] = 1;        
        
        vertex += 1;
        
        // 0,1
        mesh.vertices[3*vertex] = s.tlp.x;
        mesh.vertices[3*vertex+1] = s.tlp.y;
        mesh.vertices[3*vertex+2] = 0;
        mesh.normals[3*vertex] = 0;
        mesh.normals[3*vertex+1] = 0;
        mesh.normals[3*vertex+2] = 1;
        mesh.texcoords[2*vertex] = 0;
        mesh.texcoords[2*vertex+1] = 1;
        
        vertex += 1;
    }
    
    for (const auto& b : branch.branches) {
        addTriangles(mesh, b, vertex);
    }
}

Mesh TreeGenerator::generateMesh(const Tree& tree) {
    Mesh mesh = { 0 };

    mesh.triangleCount = countSegments(tree.root)*2;
    mesh.vertexCount = mesh.triangleCount*3;
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));
    mesh.normals = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));        
    
    uint32_t vertex = 0;
    addTriangles(mesh, tree.root, vertex);

    UploadMesh(&mesh, false);
    
    return mesh;
}