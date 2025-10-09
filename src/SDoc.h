#ifndef __SDOC__H__
#define __SDOC__H__

#include "SPbase.h"
#include "SParser.h"

#ifdef __FSTOOL__H__
    #include "FSTool/FSTool.h"
#else
    #include <fstream>
#endif

namespace sp{
    
    enum md_type{
        HTML,
        XML
    };

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
    std::string transform_tag(sp::tag & _tag);
    
    /**
     * 
     * return a formatted string
     * 
     * @param source - string with HTML/XML text  
     * 
    */
    std::string format(std::string & source);

    /**
     * 
     * load XML tree from file 
     * 
     * @param file_name name of file 
    */ 
    XML_tree load_XML_from_file(std::string file_name);

    /**
     * 
     * load HTML tree from file 
     * 
     * @param file_name name of file 
    */ 
    HTML_tree load_HTML_from_file(std::string file_name);

    /**
     * 
     * load tree from file
     * 
     * @param tree pointer to base class of tree
     * @param file_name name of file 
     * @param type tipe of tree
     * 
    */
    void load_from_file(small_free * tree,std::string file_name,md_type type);

    /**
     * load tree from file
     * 
     * @param tree - object 
     * @param file_name name of file 
    */
    void load_HTML_to_file(HTML_tree & tree, std::string file_name);

    /**
     * load tree from file
     * 
     * @param tree - object 
     * @param file_name name of file 
    */
    void load_XML_to_file(XML_tree & tree, std::string file_name);
};

#endif 