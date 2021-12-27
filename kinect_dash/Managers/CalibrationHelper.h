#pragma once
class VRManager;

class CalibrationHelper final
{
    friend VRManager;
    VRManager *m_vrManager;

    enum CircleQuadrant : unsigned char
    {
        CQ_Right = 0U,
        CQ_Up,
        CQ_Left,
        CQ_Down,

        CQ_None = 255U
    };
    struct InputInfo
    {
        unsigned char m_quadrant;
        bool m_trigger;
    };

    InputInfo m_inputInfoLeft;
    InputInfo m_inputInfoRight;

    glm::vec3 m_basePosition;
    glm::quat m_baseRotation;

    bool m_active;

    CalibrationHelper(const CalibrationHelper &that) = delete;
    CalibrationHelper& operator=(const CalibrationHelper &that) = delete;
protected:
    explicit CalibrationHelper(VRManager *p_vrManager);
    ~CalibrationHelper();

    void StartCalibration();
    void StopCalibration();
    bool IsCalibrationActive() const;

    void RecieveControllerAxis(bool p_left, const glm::vec2 &p_axis);
    void RecieveControllerTrigger(bool p_left, bool p_state);
    void ReciveControllerGrip(bool p_left, bool p_state);
    void RecieveControllerDisconnect(bool p_left);

    void DoPulse();
};

