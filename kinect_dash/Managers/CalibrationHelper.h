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
    explicit CalibrationHelper(VRManager *f_vrManager);
    ~CalibrationHelper();

    void StartCalibration();
    void StopCalibration();
    bool IsCalibrationActive() const;

    void RecieveControllerAxis(bool f_left, const glm::vec2 &f_axis);
    void RecieveControllerTrigger(bool f_left, bool f_state);
    void ReciveControllerGrip(bool f_left, bool f_state);
    void RecieveControllerDisconnect(bool f_left);

    void DoPulse();
};

