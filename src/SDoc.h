#ifndef __SDOC__H__
#define __SDOC__H__

#include "SPbase.h"
#include "SParser.h"

namespace sp{
    
    HTML_tree get_HTML_tree(std::string _source); 

    XML_tree get_XML_tree(std::string _source);

    std::string transform_HTML_to_string(HTML_tree & _tree);

    std::string transform_XML_to_string(HTML_tree & _tree, std::string encoding, std::string version);

    std::string transform_tag(sp::tag _tag);

};

#endif 