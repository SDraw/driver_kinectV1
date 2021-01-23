#pragma once

class CDriverConfig final
{
    static glm::vec3 ms_basePosition;
    static glm::quat ms_baseRotation;
    static unsigned char ms_interpolation;
    static std::vector<size_t> ms_boneIndexes;
public:
    enum FrameInterpolation : unsigned char
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

    static void Load();

    static const glm::vec3& GetBasePosition();
    static const glm::quat& GetBaseRotation();
    static unsigned char GetInterpolation();
    static const std::vector<size_t>& GetBoneIndexes();
};
