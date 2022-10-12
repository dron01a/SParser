#include "SPbase.h"

sp::_type sp::get_token_type(std::string & token){
    _type _result = _tag;
    size_t * open_char = new size_t(token.find_first_of("<")); 
    size_t * close_char = new size_t(token.find_first_of(">", *open_char));
    if(*open_char == _NPOS || *close_char == _NPOS){    
        _result = _text;
    }
    if(*open_char != _NPOS && *close_char != _NPOS && token[*close_char - 1] == '/'){
        _result = _close_tag;
    }
    delete open_char;
    delete close_char;
    return _result;
}

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

bool sp::small_parser::comp_attr(attrib_tag & _attrsA, attrib_tag & _attrsB){
    if(_attrsA.size() < _attrsB.size()){
        if(_attrsB.begin()->first == "" && _attrsB.begin()->second == "" && _attrsB.size() == 1){
            return true;
        }
        return false;
    }
    for(auto itr = _attrsB.begin(); itr != _attrsB.end(); itr++ ){
        if(_attrsA[itr->first] == itr->second){
            continue;
        }
        else{
            return false;
        }
    }
    return true;
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

int sp::small_parser::count(std::string name, attrib_tag _attrib = {{"", ""}}){
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