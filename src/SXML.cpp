#include "SXML.h"

SXML::XML* SXML::loadXML(std::string _rec, std::string mode){
    if(mode == "f"){
        return new SXML::XMLdocument(_rec);
    }
    if(mode == "s"){
        return new SXML::XMLstring(_rec);
    }
}

SXML::XML * SXML::loadXMLFile(std::string fileName){
    if(FSTool::exists(fileName)){
        return new SXML::XMLdocument(fileName);
    }
    return NULL;
}

SXML::XML* SXML::loadXMLStr(std::string text){
    return new SXML::XMLstring(text);
}

// src -> string with XML text
std::string SXML::XMLreader::TAGvalue(std::string src){
    std::string source = src;
    source.erase(std::remove(source.begin(), source.end(), '\t'), source.end());
    source.erase(std::remove(source.begin(), source.end(), '\n'), source.end());
    // result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    std::string result;
    if(source[source.length() - 1] != '/'){
        int * first_char = new int(source.find_first_of(">")); 
        int * last_char = new int(source.find_first_of("<", * first_char));
        if((*first_char != std::string::npos) && (*last_char != std::string::npos)){
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

SXML::XMLdocument::XMLdocument(std::string source){
    this->source = new FSTool::file(source);
}

SXML::XMLdocument::~XMLdocument(){
    delete source;
}

SXML::TAG SXML::XMLdocument::getTAG(std::string tag_name, std::string prop, std::string val){
    TAG result;
    int * found = new int(findTAG("<"+tag_name,prop,val));
    std::string *temp = new std::string(source->get(*found));
    result.attrib = attrib(*temp);
    result.name = TAGname(*temp);
    result.value = TAGvalue(*temp);
    std::string * closeTag = new std::string("</" + result.name + ">"); 
    int * closeTagPos = new int;
    *closeTagPos = source->find(*closeTag,*found);
    if(*closeTagPos != -1){
        for(int i = *found+1; i < *closeTagPos; i++){
            TAG subTag = get_sub_tag(i);
            result.subTAGs.insert(std::make_pair(subTag.name,subTag));
        }
    }
    delete closeTagPos;
    delete temp;
    delete closeTag;
    return result;
}

SXML::TAG SXML::XMLdocument::get_sub_tag(int & start_pos){
    TAG result;
    std::string *temp = new std::string(source->get(start_pos));
    std::remove(temp->begin(), temp->end(), '\t'); 
    if(!temp->empty()){
        result.attrib = attrib(*temp);
        result.name = TAGname(*temp);
        result.value = TAGvalue(*temp);
        std::string * closeTag = new std::string("</" + result.name + ">"); 
        int * closeTagPos = new int;
        *closeTagPos = FSTool::find(source->full_name(),*closeTag,start_pos,source->lines());
        if(*closeTagPos != -1 && start_pos != *closeTagPos){
            for(int i = start_pos+1; i < *closeTagPos; i++){
                TAG subTag = get_sub_tag(i);
                result.subTAGs.insert(std::make_pair(subTag.name,subTag));
            }
            start_pos = *closeTagPos;
        }
        delete closeTagPos;
        delete closeTag;
    }
    delete temp;
    return result;
}

SXML::TAGArray SXML::XMLdocument::select(std::string tagName){
    TAGArray result;
    int * tagPos; // position of tag in file 
    tagPos = new int(source->find("<" + tagName)); // get position 
    while(*tagPos != -1){
        TAG tag; // for collect information
        std::string *temp = new std::string(source->get(*tagPos)); // string with tag 
        tag.attrib = attrib(*temp);
        tag.name = TAGname(*temp);
        std::string * closeTag = new std::string("</" + tag.name + ">"); 
        int * closeTagPos = new int;
        *closeTagPos = source->find(*closeTag,*tagPos);
        tag.value = TAGvalue(*temp);
        if(*closeTagPos != -1){
            for(int i = *tagPos+1; i < *closeTagPos; i++){
                TAG subTag = get_sub_tag(i); // added sub tags
                tag.subTAGs.insert(std::make_pair(subTag.name,subTag)); 
            }
        }
        delete closeTagPos;
        delete temp;
        delete closeTag;
        result.push_back(tag);
        *tagPos = source->find("<" +tagName,*tagPos + 1); // get position of next tag
    } 
    delete tagPos;
    return result;
}

int SXML::XMLdocument::findTAG(std::string tag_name, std::string prop, std::string val){
    int result = source->find(tag_name);
    while( TAGname(source->get(result)) != tag_name ){
        result = source->find(tag_name);
    }
    std::string * temp = new std::string;
    *temp = source->get(result);
    WPTool::string_content * str;
    str = new WPTool::string_content(*temp," \t\"\'=");
    int * p_pos = new int(str->find(prop));
    int * v_pos = new int(str->find(val, * p_pos));
    int * l_pos = new int(*v_pos - *p_pos);
    while(*l_pos != 1){  
        result = source->find(tag_name,result + 1);
        if(result == -1){
            break;
        }
        else{
            *temp = source->get(result);
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

SXML::XMLstring::XMLstring(std::string source){
    this->source = new std::string(source);
}

SXML::XMLstring::~XMLstring(){
    delete source;
}

SXML::TAG SXML::XMLstring::getTAG(std::string tag_name, std::string prop, std::string val){
    TAG result;
    int * found = new int(findTAG(tag_name,prop,val));
    int * closeChar = new int(source->find_first_of(">", * found)); 
    std::string * closeTag = new std::string("</" + tag_name + ">"); 
    int * closeTagPos = new int;
    *closeTagPos = source->find(*closeTag,*found + tag_name.size());
    std::string *temp = new std::string(source->substr(*found, *closeTagPos - *found + closeTag->size()));
    result.attrib = attrib(*temp);
    result.name = TAGname(*temp);
    result.value = TAGvalue(*temp);
    if(*closeTagPos != std::string::npos){
        std::string tagContent = temp->substr(tag_name.size() + 2 , * closeTagPos - *closeChar - 1);
        if(tagContent.find_first_of("<>") != std::string::npos){
            for(int tagCount = * closeChar + 1; tagCount < *closeTagPos; tagCount++){
                TAG subTag = get_sub_tag(tagCount);
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

SXML::TAG SXML::XMLstring::get_sub_tag(int & start_pos){
    TAG result;
    int * openChar = new int(source->find_first_of("<", start_pos));
    int * closeChar = new int(source->find_first_of(">", start_pos)); 
    std::string *temp = new std::string(source->substr(start_pos,*closeChar - start_pos + 1 ));
    if(!temp->empty()){
        result.attrib = attrib(*temp);
        result.name = TAGname(*temp);
        result.value = TAGvalue(*temp);
        std::string * closeTag = new std::string("</" + result.name + ">"); 
        int * closeTagPos = new int;
        *closeTagPos = source->find(*closeTag, *closeChar);
        if(*closeTagPos != std::string::npos){
            for(int tagCount = *closeChar + closeTag->size() - 1; tagCount < *closeTagPos; tagCount++){
                TAG subTag = get_sub_tag(tagCount);
                result.subTAGs.insert(std::make_pair(subTag.name,subTag));
            }
            start_pos = *closeTagPos;
        }
        else{
            start_pos = source->find_first_of(">", start_pos);
        }
        delete closeTagPos;
        delete closeTag;
    }
    delete closeChar;
    delete temp;
    return result;
}

SXML::TAGArray SXML::XMLstring::select(std::string tagName){
    TAGArray result;
    int * tagPos; // position of tag in file 
    tagPos = new int(source->find("<" + tagName)); // get position 
    int * closeChar = new int(source->find_first_of(">", * tagPos)); 
    while(*tagPos != std::string::npos){
        TAG tag; // for collect information
        std::string *temp = new std::string(source->substr(*tagPos, *closeChar - *tagPos  + 1 )); // string with tag 
        tag.attrib = attrib(*temp);
        tag.name = TAGname(*temp);
        tag.value = TAGvalue(*temp);
        std::string * closeTag = new std::string("</" + tag.name + ">"); 
        int * closeTagPos = new int;
        *closeTagPos = source->find(*closeTag,*tagPos);
        if(*closeTagPos != std::string::npos){
            for(int i = source->find_first_of("<", *tagPos + 1); i < *closeTagPos; i++){
                TAG subTag = get_sub_tag(i); // added sub tags
                tag.subTAGs.insert(std::make_pair(subTag.name,subTag)); 
            }
        }
        delete temp;
        delete closeTag;
        *tagPos = source->find("<" +tagName,*closeTagPos + 1); // get position of next tag
        delete closeTagPos;
        result.push_back(tag);
        * closeChar = source->find_first_of(">", * tagPos); 
    } 
    delete closeChar;
    delete tagPos;
    return result;
}

int SXML::XMLstring::findTAG(std::string tag_name, std::string prop, std::string val){
    int result = source->find("<" + tag_name);
    int * closeChar = new int(source->find_first_of(">", result));
    while( TAGname(source->substr(result,*closeChar - result + 1 )) != tag_name ){
        result = source->find("<" + tag_name, result + 1);
    }
    std::string * temp = new std::string;
    *temp = source->substr(result,*closeChar - result + 1 );
    WPTool::string_content * str;
    str = new WPTool::string_content(*temp," \n\t\"\'=");
    int * p_pos = new int(str->find(prop));
    int * v_pos = new int(str->find(val, * p_pos));
    int * l_pos = new int(*v_pos - *p_pos);
    if(prop == "" && val == "" ){
        * l_pos = 1;
    }
    while(*l_pos != 1){  
        result = source->find(tag_name,result + 1);
        if(result == std::string::npos){
            break;
        }
        else{
            *temp = source->substr(result,*closeChar - result + 1 );
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