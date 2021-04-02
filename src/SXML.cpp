#include "SXML.h"

SXML::XML * SXML::loadXMLFile(std::string fileName){
    if(FSTool::exists(fileName)){
        return new SXML::XML(FSTool::file(fileName).buff());
    }
    return NULL;
}

SXML::XML* SXML::loadXMLStr(std::string text){
    return new SXML::XML(text);
}

// src -> string with XML text
std::string SXML::XMLreader::TAGtext(std::string src){
    std::string source = src;
    std::string result;
    if(source[source.length() - 1] != '/'){
        int * first_char = new int(source.find_first_of(">")); 
        int * last_char = new int(source.find_first_of("<", * first_char));
        if((*first_char != _NPOS) && (*last_char != _NPOS)){
            if(*first_char != *last_char){
                int * len = new int(* last_char - * first_char);
                if(*len > 0){
                    result = source.substr(* first_char+1,*len-1); // cut the tag value from the string
                }
                delete len;
            }
        }
        delete first_char;
        delete last_char;
    }
    return result;
}

// src -> string with XML text
SXML::attribTAG SXML::XMLreader::attrib(std::string src){
    SXML::attribTAG result;
    int * open_char = new int(src.find_first_of("<")); 
    int * close_char = new int(src.find_first_of(">"));
    int * prop_len = new int((*close_char - *open_char) - 1);
    if(*prop_len > 0){
        std::string *prop_str = new std::string(src.substr(*open_char,*prop_len));
        WPTool::string_content * tag_prop = new WPTool::string_content(*prop_str, " \n\t\"\'=");
        if(tag_prop->size() != 1 ){
            std::string *key = new std::string;
            std::string *val = new std::string;
            for (int i = 1; i < tag_prop->size() - 1; i+=2){
                *key = tag_prop->get(i);
                *val = tag_prop->get(i+1);
                result.insert(std::make_pair(*key,*val));
            }
            delete key;
            delete val;
        }
        delete prop_str;
        delete tag_prop;
    }
    delete open_char;
    delete close_char;
    delete prop_len;
    return result;
}

// src -> string with XML text
std::string SXML::XMLreader::TAGname(std::string src){
    WPTool::string_content tag_prop(src, " \n\t<>\"\'=");
    return tag_prop[0];
}

SXML::_tokenType SXML::getTokenType(std::string token){
    int * open_char = new int(token.find_first_of("<")); 
    int * close_char = new int(token.find_first_of(">", *open_char));
    if(*open_char == _NPOS || *close_char == _NPOS){    
        return _text;
    }
    return _tag;
}

SXML::XML::XML(std::string source){
    this->source = source;
}

SXML::TAG SXML::XML::getTAG(std::string tag_name, std::string prop, std::string val){
    TAG result;
    int * found = new int(findTAG(tag_name,prop,val));
    int * closeChar = new int(source.find_first_of(">", * found)); 
    std::string * closeTag = new std::string("</" + tag_name + ">"); 
    int * closeTagPos = new int;
    *closeTagPos = source.find(*closeTag,*found + tag_name.size());
    std::string *temp = new std::string(source.substr(*found, *closeTagPos - *found + closeTag->size()));
    printf("%s\n", temp->c_str());
    result.attrib = attrib(*temp);  // get attrib
    result.name = TAGname(*temp);   // get name 
    if(*closeTagPos != _NPOS){
        std::string tagContent = temp->substr(tag_name.size() + 2 , * closeTagPos - *closeChar - 1);
        if(getTokenType(tagContent) == _text){
            result.text = tagContent;
        }
        if(getTokenType(tagContent) == _tag){
            for(int tagCount = * closeChar + 1; tagCount < *closeTagPos; tagCount++){
                TAG subTag = getNextTag(tagCount);
                result.subTAGs.insert(std::make_pair(subTag.name,subTag));
            }
        }
    }
    delete closeChar;
    delete closeTagPos;
    delete temp;
    delete closeTag;
    return result;
}

