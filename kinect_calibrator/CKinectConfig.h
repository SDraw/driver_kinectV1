#pragma once

class CKinectConfig final
{
    struct TrackerData
    {
        size_t m_boneIndex;
        bool m_enabled;

        TrackerData(size_t f_index, bool f_enabled)
        {
            m_boneIndex = f_index;
            m_enabled = f_enabled;
        }
    };

    std::string m_path;

    glm::vec3 m_basePosition;
    glm::quat m_baseRotation;
    std::vector<TrackerData> m_trackersData; // Storage only

    CKinectConfig(const CKinectConfig &that) = delete;
    CKinectConfig& operator=(const CKinectConfig &that) = delete;

    static bool CompareTrackerData_Sort(const TrackerData &f_trackerA, const TrackerData &f_trackerB);
    static bool CompareTrackerData_Unique(const TrackerData &f_trackerA, const TrackerData &f_trackerB);
public:
    explicit CKinectConfig(const char *f_path);
    ~CKinectConfig();

    void Load();
    void Save();

    void SetBasePosition(const glm::vec3 &f_pos);
    const glm::vec3& GetBasePosition() const;

    void SetBaseRotation(const glm::quat &f_rot);
    const glm::quat& GetBaseRotation() const;
};
