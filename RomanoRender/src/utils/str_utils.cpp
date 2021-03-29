#include "str_utils.h"


void remove_spaces(char* s)
{
    char* cpy = s; 
    char* temp = s;

    while (*cpy)
    {
        if (*cpy != ' ')
            *temp++ = *cpy;
        cpy++;
    }
    *temp = 0;
}


std::string increment_name(std::string& name)
{
    size_t last_index = name.find_last_not_of("0123456789");

    if (last_index + 1 == name.length()) name.append("1");
    else
    {
        std::string count = name.substr(last_index + 1);
        int val = std::stoi(count);
        val++;
        int l = name.length() - last_index;
        name = name.replace(last_index + 1, l, std::to_string(val));
    }

    return name;
}