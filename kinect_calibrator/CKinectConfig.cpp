#include "stdafx.h"

#include "CKinectConfig.h"

#include "Utils.h"

const std::vector<std::string> g_settingNames
{
    "basePosition", "baseRotation", "interpolation", "trackers"
};
enum SettingIndex : size_t
{
    SI_BasePosition = 0U,
    SI_BaseRotation,
    SI_Interpolation,
    SI_Trackers,

    SI_Count
};

const std::vector<std::string> g_interpolationTypes
{
    "linear", "quadratic", "cubic", "quartic", "quintic", "exponential", "sine", "circular"
};
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

const std::vector<std::string> g_boneNames
{
    "HipCenter", "Spine", "ShoulderCenter", "Head",
    "ShoulderLeft", "ElbowLeft", "WristLeft", "HandLeft",
    "ShoulderRight", "ElbowRight", "WristRight", "HandRight",
    "HipLeft", "KneeLeft", "AnkleLeft", "FootLeft",
    "HipRight", "KneeRight", "AnkleRight", "FootRight"
};

CKinectConfig::CKinectConfig(const char *f_path)
{
    char l_buffer[MAX_PATH];
    GetModuleFileNameA(NULL, l_buffer, MAX_PATH);
    m_path.assign(l_buffer);
    m_path.assign(m_path.substr(0U, m_path.find_last_of("\\/")));
    m_path.append(f_path);

    m_basePosition = glm::vec3(0.f);
    m_baseRotation = glm::quat(1.f, 0.f, 0.f, 0.f);
    m_interpolation = FrameInterpolation::FI_Linear;
}

CKinectConfig::~CKinectConfig()
{
}

void CKinectConfig::Load()
{
    pugi::xml_document *l_document = new pugi::xml_document();
    if(l_document->load_file(m_path.c_str()))
    {
        const pugi::xml_node l_root = l_document->child("settings");
        if(l_root)
        {
            for(pugi::xml_node l_node = l_root.child("setting"); l_node; l_node = l_node.next_sibling("setting"))
            {
                const pugi::xml_attribute l_attribName = l_node.attribute("name");
                const pugi::xml_attribute l_attribValue = l_node.attribute("value");
                if(l_attribName && l_attribValue)
                {
                    switch(ReadEnumVector(l_attribName.as_string(), g_settingNames))
                    {
                        case SettingIndex::SI_BasePosition:
                        {
                            std::stringstream l_stream(l_attribValue.as_string("0.0 0.0 0.0"));
                            l_stream >> m_basePosition.x >> m_basePosition.y >> m_basePosition.z;
                        } break;
                        case SettingIndex::SI_BaseRotation:
                        {
                            std::stringstream l_stream(l_attribValue.as_string("0.0 0.0 0.0 1.0"));
                            l_stream >> m_baseRotation.x >> m_baseRotation.y >> m_baseRotation.z >> m_baseRotation.w;
                        } break;
                        case SettingIndex::SI_Interpolation:
                        {
                            size_t l_interpolation = ReadEnumVector(l_attribValue.as_string(), g_interpolationTypes);
                            if(l_interpolation != std::numeric_limits<size_t>::max()) m_interpolation = static_cast<unsigned char>(l_interpolation);
                        } break;
                        case SettingIndex::SI_Trackers:
                        {
                            for(pugi::xml_node l_trackerNode = l_node.child("tracker"); l_trackerNode; l_trackerNode = l_trackerNode.next_sibling("tracker"))
                            {
                                const pugi::xml_attribute l_attribBone = l_trackerNode.attribute("name");
                                const pugi::xml_attribute l_attribEnabled = l_trackerNode.attribute("value");
                                if(l_attribBone && l_attribEnabled)
                                {
                                    size_t l_boneIndex = ReadEnumVector(l_attribBone.as_string(), g_boneNames);
                                    if(l_boneIndex != std::numeric_limits<size_t>::max())
                                    {
                                        m_trackersData.emplace_back(l_boneIndex, l_attribEnabled.as_bool(false));
                                    }
                                }
                            }
                        } break;
                    }
                }
            }
        }
    }
    delete l_document;

    // Remove duplicated bones
    std::sort(m_trackersData.begin(), m_trackersData.end(),CompareTrackerData_Sort);
    m_trackersData.erase(std::unique(m_trackersData.begin(), m_trackersData.end(),CompareTrackerData_Unique), m_trackersData.end());

    // Add default tracked bones if nothing is parsed
    if(m_trackersData.empty())
    {
        for(size_t i = 0U, j = g_boneNames.size(); i < j; i++) m_trackersData.emplace_back(i, false);
        m_trackersData[0U].m_enabled = true; // SpineBase
        m_trackersData[14U].m_enabled = true; // AnkleLeft
        m_trackersData[18U].m_enabled = true; // AnkleRight
    }
}

