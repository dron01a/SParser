#ifndef __SXML__H__
#define __SXML__H__

#include "FSTool.h"
#include "WPTool.h"
#include <map>
#include <list>
#include <vector>

#define _NPOS std::string::npos

namespace SXML{ 

    enum _tokenType{
        _tag = 0,
        _closeTag,
        _text
    };
    
    _tokenType getTokenType(std::string token);

    struct TAG;  // declaration struct with tag information-

    typedef std::map<std::string, std::string> attribTAG;
    typedef std::map<std::string, TAG> TAGList;
    typedef std::vector<TAG> TAGArray;

    struct TAG {
        std::string name;  // tag name
        std::string text;  // text in tag 
        attribTAG attrib;  // tag properties
        TAGList subTAGs;   // child tags
    };

    class XML; // class for work with XML

    // load XML from file 
    XML* loadXMLFile(std::string fileName);

    // load XML from string 
    XML* loadXMLStr(std::string text); 

    class XMLreader{
    protected:
        attribTAG attrib(std::string src);    // get tag prop from src     
        std::string TAGtext(std::string src); // return tag value 
        std::string TAGname(std::string src); // return tag name
    };

    class XML : protected XMLreader{
    public:
        XML(std::string source); // class constructor 
        TAG getTAG(std::string tagName, std::string prop = "", std::string val = ""); // get tag for name, attrib, value 
        TAGArray select(std::string tagName);  // return list of tags from tag name
    private:
        std::string source; // string with XML
        int findTAG(std::string tag_name, std::string prop = "", std::string val = ""); // find tag in XML
        TAG getNextTag(int & position); // get next tag in string
    };
    

}
 

#endif