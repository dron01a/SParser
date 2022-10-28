#ifndef __SPARSER__H__
#define __SPARSER__H__

#include "SPbase.h"

namespace sp {

    typedef tag_list::iterator tl_it; // tag list iterator 

    struct base_free{
    protected:
        tag _root;
    };

    class small_free : protected base_free {
    public:
        small_free(tag _root) { this->_root = _root;} ;
        tag & get_tag(std::string name, attrib_tag _attrib = {{"", ""}});
        tag_array select(std::string name, attrib_tag _attrib = {{"", ""}});
        tag & root() { return _root; }
    private:
        tag & get_tag(tag & _tag, std::string name, attrib_tag _attrib = {{"", ""}});
    };


    class XML_tree : small_free{
        // XML version 
       // std::string ver;
  //      
       // // XML encoding 
  //      std::string enc;
//
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