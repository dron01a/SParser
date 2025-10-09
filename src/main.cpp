//#include "SPtools.h"
#include "SPbase.h"
#include "SParser.h"
#include "SDoc.h"
#include <iostream>

int main(){
    
    sp::HTML_tree __d = sp::load_HTML_from_file("test.html");

    __d.get_tag("img").attrib["src"] = "none";

    std::cout << __d.body().subTAGs.begin()->second.attrib["src"] << std::endl; 

    return 0;
}