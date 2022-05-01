#include "tree.hh"

#include <cmath>
#include <cstdlib>
#include "vector.hh"

float randF(float min, float max) { return min + static_cast<float>(rand()/static_cast<float>(RAND_MAX)*(max - min)); }
int randI(int min, int max) { return min + (rand() % (max-min+1)); }
float randRotation() { return rand() % 2 == 0 ? randF(0.4f, 1.2f) : -randF(0.4f, 1.2f); }
float randDistribution(float extrema) { return randF(-extrema, extrema); }
bool randChoice() { return rand()%2 == 0; }

Branch generate_branch(int depth, int max, Vector2 origin, float rot, float length) {
    Branch self = Branch();
    self.origin = origin;
    self.dir = {cos(rot), -sin(rot)};
    self.level = depth;
    self.length = length;

    if (depth + rand() % 2 >= max) return self;

    for (int i = 0; i < self.length * density * pow(1.03f, depth/8.8f) + 1; i++) {
        float b_len = length * len_falloff * randF(0.7f, 1.2f) / fmaxf(depth/3.5f, 1.0f);
        if (b_len < 10.0f) continue;

        Vector2 b_origin = origin + self.dir * length * randF(0.1, 0.9);
        
        self.branches.push_back(generate_branch(depth+1, max, b_origin, rot+randRotation(), b_len));
    }
  
    if (length < 14.0f) return self;
    
    if (rand() % 2 == 0) {
        self.branches.push_back(
            generate_extension_branch(depth+1, max, origin + self.dir * length, rot, length)       
        );
    }
    else {
        vector<Branch> splits = generate_split_branches(depth+1, max, origin + self.dir * length, rot, length);

        for (int i = 0; i < splits.size(); i++) {
            self.branches.push_back(splits[i]);
        }
    }

    return self;
}

vector<Branch> generate_split_branches(int depth, int max, Vector2 origin, float rot, float length) {
    auto branches = vector<Branch>();
    auto rand_angle = randF(0.3f, 0.8f);
    
    branches.push_back(generate_branch(depth, max, origin, rot+rand_angle, length * split_falloff * randF(0.9f, 1.1f)));
    branches.push_back(generate_branch(depth, max, origin, rot-rand_angle, length * split_falloff * randF(0.9f, 1.1f)));
    
    return branches;
}

Branch generate_extension_branch(int depth, int max, Vector2 origin, float rot, float length) {
    float rand_rot = rand() % 2 ? randF(0.2f, 0.6f) : -randF(0.2f, 0.6f);
    return generate_branch(depth, max, origin, rot + rand_rot, length * ext_falloff * randF(0.9f, 1.1f));
}

Branch generate_tree(int max) {
    Branch tree = Branch();
    tree.origin = {0, 500};
    float r = randF(M_PI/2.0f - 0.25f, M_PI/2.0f + 0.25f);
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

Branch TreeGenerator::getBranch(int depth, Vector2 origin, float width, float surfaceAngle, float segmentRot, float length, float maxLength) {
    Branch branch = Branch();
    Segment segment;
    
    float topWidth = width * segmentWidthFalloff * sin(segmentRot);
    
    float baseRot = surfaceAngle;
    float topRot = baseRot + segmentRot - M_PI_2 + randDistribution(rotationRandomness);
    
    Vector2 bottomTang = {cos(baseRot), -sin(baseRot)};
    Vector2 topTang = {cos(topRot), -sin(topRot)};
    
    Vector2 dir = {cos(baseRot + segmentRot), -sin(baseRot + segmentRot)};
    Vector2 topMidp = origin + dir * length;
    
    segment.blp = origin - bottomTang * width / 2.0f;
    segment.brp = origin + bottomTang * width / 2.0f;
    segment.tlp = topMidp - topTang * topWidth / 2.0f;
    segment.trp = topMidp + topTang * topWidth / 2.0f;
        
    branch.segments.push_back(std::move(segment));

    float totalLength = 0.0f;
    totalLength += length;
    
    if (totalLength > maxLength || topWidth < 3.0f) return std::move(branch);
    
    while (totalLength < maxLength && topWidth > 3.0f) {
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
        
        branch.segments.push_back(std::move(segment));
        
        // Should get longer when totalLength is close to maxLength
        length *= segmentLengthMultiplier * randF(0.92f, 1.08f);
        totalLength += length;
    }
    
    if (depth + rand() % 2 >= recursionLimit) return std::move(branch);
    
    // ADD Branches
    int segmentCount = branch.segments.size();
    int branchCount = totalLength * density + 1;
    for (int i = 0; i < branchCount; i++) {
        int segmentI = randI(segmentCount/7, segmentCount-1);
        Segment segment = branch.segments[segmentI];
        
        float width = mag(segment.blp - segment.brp) * branchWidthFalloff;
        
        Vector2 b, t, to;
        float angle, dirRot;
        
        if (randChoice()) { // Left side
            b = segment.blp;
            t = segment.tlp;
            to = t - b;
            angle = atan(-to.y/to.x);
            dirRot = randF(0.2f, 0.9f);
        } else { // Right side
            b = segment.brp;
            t = segment.trp;
            to = t - b;
            angle = atan(-to.y/to.x) - M_PI; // TODO ?? M_PI
            dirRot = M_PI - randF(0.2f, 0.9f);
        }
        
        width /= sin(dirRot);

        float dist = mag(to);
        Vector2 point;

        if (dist > width) {
            float offset = width/2.0f/dist;
            point = b + to * randF(offset, 1.0f-offset);
        } else {
            width = dist;
            point = b + to * 0.5f;
        }

        branch.branches.push_back(
            getBranch(depth+1, point, width, angle, dirRot, dist, maxLength * branchLengthFalloff * (1-segmentI/((float)segmentCount)))
        );
        
    }
    
    return std::move(branch);    
}

Branch TreeGenerator::getTree() {
    return getBranch(0, {0,500}, segmentStartWidth, 0.0f, M_PI_2, segmentStartLength, maxBranchLength);
}

