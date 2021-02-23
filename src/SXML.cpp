#include "SXML.h"

std::string SXML::XMLread::TAGvalue(std::string src){
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

SXML::attribTAG SXML::XMLread::attrib(std::string src){
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

std::string SXML::XMLread::TAGname(std::string src){
    WPTool::string_content tag_prop(src, " \t<>\"\'=");
    return tag_prop[0];
}

SXML::TAG SXML::XMLfile::getTAG(std::string tag_name, std::string prop, std::string val){
    TAG result;
    int * found = new int(get_tag_position("<"+tag_name,prop,val));
    std::string *temp = new std::string(source->get(*found));
    result.attrib = attrib(*temp);
    result.name = TAGname(*temp);
    result.value = TAGvalue(*temp);
    std::string * close_tag = new std::string("</" + result.name + ">"); 
    int * close_tag_pos = new int;
    *close_tag_pos = source->find(*close_tag,*found);
    if(*close_tag_pos != -1){
        for(int i = *found+1; i < *close_tag_pos; i++){
            TAG sub_tag = get_sub_tag(i);
            result.subTAGs.insert(std::make_pair(sub_tag.name,sub_tag));
        }
    }
    delete close_tag_pos;
    delete temp;
    delete close_tag;
    return result;
}

SXML::TAG SXML::XMLfile::get_sub_tag(int & start_pos){
    TAG result;
    std::string *temp = new std::string(source->get(start_pos));
    std::remove(temp->begin(), temp->end(), '\t'); 
    if(!temp->empty()){
        result.attrib = attrib(*temp);
        result.name = TAGname(*temp);
        result.value = TAGvalue(*temp);
        std::string * close_tag = new std::string("</" + result.name + ">"); 
        int * close_tag_pos = new int;
        *close_tag_pos = find(source->full_name(),*close_tag,start_pos,source->lines());
        if(*close_tag_pos != -1 && start_pos != *close_tag_pos){
            for(int i = start_pos+1; i < *close_tag_pos; i++){
                TAG sub_tag = get_sub_tag(i);
                result.subTAGs.insert(std::make_pair(sub_tag.name,sub_tag));
            }
            start_pos = *close_tag_pos;
        }
        delete close_tag_pos;
        delete close_tag;
    }
    delete temp;
    return result;
}

int SXML::XMLfile::get_tag_position(std::string tag_name, std::string prop, std::string val){
    int result = source->find(tag_name);
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

SXML::TAGArray SXML::XMLfile::select(std::string tagName){
    TAGArray result;
    int * tagPos; // position of tag in file 
    tagPos = new int(source->find("<" + tagName)); // get position 
    while(*tagPos != -1){
        TAG tag; // for collect information
        std::string *temp = new std::string(source->get(*tagPos)); // string with tag 
        tag.attrib = attrib(*temp);
        tag.name = TAGname(*temp);
        tag.value = TAGvalue(*temp);
        std::string * close_tag = new std::string("</" + tag.name + ">"); 
        int * close_tag_pos = new int;
        *close_tag_pos = source->find(*close_tag,*tagPos);
        if(*close_tag_pos != -1){
            for(int i = *tagPos+1; i < *close_tag_pos; i++){
                TAG sub_tag = get_sub_tag(i); // added sub tags
                tag.subTAGs.insert(std::make_pair(sub_tag.name,sub_tag)); 
            }
        }
        delete close_tag_pos;
        delete temp;
        delete close_tag;
        result.push_back(tag);
        *tagPos = source->find("<" +tagName,*tagPos + 1); // get position of next tag
    } 
    delete tagPos;
    return result;
}