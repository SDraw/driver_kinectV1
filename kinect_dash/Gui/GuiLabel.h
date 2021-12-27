#pragma once
#include "Gui/GuiElement.h"

class GuiLabel final : public GuiElement
{
    std::string m_text;

    GuiLabel(const GuiLabel &that) = delete;
    GuiLabel& operator=(const GuiLabel &that) = delete;

    // GuiElement
    void Update() override;
public:
    explicit GuiLabel(const std::string &p_caption);
    ~GuiLabel();

    void SetText(const std::string &p_text);
    const std::string& GetText() const;
};

