#pragma once

class GuiElement
{
    GuiElement(const GuiElement &that) = delete;
    GuiElement& operator=(const GuiElement &that) = delete;

    bool m_sameLine;
public:
    bool Add(GuiElement *p_child);
    void Remove(GuiElement *p_child);
    bool HasChild(const GuiElement *p_child);

    void SetPosition(const sf::Vector2f &p_pos);
    const sf::Vector2f& GetPosition() const;

    void SetSize(const sf::Vector2f &p_size);
    const sf::Vector2f& GetSize() const;

    // Not all elements are affected by color
    void SetColor(const sf::Color &p_col);
    const sf::Color& GetColor() const;

    void SetOnSameLine(bool p_state);
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

    explicit GuiElement(const std::string &p_name);
    ~GuiElement();

    virtual void Update();
    void UpdateChildren();
};
