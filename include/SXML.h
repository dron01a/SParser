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

    // load XML from source
    // f - load from file
    // s - load from string 
    XML* loadXML(std::string _rec, std::string mode = "s"); 

    // load XML from file 
    XML* loadXMLFile(std::string fileName);

    // load XML from string 
    XML* loadXMLStr(std::string text); 

    typedef std::map<std::string, std::string> attribTAG;
    typedef std::map<std::string, TAG> TAGList;
    typedef std::vector<TAG> TAGArray;

    struct TAG {
        std::string name;  // tag name
        std::string value; // tag value
        attribTAG attrib;  // tag properties
        TAGList subTAGs;   // child tags
    };

    class XMLreader{
    protected:
        attribTAG attrib(std::string src);     // get tag prop from src
        std::string TAGvalue(std::string src); // return tag value 
        std::string TAGname(std::string src);  // return tag name
    };

    class XML : protected XMLreader{
    public:
        virtual TAG getTAG(std::string tag_name, std::string prop = "", std::string val = "") = 0; // get tag for n0ame, attrib, value
        virtual TAGArray select(std::string tagName) = 0; // return list of tags from tag name 
        virtual int findTAG(std::string tag_name, std::string prop = "", std::string val = "") = 0; // find tag in XML
    };
    
    // class for work with XML in file
    class XMLdocument : public XML{
    public:
        XMLdocument(std::string fileName); // class constructor
        ~XMLdocument(); // class destructor
        TAG getTAG(std::string tag_name, std::string prop = "", std::string val = ""); // get tag for n0ame, attrib, value
        TAGArray select(std::string tagName); // return list of tags from tag name 
        int findTAG(std::string tag_name, std::string prop = "", std::string val = ""); // find tag in XML
    private:
        FSTool::file * source; // file with XML
        TAG get_sub_tag(int & start_pos);
    };

    // class for work with XML in string 
    class XMLstring : public XML{
    public:
        XMLstring(std::string source); // constructor
        ~XMLstring(); // class destructor
        TAG getTAG(std::string tag_name, std::string prop = "", std::string val = ""); // get tag for n0ame, attrib, value
        TAGArray select(std::string tagName); // return list of tags from tag name  
        int findTAG(std::string tag_name, std::string prop = "", std::string val = ""); // find tag in XML
    private:
        std::string *source; // source string 
        TAG get_sub_tag(int & start_pos);
    };

}
 

#endif