#include "SPtools.h"
#include "SPbase.h"
#include <iostream>

int main(){
    std::string src = "<A>text Text text</A> <b>44</b> <A>78</A>";
    sp::small_parser p(src);
    
    
    sp::tag_array _a = p.select("A");
    std::cout << _a[1].text << std::endl;
    return 0;
}