SXML::TAG SXML::XML::getNextTag(int & position){
    TAG result;
    int * openChar;
    int * closeChar; 
    std::string * temp;
    openChar = new int(source.find_first_of("<", position));
    closeChar = new int(source.find_first_of(">", position)); 
    temp = new std::string(source.substr(position,*closeChar - position + 1 ));
    if(!temp->empty()){
        result.attrib = attrib(*temp);
        result.name = TAGname(*temp);
        std::string * closeTag = new std::string("</" + result.name + ">"); 
        int * closeTagPos = new int;
        *closeTagPos = source.find(*closeTag, *closeChar);
        if(*closeTagPos != _NPOS){
            int *tagContentLen = new int(* closeTagPos - *closeChar - 1);
            std::string tagContent = temp->substr(result.name.size() + 2 ,*tagContentLen );
            if(getTokenType(tagContent) == _text){
                result.text = tagContent;
            }
            if(getTokenType(tagContent) == _tag){
                for(int tagCount = * closeChar + 1; tagCount < *closeTagPos; tagCount++){
                    TAG subTag = getNextTag(tagCount);
                    result.subTAGs.insert(std::make_pair(subTag.name,subTag));
                }
            }
            delete tagContentLen;
            position = *closeTagPos;
        }
        else{
            position = source.find_first_of(">", position);
        }
        delete closeTagPos;
        delete closeTag;
    }
    delete closeChar;
    delete temp;
    return result;
}


int SXML::XML::findTAG(std::string tag_name, std::string prop, std::string val){
    int result = source.find("<" + tag_name);
    int * closeChar = new int(source.find_first_of(">", result));
    while( TAGname(source.substr(result,*closeChar - result + 1 )) != tag_name ){
        result = source.find("<" + tag_name, result + 1);
        *closeChar = source.find_first_of(">", result + 1);
    }
    std::string * temp = new std::string;
    *temp = source.substr(result,*closeChar - result + 1 );
    WPTool::string_content * str;
    str = new WPTool::string_content(*temp," \n\t\"\'=");
    int * p_pos = new int(str->find(prop));
    int * v_pos = new int(str->find(val, * p_pos));
    int * l_pos = new int(*v_pos - *p_pos);
    if(prop == "" && val == "" ){
        * l_pos = 1;
    }
    while(*l_pos != 1){  
        result = source.find("<" + tag_name,result + 1);
        *closeChar = source.find_first_of(">", result + 1);
        if(result == _NPOS){
            break;
        }
        else{
            *temp = source.substr(result,*closeChar - result + 1 );
            str->set_string(*temp);
            * p_pos = str->find(prop);
            * v_pos = str->find(val, * p_pos);
            * l_pos = *v_pos - *p_pos;

        }
    }
    delete temp; 
    delete str;
    delete p_pos;
    delete v_pos;
    delete l_pos;
    return result;
}

SXML::TAGArray SXML::XML::select(std::string tagName){
    TAGArray result;
    int * tagPos; // position of tag in file 
    tagPos = new int(source.find("<" + tagName)); // get position 
    int * closeChar = new int(source.find_first_of(">", * tagPos)); 
    while(*tagPos != _NPOS){
        TAG tag; // for collect information
        std::string *temp = new std::string(source.substr(*tagPos, *closeChar - *tagPos  + 1 )); // string with tag 
        tag.attrib = attrib(*temp);
        tag.name = TAGname(*temp);
        std::string * closeTag = new std::string("</" + tag.name + ">"); 
        int * closeTagPos = new int;
        *closeTagPos = source.find(*closeTag,*tagPos);
        if(*closeTagPos != _NPOS){
            std::string tagContent = source.substr(* closeChar, * closeTagPos - *closeChar - 1);
            if(getTokenType(tagContent) == _text){
                tag.text = tagContent;
            }
            if(getTokenType(tagContent) == _tag){
                for(int tagCount = * closeChar + 1; tagCount < *closeTagPos; tagCount++){
                    TAG subTag = getNextTag(tagCount);
                    tag.subTAGs.insert(std::make_pair(subTag.name,subTag));
                }
            }
        }
        delete temp;
        delete closeTag;
        *tagPos = source.find("<" +tagName,*closeTagPos + 1); // get position of next tag
        delete closeTagPos;
        result.push_back(tag);
        * closeChar = source.find_first_of(">", * tagPos); 
    } 
    delete closeChar;
    delete tagPos;
    return result;
}