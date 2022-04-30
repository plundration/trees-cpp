#include <raylib.h>
#include <vector>
#include <cmath>
#include <raymath.h>
#include <cstdlib>
#include <ctime>

#include "tree.hh"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;

using namespace std;

void DrawBranch(const Branch &branch);

int main() 
{
    // Seed random number generator
    srand(time(0));

    float time = 0.0f;
    Branch tree = generate_tree(8);

    SCREEN_WIDTH = 850;
    SCREEN_HEIGHT = 850;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TreeX");
    SetTargetFPS(60);
    
    Camera2D camera = {
        .offset = {SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f},
        .target = {0,0},
        .rotation = 0.0f,
        .zoom = 1.0f
    };
    
    float timeMultiplier = 1.0f;
    
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime()*timeMultiplier;
        time += deltaTime;

        if (IsKeyPressed(KEY_R)) {
            tree = generate_tree(7);
            time = 0.0f;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode2D(camera);
                DrawBranch(tree);  
            EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void DrawBranch(const Branch &branch) {
    // DrawLineEx(branch.origin, branch.origin + branch.dir * branch.length, 45.0f/pow(branch.level+1.0f,2.5f), RAYWHITE);
    DrawLineEx(branch.origin, Vector2Add(branch.origin, Vector2Scale(branch.dir, branch.length)), 45.0f/pow(branch.level+1.0f,1.2f), RAYWHITE);
    for (auto& b : branch.branches) {
        DrawBranch(b);
    }
}
