#ifndef __SPARSER__H__
#define __SPARSER__H__

#include "SPbase.h"

namespace sp {

    typedef tag_list::iterator tl_it; // tag list iterator 

    struct base_free{
    protected:
    //    virtual tag tag(std::string name, attrib_tag _attrib = {{"", ""}}) = 0;
        tag root;
    };

    class small_free : protected base_free , protected small_parser{
    public:
        tl_it get_tag(std::string name, attrib_tag _attrib = {{"", ""}});
    private:
        tl_it get_tag(tl_it & _tag, std::string name, attrib_tag _attrib = {{"", ""}});
    };


    class XML_tree : small_free{
        // XML version 
       // std::string ver;
  //      
       // // XML encoding 
  //      std::string enc;
//
    };


    class HTML_tree : small_free{
        // tag 
     //   tag & body();

     //   tag & title();

     //   tag & head();


    };

};

#endif