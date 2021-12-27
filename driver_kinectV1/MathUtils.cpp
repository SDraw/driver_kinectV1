#include "stdafx.h"

#include "MathUtils.h"

const float g_pi = glm::pi<float>();

float QuadraticEaseInOut(float p_value)
{
    float l_result = 0.f;
    if(p_value < 0.5f) l_result = 2.f*glm::pow(p_value, 2.f);
    else l_result = (-2.f * glm::pow(p_value, 2.f)) + (4.f * p_value) - 1.f;
    return l_result;
}

float CubicEaseInOut(float p_value)
{
    float l_result = 0.f;
    if(p_value < 0.5f) l_result = 4.f * glm::pow(p_value, 3.f);
    else
    {
        float l_step = ((2.f * p_value) - 2.f);
        l_result = 0.5f * glm::pow(l_step, 3.f) + 1.f;
    }
    return l_result;
}

float QuarticEaseInOut(float p_value)
{
    float l_result = 0.f;
    if(p_value < 0.5f) l_result = 8.f * glm::pow(p_value, 4.f);
    else
    {
        float l_step = p_value - 1.f;
        l_result = -8.f * glm::pow(l_step, 4.f) + 1.f;
    }
    return l_result;
}

float QuinticEaseInOut(float p_value)
{
    float l_result = 0.f;

    if(p_value < 0.5f) l_result = 16 * glm::pow(p_value, 5.f);
    else
    {
        float l_step = ((2.f * p_value) - 2.f);
        l_result = 0.5f * glm::pow(l_step, 5.f) + 1.f;
    }
    return l_result;
}

float ExponentialEaseInOut(float p_value)
{
    float l_result = p_value;
    if(p_value > 0.f && p_value < 1.f)
    {
        if(p_value < 0.5) l_result = 0.5f * glm::pow(2.f, (20.f * p_value) - 10.f);
        else l_result = -0.5f * glm::pow(2.f, (-20.f * p_value) + 10.f) + 1.f;
    }
    return l_result;
}

float SineEaseInOut(float p_value)
{
    return -0.5f*(cos(g_pi*p_value) - 1.f);
}

float CircularEaseInOut(float p_value)
{
    float l_result = 0.f;
    if(p_value < 0.5f) l_result = 0.5f * (1.f - glm::sqrt(1.f - 4.f * glm::pow(p_value, 2.f)));
    else l_result = 0.5f * (glm::sqrt(-((2.f * p_value) - 3.f) * ((2.f * p_value) - 1.f)) + 1.f);
    return l_result;
}
