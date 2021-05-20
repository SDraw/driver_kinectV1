#include "stdafx.h"

#include "CDriverConfig.h"

#include "Utils.h"

extern char g_modulePath[];

const std::vector<std::string> g_settingNames
{
    "basePosition", "baseRotation", "interpolation", "trackers"
};

enum SettingIndex : size_t
{
    SI_BasePosition = 0U,
    SI_BaseRotation,
    SI_Interpolation,
    SI_Trackers
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

glm::vec3 CDriverConfig::ms_basePosition(0.f);
glm::quat CDriverConfig::ms_baseRotation(1.f, 0.f, 0.f, 0.f);
unsigned char CDriverConfig::ms_interpolation = CDriverConfig::FI_Linear;
bool CDriverConfig::ms_tracking = true;
std::vector<size_t> CDriverConfig::ms_boneIndexes;

void CDriverConfig::Load()
{
    std::string l_path(g_modulePath);
    l_path.erase(l_path.begin() + l_path.rfind('\\'), l_path.end());
    l_path.append("\\..\\..\\resources\\settings.xml");

    pugi::xml_document *l_settings = new pugi::xml_document();
    if(l_settings->load_file(l_path.c_str()))
    {
        const pugi::xml_node l_root = l_settings->child("settings");
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
                            l_stream >> ms_basePosition.x >> ms_basePosition.y >> ms_basePosition.z;
                        } break;

                        case SettingIndex::SI_BaseRotation:
                        {
                            std::stringstream l_stream(l_attribValue.as_string("0.0 0.0 0.0 1.0"));
                            l_stream >> ms_baseRotation.x >> ms_baseRotation.y >> ms_baseRotation.z >> ms_baseRotation.w;
                        } break;

                        case SettingIndex::SI_Interpolation:
                        {
                            size_t l_interpolation = ReadEnumVector(l_attribValue.as_string("linear"), g_interpolationTypes);
                            if(l_interpolation != std::numeric_limits<size_t>::max()) ms_interpolation = static_cast<unsigned char>(l_interpolation);
                        } break;

                        case SettingIndex::SI_Trackers:
                        {
                            ms_tracking = l_attribValue.as_bool(true);
                            for(pugi::xml_node l_trackerNode = l_node.child("tracker"); l_trackerNode; l_trackerNode = l_trackerNode.next_sibling("tracker"))
                            {
                                const pugi::xml_attribute l_attribBone = l_trackerNode.attribute("name");
                                const pugi::xml_attribute l_attribEnabled = l_trackerNode.attribute("value");
                                if(l_attribBone && l_attribEnabled)
                                {
                                    size_t l_boneIndex = ReadEnumVector(l_attribBone.as_string(), g_boneNames);
                                    if(l_boneIndex != std::numeric_limits<size_t>::max())
                                    {
                                        if(l_attribEnabled.as_bool(false)) ms_boneIndexes.push_back(l_boneIndex);
                                    }
                                }
                            }
                        } break;
                    }
                }
            }
        }
    }

    delete l_settings;

    // Remove duplicated bones
    std::sort(ms_boneIndexes.begin(), ms_boneIndexes.end());
    ms_boneIndexes.erase(std::unique(ms_boneIndexes.begin(), ms_boneIndexes.end()), ms_boneIndexes.end());
}

const glm::vec3& CDriverConfig::GetBasePosition()
{
    return ms_basePosition;
}

const glm::quat& CDriverConfig::GetBaseRotation()
{
    return ms_baseRotation;
}

unsigned char CDriverConfig::GetInterpolation()
{
    return ms_interpolation;
}

bool CDriverConfig::GetTrackingState()
{
    return ms_tracking;
}

const std::vector<size_t>& CDriverConfig::GetBoneIndexes()
{
    return ms_boneIndexes;
}
