#include "SParser.h"

sp::tag & sp::small_free::get_tag(std::string name, attrib_tag _attrib){
    return get_tag(_root,name,_attrib);
}

sp::tag & sp::small_free::get_tag(tag & _tag, std::string name, attrib_tag _attrib){
    tag result;
    for(tl_it iter = _tag.subTAGs.begin(); iter != _tag.subTAGs.end() ; iter++){
        if(iter->first == name && comp_attr(iter->second.attrib,_attrib) == true){
            result = iter->second;
            break;
        }
        result = get_tag(iter->second, name, _attrib);
        if(result.name == name && comp_attr(result.attrib,_attrib) == true){
            break;
        }
    }
    return result;
}

sp::tag_array sp::small_free::select(std::string name, attrib_tag _attrib){
    tag_array result;
    for(tl_it iter = _root.subTAGs.begin(); iter != _root.subTAGs.end(); iter++){
        if(iter->first == name && comp_attr(iter->second.attrib,_attrib) == true){
            result.push_back(iter->second);
        }
        small_free _sub(iter->second);
        tag_array tmp = _sub.select(name,_attrib);
        if(tmp.size() != 0){
            result.insert(result.end(), tmp.begin(), tmp.end());
        }
    }
    return result;
}

sp::tag & sp::HTML_tree::body(){
    return get_tag("body");
}

sp::tag & sp::HTML_tree::title(){
    return get_tag("title");
}

sp::tag & sp::HTML_tree::head(){
    return get_tag("head");
}

sp::tag & sp::HTML_tree::h1(){
    return get_tag("h1");
}

sp::tag_array sp::HTML_tree::h2(){
    return select("h2");
}

sp::tag_array sp::HTML_tree::h3(){
    return select("h3");
}

sp::tag_array sp::HTML_tree::h4(){
    return select("h4");
}

sp::tag_array sp::HTML_tree::h5(){
    return select("h5");
}

sp::tag_array sp::HTML_tree::h6(){
    return select("h6");
}

sp::HTML_tree sp::get_HTML_tree(std::string _source){
    sp::small_parser _parser(_source);
    return sp::HTML_tree(_parser.get_tag("html"));
}

std::string sp::transform_HTML_to_string(HTML_tree & _tree){
    std::string result = "<!DOCTYPE html>"; 
    result+=transform_tag(_tree.root());
    return result;
}

std::string sp::transform_tag(sp::tag _tag){
    std::string result = "<" + _tag.name;
    if(_tag.attrib.size() != 0){
        for(sp::attrib_tag::iterator it = _tag.attrib.begin(); it != _tag.attrib.end(); it++){
            result += " " + it->first + "=\"" + it->second +"\""; // add tag attrib`s to string
        }
    }
    if(_tag.type == sp::_close_tag){
        result += "/>";
    }
    else{
        result += ">";
        if(!_tag.text.empty()){
            result+= _tag.text;
        }
        if(_tag.type == sp::_tag){
            for(sp::tl_it it = _tag.subTAGs.begin(); it != _tag.subTAGs.end(); it++){
                result+=transform_tag(it->second);
            }
        }
        result += "</" + _tag.name + ">";
    }   
    return result; 
}
