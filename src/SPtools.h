#ifndef __SPTOOLS__H__
#define __SPTOOLS__H__

#define _NPOS std::string::npos

#include <string>
#include <map>
#include <list>
#include <vector>

namespace sp {
    
    typedef std::map<std::string, std::string> attrib_tag;

    /*    
        return text of HTML/XML element
        param 1: _src -> string with XML/HTML text
    */
    std::string name(std::string _src); 
    
    /*    
        return text of HTML/XML element
        param 1: _src -> string with XML/HTML text
    */
    attrib_tag attrib(std::string _src);
    
    /*    
        return text of HTML/XML element
        param 1: _src -> string with XML/HTML text
    */
    std::string text(std::string _src);

    /* 
        return content in string( _src ) between positions _posStart and _posEnd 
        param 1: _src -> string with text
        param 2: _posStart -> start position
        param 3: _posEnd -> end position
    */
    std::string content(std::string _src, int _posStart, int _posEnd ); 

    /*
        return true if content in attrsA == _attrsB
        param 1: _attribA - tag A attributes
        param 2: _attribB - tag B attributes
    */
    bool comp_attr(attrib_tag & _attrsA, attrib_tag & _attrsB);
};

#endif