#ifndef __SPPBASE__H__
#define __SPPBASE__H__

#include <map>
#include <vector>
#include <string>
#include <algorithm>

#define _NPOS std::string::npos 

namespace sp {

    struct tag; // declaration struct with tag information-

    typedef std::map<std::string, std::string> attrib_tag;
    typedef std::multimap<std::string, tag> tag_list;
    typedef std::vector<tag> tag_array;

    enum _type{
        _tag = 0,
        _close_tag,
        _text
    };

    struct tag {
        std::string name; // tag name
        std::string text; // text in tag 
        attrib_tag attrib; // tag properties
        tag_list subTAGs;  // child tags
        _type type;  // type of tag 
    };

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

    /**
     *  return type of token
     *  @param token -> string with token  
    */
    _type get_token_type(std::string & token); 

};

#endif 