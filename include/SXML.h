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

    class XML{
    public:
        // class constructor
        XML(std::string source); 

        // get tag for name, attrib, value
        TAG getTAG(std::string tagName, attribTAG _attrib = {{"", ""}}); 

        // return list of tags from tag name
        TAGArray select(std::string tagName);  
        
        // return count of tags with the same attributes
        int count(std::string tagName, attribTAG _attrib = {{"", ""}});

        // return root tag 
        TAG root(); 
    private:

        // return true if content in attrsA == _attrsB
        bool compareTagAttrs(attribTAG _attrsA, attribTAG _attrsB); 

        // found start and end position of string with tag
        void setRange(std::string _name, size_t & _begin, size_t & _end);

        // find tag in XML
        int findTAG(std::string tagName, attribTAG _attrib = {{"", ""}});
        
        // methods to get tag params
        attribTAG attrib(std::string src);    // get tag prop from src     
        std::string TAGtext(std::string src); // return tag value 
        std::string TAGname(std::string src); // return tag name

        // parce content in xml tag, set child tag`s and text
        void parceTagContent(TAG & _tag, int _start, int _end);

        // parce arttr in tag, set name to tag in object        
        void parceTagAttrs(TAG & _tag, int _start, int _end);
      
        // get next tag in string
        TAG getNextTag(int & position); 
        
        std::string source; // string with XML
    };

    // convert tag to string
    static std::string convertTagAttr(TAG _tag);
    static std::string convertTAG(int & lvl, TAG _tag);
    std::string convertTAG(TAG _tag); 

    // write XML free to file  
    int writeXML(std::string fileName,TAG root, std::string version = "1.0" , std::string enc = "utf-8");
}
 

#endif