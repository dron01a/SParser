#ifndef __SXML__H__
#define __SXML__H__

#include "FSTool.h"
#include "WPTool.h"
#include <map>
#include <list>
#include <vector>


namespace SXML{ 

    struct TAG;  // declaration struct with tag information

    class XML; // class for work with XML
    class XMLfile; // class for work with XML in file
    class XMLstring; // class for work with XML in string 

    typedef std::map<std::string, std::string> attribTAG;
    typedef std::map<std::string, TAG> TAGList;
    typedef std::vector<TAG> TAGArray;

    struct TAG {
        std::string name;  // tag name
        std::string value; // tag value
        attribTAG attrib;  // tag properties
        TAGList subTAGs;   // child tags
    };

    class XMLread{
    protected:
        attribTAG attrib(std::string src);     // get tag prop from src
        std::string TAGvalue(std::string src); // return tag value 
        std::string TAGname(std::string src);  // return tag name
    };

    class XML : protected XMLread{
    public:
        virtual TAG getTAG(std::string tag_name, std::string prop = "", std::string val = ""); // get tag for n0ame, attrib, value
        virtual TAGArray select(std::string tagName); // return list of tags from tag name 
        virtual TAGList ALL(); // return all tags 
        virtual int findTAG(std::string tag_name, std::string prop = "", std::string val = ""); // find tag in XML
    };

    class XMLfile : public XML{
    public:
        XMLfile(std::string fileName); // class constructor
        ~XMLfile(); // class destructor
        TAG getTAG(std::string tag_name, std::string prop = "", std::string val = ""); // get tag for n0ame, attrib, value
        TAGArray select(std::string tagName); // return list of tags from tag name 
        TAGList ALL(); // return all tags 
        int findTAG(std::string tag_name, std::string prop = "", std::string val = ""); // find tag in XML
    private:
        FSTool::file * source; // file with XML
        TAG get_sub_tag(int & start_pos);
        int get_tag_position(std::string tag_name, std::string prop = "", std::string val = "" );
    };

    class XMLstring : public XML{
        XMLstring(std::string source); // constructor
        ~XMLstring(); // class destructor
        TAG getTAG(std::string tag_name, std::string prop = "", std::string val = ""); // get tag for n0ame, attrib, value
        TAGArray select(std::string tagName); // return list of tags from tag name 
        TAGList ALL(); // return all tags 
        int findTAG(std::string tag_name, std::string prop = "", std::string val = ""); // find tag in XML
    private:
        std::string *source; // source string 
    };
}
 

#endif