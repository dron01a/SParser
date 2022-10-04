#include "SParser.h"

sp::tl_it sp::small_free::get_tag(std::string name, attrib_tag _attrib = {{"", ""}}){
    tl_it result;
    for(tl_it iter = root.subTAGs.begin(); iter != root.subTAGs.end() ; iter++){
        if(iter->first == name && comp_attr(iter->second.attrib,_attrib) == true){
            result = iter;
            break;
        }
        result = get_tag(iter, name, _attrib);
        if(result->first == name && comp_attr(result->second.attrib,_attrib) == true){
            break;
        }
    }
    return result;
}

sp::tl_it sp::small_free::get_tag(tl_it & _tag, std::string name, attrib_tag _attrib = {{"", ""}}){
    tl_it result;
    for(tl_it iter = _tag->second.subTAGs.begin(); iter != _tag->second.subTAGs.end() ; iter++){
        if(iter->first == name && comp_attr(iter->second.attrib,_attrib) == true){
            result = iter;
            break;
        }
        result = get_tag(iter, name, _attrib);
        if(result->first == name && comp_attr(result->second.attrib,_attrib) == true){
            break;
        }
    }
    return result;
}