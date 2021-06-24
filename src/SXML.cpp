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
            result = content(source, *first_char, *last_char + 1);
        }
        delete first_char;
        delete last_char;
    }
    return result;
}

// src -> string with XML text
SXML::attribTAG SXML::XMLreader::attrib(std::string src){
    SXML::attribTAG result;
    int * open_char = new int(src.find(" ", src.find("<"))); 
    int * close_char = new int(src.find(">"));
    if(*open_char != _NPOS || *close_char != _NPOS){
        std::string prop_str = content(src,*open_char, *close_char + 1); // string with prop
        int _start = 0;
        int _del = prop_str.find("=");
        while(_del != _NPOS){
            int _brS = prop_str.find("\"", _del);
            int _brE = prop_str.find("\"", _brS + 1);
            std::string _key = content(prop_str, _start - 1, _del);
            std::string _val = content(prop_str, _brS, _brE);
            result[_key] = _val;
            _start = src.find_first_not_of(" \t", _brE + 1) + 1;
            _del = prop_str.find("=", _del + 1);
        }    
    }
    delete open_char;
    delete close_char;
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

SXML::TAG SXML::XML::getTAG(std::string name, std::string prop, std::string val){
    TAG result;
    size_t *found = new size_t(findTAG(name,prop,val)); // tag position
    size_t *closeChar = new size_t(source.find_first_of(">", * found)); 
    size_t *closeTagPos = new size_t(source.find("</" + name + ">",*found + name.size()));
    std::string *temp = new std::string(content(source, *found - 1, *closeChar + 1));
    result.attrib = attrib(*temp); // get attrib
    result.name = TAGname(*temp);  // get name 
    if(*closeTagPos != _NPOS){
        std::string *tagContent = new std::string(content(source,*closeChar,*closeTagPos));
        if(getTokenType(*tagContent) == _text){
            result.text = *tagContent;
        }
        if(getTokenType(*tagContent) == _tag){
            for(int tagCount = * closeChar + 1; tagCount < *closeTagPos - 1; tagCount++){
                TAG subTag = getNextTag(tagCount);
                if(subTag.name.find("/") == _NPOS){
                    result.subTAGs.insert(std::make_pair(subTag.name,subTag));
                }
            }
        }
        delete tagContent;
    }
    delete closeChar;
    delete closeTagPos;
    delete temp;
    return result;
}

SXML::TAG SXML::XML::getNextTag(int & position){
    TAG result;
    size_t *openChar = new size_t(source.find("<", position));
    size_t *closeChar = new size_t(source.find(">", position)); 
    std::string *temp = new std::string(content(source, *openChar - 1, *closeChar + 1));
    if(!temp->empty()){
        result.attrib = attrib(*temp);
        result.name = TAGname(*temp);
        size_t *closeTagPos = new size_t(source.find("</" + result.name + ">", *closeChar));
        if(*closeTagPos != _NPOS){
            std::string *tagContent = new std::string(content(source,*closeChar,*closeTagPos));
            if(getTokenType(*tagContent) == _text){
                result.text = *tagContent;
            }
            if(getTokenType(*tagContent) == _tag){
                for(int tagCount = * closeChar + 1; tagCount < *closeTagPos - 1; tagCount++){
                    TAG subTag = getNextTag(tagCount);
                    if(subTag.name.find("/") == _NPOS){
                        result.subTAGs.insert(std::make_pair(subTag.name,subTag));
                    }
                }
            }
            delete tagContent;
            position = source.find_first_of(">", *closeTagPos + 1);
        }
        else{
            position = source.find_first_of(">", position + 1);
        }
        delete closeTagPos;
    }
    delete closeChar;
    delete temp;
    return result;
}


int SXML::XML::findTAG(std::string tag_name, std::string prop, std::string val){
    size_t result = source.find("<" + tag_name);
    size_t * closeChar = new size_t(source.find_first_of(">", result));
    while(TAGname(content(source, result, *closeChar)) != tag_name ){
        result = source.find("<" + tag_name, result + 1);
        *closeChar = source.find_first_of(">", result + 1);
    }
    std::string * temp = new std::string(content(source, result - 1, *closeChar + 3));
    attribTAG atrs = attrib(*temp);
    while(atrs[prop] != val){
        result = source.find("<" + tag_name, result + 1);
        *closeChar = source.find_first_of(">", result) + 1;
        while( TAGname(content(source, result, *closeChar)) != tag_name ){
            result = source.find("<" + tag_name, result + 1);
            *closeChar = source.find_first_of(">", result + 1);
        }

        *temp = content(source, result - 1, *closeChar + 3);
        atrs = attrib(*temp);
    }
    delete closeChar;
    delete temp; 
    return result;
}

SXML::TAGArray SXML::XML::select(std::string tagName){
    TAGArray result;
    int * tagPos = new int(source.find("<" + tagName)); // position of tag
    int * closeChar = new int(source.find(">", * tagPos)); 
    while(*tagPos != _NPOS){
        TAG tag; // for collect information
        std::string *temp = new std::string(content(source, *tagPos - 1, *closeChar + 1)); // string with tag 
        tag.attrib = attrib(*temp);
        tag.name = TAGname(*temp);
        int * closeTagPos = new int(source.find("</" + tag.name + ">",*tagPos));
        if(*closeTagPos != _NPOS){
            std::string tagContent = content(source, *closeChar, *closeTagPos - 2);
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
        * tagPos = source.find("<" + tagName, * tagPos + 1); // get position of next tag
        * closeChar = source.find(">", * tagPos); 
        delete temp;
        delete closeTagPos;
        result.push_back(tag);
    } 
    delete closeChar;
    delete tagPos;
    return result;
}

std::string SXML::content(std::string _src, int _posStart, int _posEnd){
    return _src.substr(_posStart + 1, _posEnd - _posStart - 1);
}

SXML::TAG SXML::XML::root(){
    TAG result; 
    size_t * firstIndex = new size_t(source.find("<?")); // positions with data of encoding and XML version
    size_t * secondIndex = new size_t(source.find("?>")); 
    size_t * start; // positions with TAG
    size_t * end; 
    std::string * temp; // twmp string 
    if( * firstIndex == _NPOS ){
        start = new size_t(source.find("<", *secondIndex + 1));
    }
    else if( * secondIndex == _NPOS ){
        start = new size_t(source.find("<", *firstIndex + 1));
    }
    else if ( * firstIndex == _NPOS && * secondIndex == _NPOS){
        start = new size_t(source.find("<"));
    }
    else{
        start = new size_t(source.find("<", *secondIndex + 1));
    }
    end = new size_t(source.find(">", *start));
    temp = new std::string(content(source, *start -1, *end));
    result = getTAG(TAGname(*temp));
    delete firstIndex;
    delete secondIndex;
    delete start;
    delete end;
    delete temp;
    return result;
}

int SXML::writeXML(std::string fileName,TAG root,std::string version, std::string enc){
    FSTool::file xmlFile(fileName); // file for write XML
    if(!xmlFile.exists()){
        throw 1;
    }
    if(!xmlFile.empty()){
        xmlFile.clear();
    }    
    xmlFile.add("<?xml version=\"" + version + "\" encoding=\"" + enc + "\"?>");
    xmlFile.add(convertTAG(root));
}

std::string SXML::convertTAG(TAG tag){
    std::string tagString = "<" + tag.name;
    if(tag.attrib.size() != 0){
        for(attribTAG::iterator it = tag.attrib.begin(); it != tag.attrib.end(); it++){
            tagString += " " + it->first + "=\"" + it->second +"\""; // add tag attrib`s to string
        }
        if((tag.subTAGs.size() == 0) && tag.text.empty()){
            tagString += "/";
        }
    }
    tagString += ">"; // add char to next line 
    if(tag.subTAGs.size() != 0){
        int lev = 1;
        tagString += "\n";
        for (TAGList::iterator itr = tag.subTAGs.begin();itr != tag.subTAGs.end(); itr++){
            tagString += convertTAG(lev,itr->second); // get string with child tag 
            lev = 1;
        }
        tagString += "</" + tag.name + ">\n";
    }
    else{
        tagString += tag.text; 
        tagString += "</" + tag.name + ">\n";
    }
    return tagString;
}

std::string SXML::convertTAG(int & lvl, TAG tag){
    std::string space; 
    for(int count = 0; count < lvl; count++){
        space += "\t";
    }
    std::string tagString = space + "<" + tag.name;
    if(tag.attrib.size() != 0){
        for(attribTAG::iterator it = tag.attrib.begin(); it != tag.attrib.end(); it++){
            tagString += " " + it->first + "=\"" + it->second +"\""; // add tag attrib`s to string
        }
        if(tag.subTAGs.size() == 0 && tag.text.empty()){
            tagString += "/";
        }
    }
    tagString += ">"; // add char to next line 
    if(tag.subTAGs.size() != 0){
        lvl++;
        tagString += "\n";
        for (TAGList::iterator itr = tag.subTAGs.begin();itr != tag.subTAGs.end(); itr++){
            tagString += convertTAG(lvl,itr->second); // get string with child tag 
        }
        tagString += space + "</" + tag.name + ">\n";
    }
    else{
        tagString += tag.text; 
        tagString += "</" + tag.name + ">\n";
    }
    return tagString;
}
