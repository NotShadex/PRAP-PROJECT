#include "DebugManager.h"

void Debug::Render(Player player)
{
    if (!showWindow) return;
    ImGuiWindowFlags windowFlags = 0;
    ImGui::Begin("Debug Window");
    ImGui::Text("Debug Mode Active");
    ImGui::Text("Velocity X: %f", player.velocity.x);
    ImGui::Text("Velocity Y: %f", player.velocity.y);
    ImGui::Text("Direction: %d", player.dir);
    ImGui::Text("Dash Timer: %d", player.dashTimer);
    ImGui::Text("Is Running: %d", player.isRunning);
    ImGui::Text("On Floor: %d", player.isOnFloor);
    
    ImGui::End();
}
