#include "SPtools.h"
#include "SPbase.h"
#include "SParser.h"
#include <iostream>

int main(){
    std::string src =
                    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" 
                    "<root>"
                        "<A>text Text text</A>" 
                        "<b>"
                            "<ff>78</ff>"
                            "<A>718</A>"
                            "<A>718</A>"
                        "</b> "
                       "</root>";
    sp::small_parser p(src);
    
    
    sp::tag _a = p.get_tag("root");
    sp::small_free _f(_a);
    //std::cout << sp::transform_tag(_a) << std::endl;
    sp::get_XML_tree(src); 
    return 0;
}