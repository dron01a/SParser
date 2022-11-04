#ifndef __SDOC__H__
#define __SDOC__H__

#include "SPbase.h"
#include "SParser.h"

namespace sp{
    
    /**
     * 
     * return HTML_free from string 
     * 
     * @param _source - source string 
     * 
    */
    HTML_tree get_HTML_tree(std::string _source); 

    /**
     * 
     * return XML_free from string 
     * 
     * @param _source - source string 
     * 
    */
    XML_tree get_XML_tree(std::string _source);

    /**
     * 
     * converts an object of sp::HTML_free to a string
     * 
     * @param _tree - object 
     * 
    */
    std::string transform_HTML_to_string(HTML_tree & _tree);

    /**
     * 
     * converts an object of sp::XML_free to a string
     * 
     * @param _tree - object 
     * 
    */
    std::string transform_XML_to_string(XML_tree & _tree);

    /**
     * 
     * converts an object of sp::tag to a string
     * 
     * @param _tag - object 
     * 
    */
    std::string transform_tag(sp::tag _tag);

};

#endif 