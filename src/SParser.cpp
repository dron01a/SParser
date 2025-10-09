#include "SParser.h"

sp::tag sp::small_parser::get_tag(std::string name, attrib_tag _attrib){
    tag _result;
    size_t *tag_position = new size_t(find_tag(name,_attrib)); // tag position
    size_t *close_char = new size_t(source->find_first_of(">", *tag_position)); 
    set_tag_attrs(_result, *tag_position, *close_char);
    if(_result.type != _close_tag){
        size_t *close_tag_position = new size_t(source->find("</" + name + ">", *close_char));
        if(*close_tag_position != _NPOS){
            parse_tag_content(_result, * close_char, * close_tag_position - 1);
        }
        delete close_tag_position;
    }
    delete tag_position;
    delete close_char;
    return _result;
} 

sp::tag_array sp::small_parser::select(std::string name){
    tag_array result;
    size_t *tag_position = new size_t(source->find("<" + name)); // tag position
    size_t *close_char = new size_t(source->find(">", *tag_position)); 
    for(size_t count=0; *tag_position != _NPOS; count++){
        size_t *close_tag_position = new size_t(source->find("</" + name + ">", *close_char));
        result.resize(result.size()+1);
        set_tag_attrs(result[count],*tag_position, *close_char);
        if(result[count].type != _close_tag){
            if(*close_tag_position != _NPOS){
                parse_tag_content(result[count],*close_char, *close_tag_position);
            }
        }
        *tag_position = source->find("<" + name, *tag_position + 1);
        *close_char = source->find(">", *tag_position);
        delete close_tag_position;
    }
    delete tag_position;
    delete close_char;
    return result;
}

size_t sp::small_parser::find_tag(std::string name, attrib_tag _attrib){
    size_t result = 0;
    size_t closeChar;
    attrib_tag atrs;
    bool passed = false; // set true if attributes match 
    while(!passed){
        set_range(name, result, closeChar);
        if(result == _NPOS || closeChar == _NPOS){
            throw "tag not found";
        }
        atrs = attrib(content(*source, result, closeChar));  // compare attributes and value of tags
        passed = comp_attr(atrs, _attrib);
    }
    return result;
}

void sp::small_parser::set_range(std::string _name, size_t & _begin, size_t & _end){
    _begin = source->find("<" + _name, (_begin == 0) ? 0 : _begin + 1); // begin position 
    _end = source->find_first_of(">", _begin);
    while(_begin != _NPOS && _end != _NPOS){
        if((name(content(*source, _begin, _end)) == _name)){
            break;
        }
        _begin = source->find("<" + _name, _begin + 1); // set new positions 
        _end = source->find_first_of(">", _begin);
    }
}

void sp::small_parser::set_tag_attrs(tag & _tag, size_t _start, size_t _end){
    std::string *temp = new std::string(content(*source, _start, _end));
    _tag.attrib = attrib(*temp);     // set attrib  
    _tag.name = name(*temp);      // set name
    _tag.type = get_token_type(*temp); // set type
    delete temp;
}

void sp::small_parser::parse_tag_content(tag & _tag, size_t _start, size_t _end){
    std::string *tag_content = new std::string(content(*source, _start + 1, _end - 1));
    if(get_token_type(*tag_content) == _text){
        _tag.text = *tag_content; // set tag text
    }
    else{
        for(int tagCount = _start + 1; tagCount < _end - 1; tagCount++){
            tag sub_tag = get_next_tag(tagCount); 
            if(sub_tag.name.find("/") == _NPOS){
                _tag.subTAGs.insert(std::make_pair(sub_tag.name,sub_tag)); // add sub tags 
            }
        }
    }
    delete tag_content;
}

int sp::small_parser::count(std::string name, attrib_tag _attrib){
    int result = 0;       // count 
    size_t begin_pos = 0; // start position 
    size_t end_pos = 0;   // end position
    attrib_tag atrs;
    bool passed = false;
    while(begin_pos != _NPOS && end_pos != _NPOS){
        set_range(name, begin_pos, end_pos);
        if(begin_pos == _NPOS || end_pos == _NPOS){
            break;
        }
        atrs = attrib(content(*source, begin_pos, end_pos));
        passed = comp_attr(atrs, _attrib);
        if(passed){
            result++;
        }
    }
    return result;
}

sp::tag sp::small_parser::get_next_tag(int & position){
    tag result;
    size_t *open_char = new size_t(source->find("<", position));
    size_t *close_char = new size_t(source->find(">", position)); 
    set_tag_attrs(result, *open_char, *close_char);
    if(result.type != _close_tag){
        size_t *close_tag_position = new size_t(source->find("</" + result.name + ">", *close_char));
        if(*close_tag_position != _NPOS){
            parse_tag_content(result,*close_char, *close_tag_position);
            position = source->find_first_of(">", *close_tag_position + 1);
        }
        else{
            position = source->find_first_of(">", position + 1);
        }
        delete close_tag_position;
    }
    else{
        position = source->find_first_of(">", position + 1);
    }
    delete open_char;
    delete close_char;
    return result;
}

sp::tag & sp::small_free::get_tag(std::string name, attrib_tag _attrib){
    return get_tag(_root,name,_attrib);
}

sp::tag & sp::small_free::get_tag(tag & _tag, std::string name, attrib_tag _attrib){
    for(tl_it iter = _tag.subTAGs.begin(); iter != _tag.subTAGs.end() ; iter++){
        if(iter->first == name && comp_attr(iter->second.attrib,_attrib) == true){
            return iter->second;
            break;
        }
        tag temp = get_tag(iter->second, name, _attrib);
        if(temp.name == name && comp_attr(temp.attrib,_attrib) == true){
            return get_tag(iter->second, name, _attrib);
            break;
        }
    }
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

std::string & sp::XML_tree::version(){
    return ver;
}

std::string & sp::XML_tree::encoding(){
    return enc;
}

 