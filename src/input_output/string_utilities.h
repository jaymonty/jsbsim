/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       string_utilities.h
 Author:       Jon S. Berndt
 Date started: 06/01/09

 ------------- Copyright (C) 2009  Jon S. Berndt (jon@jsbsim.org) -------------

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.

HISTORY
--------------------------------------------------------------------------------
06/01/09  JSB  Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <string>
#include <vector>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define ID_STRINGUTILS "$Id: string_utilities.h,v 1.13 2010/07/07 11:59:48 jberndt Exp $"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#if !defined(BASE)
extern std::string& trim_left(std::string& str);
extern std::string& trim_right(std::string& str);
extern std::string& trim(std::string& str);
extern std::string& trim_all_space(std::string& str);
extern std::string& to_upper(std::string& str);
extern std::string& to_lower(std::string& str);
extern bool is_number(const std::string& str);
std::vector <std::string> split(std::string str, char d);
#else
#include <cctype>


std::string& trim_left(std::string& str)
{
    using namespace std;
    while (str.size() && isspace((unsigned char)str[0]))
    {
        str = str.erase(0,1);
    }
    return str;
}

std::string& trim_right(std::string& str)
{
    using namespace std;
    while (str.size() && isspace((unsigned char)str[str.size()-1]))
    {
        str = str.erase(str.size()-1,1);
    }
    return str;
}

std::string& trim(std::string& str)
{
    using namespace std;
    if (str.size() == 0) return str;
    string temp_str = trim_right(str);
    return str = trim_left(temp_str);
}

std::string& trim_all_space(std::string& str)
{

    using namespace std;

    for (size_t i=0; i<str.size(); i++)
    {
        if (isspace((unsigned char)str[i]))
        {
            str = str.erase(i,1);
            --i;
        }
    }
    return str;
}

std::string& to_upper(std::string& str)
{
    using namespace std;
    for (size_t i=0; i<str.size(); i++) str[i] = toupper(str[i]);
    return str;
}

std::string& to_lower(std::string& str)
{
    using namespace std;
    for (size_t i=0; i<str.size(); i++) str[i] = tolower(str[i]);
    return str;
}

bool is_number(const std::string& str)
{
    using namespace std;
    return (str.find_first_not_of("+-.0123456789Ee") == string::npos);
}

std::vector <std::string> split(std::string str, char d)
{
    using namespace std;
    vector <string> str_array;
    size_t index=0;
    string temp = "";

    trim(str);
    index = str.find(d);
    while (index != string::npos)
    {
        temp = str.substr(0,index);
        trim(temp);
        if (temp.size() > 0) str_array.push_back(temp);
        str = str.erase(0,index+1);
        index = str.find(d);
    }
    if (str.size() > 0)
    {
        temp = trim(str);
        if (temp.size() > 0) str_array.push_back(temp);
    }

    return str_array;
}

#endif

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#endif

