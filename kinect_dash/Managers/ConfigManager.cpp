#include "stdafx.h"

#include "Managers/ConfigManager.h"
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

const std::vector<std::string> g_boneNames
{
    "HipCenter", "Spine", "ShoulderCenter", "Head",
    "ShoulderLeft", "ElbowLeft", "WristLeft", "HandLeft",
    "ShoulderRight", "ElbowRight", "WristRight", "HandRight",
    "HipLeft", "KneeLeft", "AnkleLeft", "FootLeft",
    "HipRight", "KneeRight", "AnkleRight", "FootRight"
};
extern const size_t g_bonesCount = g_boneNames.size();

const char* g_settingsPath = "..\\..\\resources\\settings.xml";

ConfigManager::ConfigManager()
{
    m_basePosition = glm::vec3(0.f);
    m_baseRotation = glm::quat(1.f, 0.f, 0.f, 0.f);
    m_interpolationType = FrameInterpolation::FI_Linear;
    m_trackersStates.assign(g_bonesCount, false);
    m_trackingState = true;
}
ConfigManager::~ConfigManager()
{
}

void ConfigManager::Load()
{
    pugi::xml_document *l_document = new pugi::xml_document();
    if(l_document->load_file(g_settingsPath))
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
                            m_interpolationType = ReadEnumVector(l_attribValue.as_string(), g_interpolationTypes);
                            if(m_interpolationType == std::numeric_limits<size_t>::max()) m_interpolationType = FrameInterpolation::FI_Linear;
                        } break;
                        case SettingIndex::SI_Trackers:
                        {
                            m_trackingState = l_attribValue.as_bool(true);

                            for(pugi::xml_node l_trackerNode = l_node.child("tracker"); l_trackerNode; l_trackerNode = l_trackerNode.next_sibling("tracker"))
                            {
                                const pugi::xml_attribute l_attribBone = l_trackerNode.attribute("name");
                                const pugi::xml_attribute l_attribEnabled = l_trackerNode.attribute("value");
                                if(l_attribBone && l_attribEnabled)
                                {
                                    size_t l_boneIndex = ReadEnumVector(l_attribBone.as_string(), g_boneNames);
                                    if(l_boneIndex != std::numeric_limits<size_t>::max())
                                    {
                                        m_trackersStates[l_boneIndex] = l_attribEnabled.as_bool(false);
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
}

void ConfigManager::Save()
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
                            l_valueAttrib.set_value(g_interpolationTypes[m_interpolationType].c_str());
                            break;
                        case SettingIndex::SI_Trackers:
                        {
                            l_valueAttrib.set_value(m_trackingState);
                            for(size_t j = 0U; j < g_bonesCount; j++)
                            {
                                pugi::xml_node l_trackerNode = l_settingNode.append_child("tracker");
                                if(l_trackerNode)
                                {
                                    pugi::xml_attribute l_boneAttribute = l_trackerNode.append_attribute("name");
                                    pugi::xml_attribute l_enabledAttribute = l_trackerNode.append_attribute("value");
                                    if(l_boneAttribute && l_enabledAttribute)
                                    {
                                        l_boneAttribute.set_value(g_boneNames[j].c_str());
                                        l_enabledAttribute.set_value(m_trackersStates[j]);
                                    }
                                }
                            }
                        } break;
                    }
                }
            }
        }
    }
    l_document->save_file(g_settingsPath);
    delete l_document;
}

void ConfigManager::ChangeTrackerState(size_t p_index)
{
    if(p_index < g_bonesCount) m_trackersStates[p_index] = !m_trackersStates[p_index];
}
bool ConfigManager::GetTrackerState(size_t p_index) const
{
    bool l_result = false;
    if(p_index < g_bonesCount) l_result = m_trackersStates[p_index];
    return l_result;
}

void ConfigManager::ChangeTrackingState()
{
    m_trackingState = !m_trackingState;
}
bool ConfigManager::GetTrackingState() const
{
    return m_trackingState;
}

void ConfigManager::SetInterpolationType(size_t p_type)
{
    m_interpolationType = glm::clamp<size_t>(p_type, FrameInterpolation::FI_Linear, FrameInterpolation::FI_Circular);
}
size_t ConfigManager::GetInterpolationType() const
{
    return m_interpolationType;
}

void ConfigManager::SetBasePosition(const glm::vec3 &p_pos)
{
    std::memcpy(&m_basePosition, &p_pos, sizeof(glm::vec3));
}
const glm::vec3& ConfigManager::GetBasePosition() const
{
    return m_basePosition;
}

void ConfigManager::SetBaseRotation(const glm::quat &p_rot)
{
    std::memcpy(&m_baseRotation, &p_rot, sizeof(glm::quat));
}
const glm::quat& ConfigManager::GetBaseRotation() const
{
    return m_baseRotation;
}