void CKinectConfig::Save()
{
    pugi::xml_document *l_document = new pugi::xml_document();
    pugi::xml_node l_root = l_document->append_child("settings");
    if(l_root)
    {
        for(size_t i = 0U; i < SI_Count; i++)
        {
            pugi::xml_node l_settingNode = l_root.append_child("setting");
            if(l_settingNode)
            {
                pugi::xml_attribute l_nameAttrib = l_settingNode.append_attribute("name");
                pugi::xml_attribute l_valueAttrib = l_settingNode.append_attribute("value");
                if(l_nameAttrib && l_valueAttrib)
                {
                    l_nameAttrib.set_value(g_settingNames[i].c_str());
                    switch(i)
                    {
                        case SettingIndex::SI_BasePosition:
                        {
                            std::string l_pos;
                            for(int j = 0; j < 3; j++)
                            {
                                l_pos.append(std::to_string(m_basePosition[j]));
                                if(j < 2) l_pos.push_back(' ');
                            }
                            l_valueAttrib.set_value(l_pos.c_str());
                        } break;
                        case SettingIndex::SI_BaseRotation:
                        {
                            std::string l_rot;
                            for(int j = 0; j < 4; j++)
                            {
                                l_rot.append(std::to_string(m_baseRotation[j]));
                                if(j < 3) l_rot.push_back(' ');
                            }
                            l_valueAttrib.set_value(l_rot.c_str());
                        } break;
                        case SettingIndex::SI_Interpolation:
                            l_valueAttrib.set_value(g_interpolationTypes[m_interpolation].c_str());
                            break;
                        case SettingIndex::SI_Trackers:
                        {
                            l_valueAttrib.set_value("");
                            for(auto l_data : m_trackersData)
                            {
                                pugi::xml_node l_trackerNode = l_settingNode.append_child("tracker");
                                if(l_trackerNode)
                                {
                                    pugi::xml_attribute l_boneAttribute = l_trackerNode.append_attribute("name");
                                    pugi::xml_attribute l_enabledAttribute = l_trackerNode.append_attribute("value");
                                    if(l_boneAttribute && l_enabledAttribute)
                                    {
                                        l_boneAttribute.set_value(g_boneNames[l_data.m_boneIndex].c_str());
                                        l_enabledAttribute.set_value(l_data.m_enabled);
                                    }
                                }
                            }
                        } break;
                    }
                }
            }
        }
    }
    l_document->save_file(m_path.c_str());
    delete l_document;
}

void CKinectConfig::SetBasePosition(const glm::vec3 &f_pos)
{
    std::memcpy(&m_basePosition, &f_pos, sizeof(glm::vec3));
}

const glm::vec3& CKinectConfig::GetBasePosition() const
{
    return m_basePosition;
}

void CKinectConfig::SetBaseRotation(const glm::quat &f_rot)
{
    std::memcpy(&m_baseRotation, &f_rot, sizeof(glm::quat));
}

const glm::quat& CKinectConfig::GetBaseRotation() const
{
    return m_baseRotation;
}

bool CKinectConfig::CompareTrackerData_Sort(const TrackerData &f_trackerA, const TrackerData &f_trackerB)
{
    return (f_trackerA.m_boneIndex < f_trackerB.m_boneIndex);
}

bool CKinectConfig::CompareTrackerData_Unique(const TrackerData &f_trackerA, const TrackerData &f_trackerB)
{
    return (f_trackerA.m_boneIndex == f_trackerB.m_boneIndex);
}
