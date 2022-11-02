#ifndef __SPPBASE__H__
#define __SPPBASE__H__

#include "SPtools.h"

namespace sp {

    struct tag; // declaration struct with tag information-

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

    class small_parser{
    public:
        /*
            class constructor 
            param 1: _src -> string with XML/HTML text
        */
        small_parser(std::string _src) { source = new std::string(_src); };

        /**
         * class destructor 
         * 
        */
        ~small_parser() { delete source; }

        /*
            get tag for name, attribute, value
            param 1: name - name of tag 
            param 2: _attrib - tag attributes
        */
        tag get_tag(std::string name, attrib_tag _attrib = {{"", ""}}); 

        /*
            get array of tag for same names 
            param 1: name - name of tag 
        */
        tag_array select(std::string name);
        
        /**
         * return count of tags
         * param 1: name - name of tag 
        */
        int count(std::string name, attrib_tag _attrib = {{"", ""}});

    private:

        // string with code 
        std::string *source;

        /*
            parse content in tag, set child tag`s and text
            param 1: _tag -> tag for setting values
            param 2: _start -> start position
            param 3: _end -> end position
        */
        void parse_tag_content( tag & _tag, size_t _start, size_t _end);
 
        /*
            find tag in XML
            return tag position
            param 1: name - name of tag 
            param 2: _attrib - tag attributes
        */
        size_t find_tag(std::string name, attrib_tag _attrib = {{"", ""}});

        /*
            parse content in tag, set child tag`s and text
            param 1: _tag -> tag for setting values
            param 2: _start -> start position
            param 3: _end -> end position
        */
        void set_tag_attrs(tag & _tag, size_t _start, size_t _end);

        /*
            found start and end position of string with tag
            param 1: name - name of tag 
            param 2: _start -> start position
            param 3: _end -> end position
        */
        void set_range(std::string _name, size_t & _start, size_t & _end);\
        

        // 
        /*
            get next tag in string
            param 1: currient position
        */
        tag get_next_tag(int & position); 
    };

    /*
        return type of token
        param 1: token -> string with token  
    */
    _type get_token_type(std::string & token); 

};

#endif 