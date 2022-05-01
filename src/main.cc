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

    TreeGenerator gen{};
    Tree tree = gen.getTree();
    Model model = LoadModelFromMesh(TreeGenerator::generateMesh(tree));
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("t.png");
    
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
            tree = gen.getTree();
            model.meshes[0] = TreeGenerator::generateMesh(tree);
            regenerate = false;
        }
        
        BeginDrawing();
            ClearBackground({93,155,168,255});
            BeginMode2D(camera);
                DrawRectangleRounded({-400,0,800,100}, 0.2f, 8, {59,112,63,255});
                DrawModel(model, {0,0,0}, 1.0f, RAYWHITE);
                //DrawModelWires(model, {0,0,0}, 1.0f, RAYWHITE);
            EndMode2D();
        
            if (GuiButton({10,10,100,40}, "GENERATE")) { regenerate = true; }

            gen.branchDensity = GuiSlider( { 50, 60, 130, 20 }, "DENSITY", TextFormat("%.4f", gen.branchDensity), gen.branchDensity, 0.1f, 0.8f );
            gen.maxBranchLength = GuiSlider( { 40, 90, 130, 20 }, "MAX L", TextFormat("%4.f", gen.maxBranchLength), gen.maxBranchLength, 400.0f, 1500.0f );
            gen.branchLengthFalloff = GuiSlider( { 40, 120, 130, 20 }, "BWF", TextFormat("%.4f", gen.branchLengthFalloff), gen.branchLengthFalloff, 0.40f, 0.95f );
            gen.branchWidthFalloff = GuiSlider( { 40, 150, 130, 20 }, "BLF", TextFormat("%.4f", gen.branchWidthFalloff), gen.branchWidthFalloff, 0.30f, 0.90f );
            gen.segmentStartLength = GuiSlider( { 40, 180, 130, 20 }, "SSL", TextFormat("%2.1f", gen.segmentStartLength), gen.segmentStartLength, 20.0f, 90.0f );
            gen.segmentStartWidth = GuiSlider( { 40, 210, 130, 20 }, "SSW", TextFormat("%2.1f", gen.segmentStartWidth), gen.segmentStartWidth, 20.0f, 90.0f );
            gen.segmentWidthFalloff = GuiSlider( { 40, 240, 130, 20 }, "SWF", TextFormat("%.4f", gen.segmentWidthFalloff), gen.segmentWidthFalloff, 0.85f, 1.0f );
            gen.segmentLengthMultiplier = GuiSlider( { 40, 270, 130, 20 }, "SLM", TextFormat("%.4f", gen.segmentLengthMultiplier), gen.segmentLengthMultiplier, 0.99f, 1.03f );
            gen.rotationRandomness = GuiSlider( { 40, 300, 130, 20 }, "ROT", TextFormat("%.3f", gen.rotationRandomness), gen.rotationRandomness, 0.05f, 0.45f );

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