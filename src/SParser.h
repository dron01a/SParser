#ifndef __SPARSER__H__
#define __SPARSER__H__

#include "SPbase.h"

namespace sp {

    typedef tag_list::iterator tl_it; // tag list iterator 

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
        
        /**
         *  get tag for name, attribute, value
         *  @param name - name of tag 
         *  @param _attrib - tag attributes
        */
        tag get_tag(std::string name, attrib_tag _attrib = {{"", ""}}); 
        
        /**
         *  get array of tag for same names 
         *  @param name - name of tag 
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

        /**
         *  parse content in tag, set child tag`s and text
         * @param _tag -> tag for setting values
         * @param _start -> start position
         * @param _end -> end position
        */
        void parse_tag_content( tag & _tag, size_t _start, size_t _end);
 
        /**
         * find tag in XML
         * return tag position
         * @param name - name of tag 
         * @param _attrib - tag attributes
        */
        size_t find_tag(std::string name, attrib_tag _attrib = {{"", ""}});

        /**
         * parse content in tag, set child tag`s and text
         * @param _tag -> tag for setting values
         * @param _start -> start position
         * @param _end -> end position
        */
        void set_tag_attrs(tag & _tag, size_t _start, size_t _end);

        /**
         *  found start and end position of string with tag
         *  @param name - name of tag 
         *  @param _start -> start position
         *  @param _end -> end position
        */
        void set_range(std::string _name, size_t & _start, size_t & _end);\
        

        /**
         *  get next tag in string
         *  @param currient position
        */
        tag get_next_tag(int & position); 
    };

    struct base_free{
    protected:
        tag _root;
    };

    class small_free : protected base_free {
    public:
        /**
         * class constructor 
         * 
         * @param _root: root tag of free  
         * 
        */
        small_free(tag _root) { this->_root = _root;} ;
        /**
         *  get tag for name, attribute, value
         *  @param name - name of tag 
         *  @param _attrib - tag attributes
        */
        tag & get_tag(std::string name, attrib_tag _attrib = {{"", ""}});
        /**
         *  get array of tag for same names 
         *  @param name - name of tag 
         *  @param _attrib - tag attributes
        */
        tag_array select(std::string name, attrib_tag _attrib = {{"", ""}});
        /**
         * return root tag 
        */
        tag & root() { return _root; }
    private:
        tag & get_tag(tag & _tag, std::string name, attrib_tag _attrib = {{"", ""}});
    };


    class XML_tree : public small_free{
    public:
        XML_tree(tag _tag) : small_free(_tag) {};
        /**
         *  return version 
        */    
        std::string & version();
        /**
         *  return encoding
        */  
        std::string & encoding();
    private:
        // XML version 
        std::string ver;      
        // XML encoding 
        std::string enc;
    };


    class HTML_tree : public small_free{
    public:
        HTML_tree(tag _tag) : small_free(_tag) {};
        tag & body();
        tag & title();
        tag & head();
        tag & h1();
        tag_array h2();
        tag_array h3();
        tag_array h4();
        tag_array h5();
        tag_array h6();
    };
};

#endif