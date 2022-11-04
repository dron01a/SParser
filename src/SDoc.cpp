#include "SDoc.h"

sp::XML_tree sp::get_XML_tree(std::string _source){ 
    size_t * first = new size_t(_source.find("<?"));
    size_t * last = new size_t(_source.find("?>"));
    sp::tag _root;
    sp::attrib_tag _atrs;
    if(*first != _NPOS && *last != _NPOS){
        std::string * temp = new std::string(sp::content(_source, * first, * last + 1));
        _atrs = attrib(*temp);
        delete temp;
        std::string root_tag_name = sp::name(sp::content(_source, *last + 1 , _source.size() -1));
        sp::small_parser _parser(sp::content(_source, *last + 1 , _source.size() -1));
        _root = _parser.get_tag(root_tag_name); 
    }
    else{
        throw 0; 
    }
    XML_tree _result(_root); 
    _result.version() = _atrs["version"];
    _result.encoding() = _atrs["encoding"];
    delete first;
    delete last;
    return _result;
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

std::string sp::transform_XML_to_string(sp::XML_tree & _tree){
    std::string result = "<?xml version=\"" + _tree.version() + "\" encoding=\"" + _tree.encoding() + "\" ?>"; 
    result += transform_tag(_tree.root());
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


