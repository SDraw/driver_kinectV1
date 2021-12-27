#pragma once

class ConfigManager final
{
    friend class Core;

    glm::vec3 m_basePosition;
    glm::quat m_baseRotation;
    size_t m_interpolationType;
    bool m_trackingState;
    std::vector<bool> m_trackersStates;

    ConfigManager(const ConfigManager &that) = delete;
    ConfigManager& operator=(const ConfigManager &that) = delete;
public:
    enum FrameInterpolation : size_t
    {
        FI_Linear = 0U,
        FI_Quadratic,
        FI_Cubic,
        FI_Quartic,
        FI_Quintic,
        FI_Exponential,
        FI_Sine,
        FI_Circular
    };

    void ChangeTrackerState(size_t p_index);
    bool GetTrackerState(size_t p_index) const;

    void ChangeTrackingState();
    bool GetTrackingState() const;

    void SetInterpolationType(size_t p_type);
    size_t GetInterpolationType() const;

    void SetBasePosition(const glm::vec3 &p_pos);
    const glm::vec3& GetBasePosition() const;

    void SetBaseRotation(const glm::quat &p_rot);
    const glm::quat& GetBaseRotation() const;
protected:
    ConfigManager();
    ~ConfigManager();

    void Load();
    void Save();
};

