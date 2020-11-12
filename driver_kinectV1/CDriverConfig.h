#pragma once

class CDriverConfig final
{
    static glm::vec3 ms_basePosition;
    static glm::quat ms_baseRotation;
    static std::vector<size_t> ms_boneIndexes;
public:
    static void Load();

    static const glm::vec3& GetBasePosition();
    static const glm::quat& GetBaseRotation();
    static const std::vector<size_t>& GetBoneIndexes();
};
