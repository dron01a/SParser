#ifndef __SXML__H__
#define __SXML__H__

#include <map>
#include <list>
#include <vector>

#include "FSTool.h"
#include "WPTool.h"

#define _NPOS std::string::npos

namespace SXML{ 

    enum _tokenType{
        _tag = 0,
        _closeTag,
        _text
    };
    
    _tokenType getTokenType(std::string token);
    std::string content(std::string _src, int _posStart, int _posEnd ); // return tag content

    struct TAG;     // declaration struct with tag information-
    class XML;      // class for work with XML

    typedef std::map<std::string, std::string> attribTAG;
    typedef std::multimap<std::string, TAG> TAGList;
    typedef std::vector<TAG> TAGArray;

    struct TAG {
        std::string name; // tag name
        std::string text; // text in tag 
        attribTAG attrib; // tag properties
        TAGList subTAGs;  // child tags
        _tokenType type;  // type of tag 
    };

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
        TAG root(); // return root tag 
    private:
        std::string source; // string with XML
        int findTAG(std::string tag_name, std::string prop = "", std::string val = ""); // find tag in XML
        TAG getNextTag(int & position); // get next tag in string
        void parceTagContent(TAG & _tag, int _start, int _end);
        void parceTagAttrs(TAG & _tag, int _start, int _end);
    };

    // convert tag to string
    static std::string convertTagAttr(TAG _tag);
    static std::string convertTAG(int & lvl, TAG _tag);
    std::string convertTAG(TAG _tag); 

    // write XML free to file  
    int writeXML(std::string fileName,TAG root, std::string version = "1.0" , std::string enc = "utf-8");
}
 
#endif