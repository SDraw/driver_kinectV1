#include "stdafx.h"

#include "MathUtils.h"

const float g_pi = glm::pi<float>();

float QuadraticEaseInOut(float f_value)
{
    float l_result = 0.f;
    if(f_value < 0.5f) l_result = 2.f*glm::pow(f_value, 2.f);
    else l_result = (-2.f * glm::pow(f_value, 2.f)) + (4.f * f_value) - 1.f;
    return l_result;
}

float CubicEaseInOut(float f_value)
{
    float l_result = 0.f;
    if(f_value < 0.5f) l_result = 4.f * glm::pow(f_value, 3.f);
    else
    {
        float l_step = ((2.f * f_value) - 2.f);
        l_result = 0.5f * glm::pow(l_step, 3.f) + 1.f;
    }
    return l_result;
}

float QuarticEaseInOut(float f_value)
{
    float l_result = 0.f;
    if(f_value < 0.5f) l_result = 8.f * glm::pow(f_value, 4.f);
    else
    {
        float l_step = f_value - 1.f;
        l_result = -8.f * glm::pow(l_step, 4.f) + 1.f;
    }
    return l_result;
}

float QuinticEaseInOut(float f_value)
{
    float l_result = 0.f;

    if(f_value < 0.5f) l_result = 16 * glm::pow(f_value, 5.f);
    else
    {
        float l_step = ((2.f * f_value) - 2.f);
        l_result = 0.5f * glm::pow(l_step, 5.f) + 1.f;
    }
    return l_result;
}

float ExponentialEaseInOut(float f_value)
{
    float l_result = f_value;
    if(f_value > 0.f && f_value < 1.f)
    {
        if(f_value < 0.5) l_result = 0.5f * glm::pow(2.f, (20.f * f_value) - 10.f);
        else l_result = -0.5f * glm::pow(2.f, (-20.f * f_value) + 10.f) + 1.f;
    }
    return l_result;
}

float SineEaseInOut(float f_value)
{
    return -0.5f*(cos(g_pi*f_value) - 1.f);
}

float CircularEaseInOut(float f_value)
{
    float l_result = 0.f;
    if(f_value < 0.5f) l_result = 0.5f * (1.f - glm::sqrt(1.f - 4.f * glm::pow(f_value, 2.f)));
    else l_result = 0.5f * (glm::sqrt(-((2.f * f_value) - 3.f) * ((2.f * f_value) - 1.f)) + 1.f);
    return l_result;
}
