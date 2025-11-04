#include "EnginePCH.h"
#include "Editor.h"

namespace neu {
void Editor::Begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Editor::UpdateGui(Scene& scene) {
    if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) m_active = !m_active;
    if (!m_active) return;

    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4{ 1, 1, 0, 1 });

    ImGui::Begin("Scene");
    scene.UpdateGui();
    ImGui::Separator();
    int index = 0;
    for (auto& actor : scene.m_actors)
    {
        bool done = false;
        ImGui::PushID(index++);
        if (ImGui::Selectable(actor->name.c_str(), actor.get() == m_selected))
        {
            m_selected = actor.get();
        }
        ImGui::PopID();
    }
    ImGui::End();

    //Inspector
    ImGui::Begin("Inspector");
    if (m_selected) {
        m_selected->UpdateGui();
    }
    ImGui::End();

    ImGui::PopStyleColor();
}

}
