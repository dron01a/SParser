#include "SPtools.h"

std::string sp::content(std::string _src, int _posStart, int _posEnd){
    return _src.substr(_posStart, _posEnd - _posStart + 1);
}

std::string sp::text(std::string _src){
    std::string result;
    if(_src[_src.length() - 1] != '/'){
        int * first_char = new int(_src.find_first_of(">")); 
        int * last_char = new int(_src.find_first_of("<", * first_char));
        if((*first_char != _NPOS) && (*last_char != _NPOS)){
            result = content(_src, *first_char + 1, *last_char - 1);
        }
        delete first_char;
        delete last_char;
    }
    return result;
}

sp::attrib_tag sp::attrib(std::string src){
    sp::attrib_tag result;
    int * open_char = new int(src.find(" ", src.find("<"))); 
    int * close_char = new int(src.find(">"));
    if(*open_char != _NPOS || *close_char != _NPOS){
        std::string prop_str = content(src,*open_char + 1, *close_char ); // string with prop
        int _start = 0;
        int _del = prop_str.find("=");
        while(_del != _NPOS){
            int _brS = prop_str.find("\"", _del);
            int _brE = prop_str.find("\"", _brS + 1);
            std::string _key = content(prop_str, _start, _del - 1);
            std::string _val = content(prop_str, _brS + 1, _brE - 1);
            result[_key] = _val;
            _start = src.find_first_not_of(" \t", _brE + 1) + 1;
            _del = prop_str.find("=", _del + 1);
        }    
    }
    delete open_char;
    delete close_char;
    return result;
}

std::string sp::name(std::string src){
    int open_char = src.find_first_of("<"); 
    int close_char = src.find_first_of(">", open_char);
    if(open_char == close_char || open_char == _NPOS || close_char == _NPOS){
        throw "error";
    }
    return content(src, open_char + 1, close_char -1);
}

bool sp::comp_attr(attrib_tag & _attrsA, attrib_tag & _attrsB){
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