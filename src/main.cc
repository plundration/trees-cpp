#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include <vector>
#include <cmath>
#include <raymath.h>
#include <cstdlib>
#include <ctime>

#include "tree.hh"
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

const float CAMERA_SPEED = 2.0f;

using namespace std;

void DrawBranch(const Branch &branch);

int main() 
{
    // Seed random number generator
    srand(time(0));

    SCREEN_WIDTH = 850;
    SCREEN_HEIGHT = 850;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TreeX");
    SetTargetFPS(60);

    Camera2D camera = { .offset = {SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f}, .target = {0,0}, .rotation = 0.0f, .zoom = 1.0f };

    TreeGenerator treeGenerator{};
    Tree tree = treeGenerator.getTree();
    Model model = LoadModelFromMesh(TreeGenerator::generateMesh(tree));
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("texture.png");
    
    bool regenerate = false;    
    
    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_W)) camera.target.y -= CAMERA_SPEED/camera.zoom;
        if (IsKeyDown(KEY_S)) camera.target.y += CAMERA_SPEED/camera.zoom;
        if (IsKeyDown(KEY_D)) camera.target.x += CAMERA_SPEED/camera.zoom;
        if (IsKeyDown(KEY_A)) camera.target.x -= CAMERA_SPEED/camera.zoom;
        camera.zoom *= 1 + GetMouseWheelMove()/30.0f;

        if (IsKeyPressed(KEY_R) || regenerate) {
            UnloadMesh(model.meshes[0]);
            tree = treeGenerator.getTree();
            model.meshes[0] = TreeGenerator::generateMesh(tree);
            regenerate = false;
        }
        
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                DrawModel(model, {0,0,0}, 1.0f, RAYWHITE);
                //DrawModelWires(model, {0,0,0}, 1.0f, RAYWHITE);
            EndMode2D();
        
            if (GuiButton({10,10,100,40}, "GENERATE")) { regenerate = true; }

            treeGenerator.branchDensity = GuiSlider(
                { 10, 60, 200, 20 }, "TEST", TextFormat("%.4f", treeGenerator.branchDensity), treeGenerator.branchDensity, 0.0f, 0.009f
            );

        EndDrawing();
    }
    
    UnloadModel(model);

    CloseWindow();
    return 0;
}


void DrawBranch(const Branch &branch) {
    for (auto& s : branch.segments) {
        DrawTriangleLines(s.blp, s.brp, s.trp, RAYWHITE);
        DrawTriangleLines(s.blp, s.trp, s.tlp, RAYWHITE);
    }
    for (auto& b : branch.branches) {
        DrawBranch(b);
    }
}