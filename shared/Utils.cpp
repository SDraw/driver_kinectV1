#include <string>
#include <vector>
#include <limits>

#include "Utils.h"

size_t ReadEnumVector(const std::string &p_val, const std::vector<std::string> &p_vec)
{
    size_t l_result = std::numeric_limits<size_t>::max();
    for(auto iter = p_vec.begin(), iterEnd = p_vec.end(); iter != iterEnd; ++iter)
    {
        if(!iter->compare(p_val))
        {
            l_result = std::distance(p_vec.begin(), iter);
            break;
        }
    }
    return l_result;
}

size_t ReadEnumVector(const char *p_val, const std::vector<std::string> &p_vec)
{
    size_t l_result = std::numeric_limits<size_t>::max();
    for(auto iter = p_vec.begin(), iterEnd = p_vec.end(); iter != iterEnd; ++iter)
    {
        if(!iter->compare(p_val))
        {
            l_result = std::distance(p_vec.begin(), iter);
            break;
        }
    }
    return l_result;
}
