#include "DebugManager.h"

void Debug::Render(Player player)
{
    if (!showWindow) return;
    ImGuiWindowFlags windowFlags = 0;
    ImGui::Begin("Debug Window");
    
    ImGui::Text("Debug Mode Active");
    ImGui::Text("Velocity X: %f", player.velocity.x);
    ImGui::Text("Velocity Y: %f", player.velocity.y);
    ImGui::Text("Position X: %f", player.position.x);
    ImGui::Text("Position Y: %f", player.position.y);
    ImGui::End();
}
