#include "stdafx.h"

#include "Gui/GuiWindow.h"

GuiWindow::GuiWindow(const std::string &f_title) : GuiElement(f_title)
{
}
GuiWindow::~GuiWindow()
{
}

void GuiWindow::Update()
{
    GuiElement::Update();

    ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowPos(m_imguiPosition);
    ImGui::SetWindowSize(m_imguiSize);

    GuiElement::UpdateChildren();
    ImGui::End();
}
