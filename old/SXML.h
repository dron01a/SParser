#ifndef __XML__PARCE__
#define __XML__PARCE__ 

#include "FSTool.h"
#include "WPTool.h"
#include <map>
#include <list>
#include <vector>

using namespace FSTool;
using namespace WPTool;

struct TAG;  // stuct with content of tag 

typedef std::map<std::string, std::string> attribTAG;
typedef std::map<std::string, TAG> TAGList;
typedef std::vector<TAG> TAGArray;

struct TAG {
    std::string name;  // tag name
    std::string value; // tag value
    attribTAG attrib;  // tag properties
    TAGList subTAGs;   // child tags
};

class XML{
public:
    XML(std::string xml_file);
    ~XML();
    TAG getTAG(std::string tag_name, std::string prop = "", std::string val = ""); // get tag for Name, attrib, value
    TAGArray select(std::string tagName); // return list of tags from tag name 
private:
    file * source; // file with XML
    attribTAG get_tag_attrib(std::string src); // get tag prop from src
    std::string get_tag_value(std::string src); // return tag value 
    std::string get_tag_name(std::string src); // return tag name
    TAG get_sub_tag(int & start_pos);
    int get_tag_position(std::string tag_name, std::string prop = "", std::string val = "" );
};

#endif