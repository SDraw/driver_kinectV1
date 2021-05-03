#pragma once
#include "Gui/GuiElement.h"

class GuiInputText final : public GuiElement
{
    std::string m_text;
    bool m_sidesSwitched;

    GuiInputText(const GuiInputText &that) = delete;
    GuiInputText& operator=(const GuiInputText &that) = delete;

    // GuiElement
    void Update() override;
public:
    explicit GuiInputText(const std::string &f_name);
    ~GuiInputText();

    void SetText(const std::string &f_text);
    const std::string& GetText() const;

    void SetSidesSwitched(bool f_state);
    bool GetSidesSwitched() const;
};

