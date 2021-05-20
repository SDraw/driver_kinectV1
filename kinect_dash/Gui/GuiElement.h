#pragma once

class GuiElement
{
    GuiElement(const GuiElement &that) = delete;
    GuiElement& operator=(const GuiElement &that) = delete;

    bool m_sameLine;
public:
    bool Add(GuiElement *f_child);
    void Remove(GuiElement *f_child);
    bool HasChild(const GuiElement *f_child);

    void SetPosition(const sf::Vector2f &f_pos);
    const sf::Vector2f& GetPosition() const;

    void SetSize(const sf::Vector2f &f_size);
    const sf::Vector2f& GetSize() const;

    // Not all elements are affected by color
    void SetColor(const sf::Color &f_col);
    const sf::Color& GetColor() const;

    void SetOnSameLine(bool f_state);
    bool GetOnSameLine() const;
protected:
    std::string m_name;
    sf::Vector2f m_position;
    ImVec2 m_imguiPosition;
    sf::Vector2f m_size;
    ImVec2 m_imguiSize;
    sf::Color m_color;
    ImVec4 m_imguiColor;

    std::vector<GuiElement*> m_children;
    GuiElement *m_parent;

    explicit GuiElement(const std::string &f_name);
    ~GuiElement();

    virtual void Update();
    void UpdateChildren();
};
