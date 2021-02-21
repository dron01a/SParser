#include "SXML.h"

std::string SXML::XML::TAGvalue(std::string src){
    std::string result;
    if(src[src.length() - 1] != '/'){
        int * first_char = new int(src.find_first_of("<")); 
        int * last_char = new int(src.find_last_of("<"));
        if((*first_char != std::string::npos) && (*last_char != std::string::npos)){
            if(*first_char != *last_char){
                int * open_char = new int(src.find_last_of("<")); // get first position of "<"
                int * close_char = new int(src.find_first_of(">")); // get first position of ">"       
                int * len = new int(*open_char - *close_char);
                if(*len > 0){
                    result = src.substr(*close_char+1,*len-1);
                }
                delete len;
                delete open_char;
                delete close_char;
            }
        }
        delete first_char;
        delete last_char;
    }
    return result;
}

SXML::attribTAG SXML::XML::attrib(std::string src){
    SXML::attribTAG result;
    int * open_char = new int(src.find_first_of("<")); 
    int * close_char = new int(src.find_first_of(">"));
    int * prop_len = new int((*close_char - *open_char) - 1);
    if(*prop_len > 0){
        std::string *prop_str = new std::string(src.substr(*open_char,*prop_len));
        WPTool::string_content * tag_prop = new WPTool::string_content(*prop_str, " \"\'=");
        if(tag_prop->size() != 1 ){
            std::string *key = new std::string;
            std::string *val = new std::string;
            for (int i = 1; i < tag_prop->size() - 1; i+=2){
                *key = tag_prop->get(i);
                *val = tag_prop->get(i+1);
                result.insert(std::make_pair(*key,*val));
            }
            delete key, val;
        }
        delete prop_str;
        delete tag_prop;
    }
    delete open_char;
    delete close_char;
    delete prop_len;
    return result;
}

std::string SXML::XML::TAGname(std::string src){
    WPTool::string_content tag_prop(src, " \t<>\"\'=");
    return tag_prop[0];
}