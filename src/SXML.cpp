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
std::string SXML::XML::TAGtext(std::string src){
    std::string source = src;
    std::string result;
    if(source[source.length() - 1] != '/'){
        int * first_char = new int(source.find_first_of(">")); 
        int * last_char = new int(source.find_first_of("<", * first_char));
        if((*first_char != _NPOS) && (*last_char != _NPOS)){
            result = content(source, *first_char, *last_char);
        }
        delete first_char;
        delete last_char;
    }
    return result;
}

// src -> string with XML text
SXML::attribTAG SXML::XML::attrib(std::string src){
    SXML::attribTAG result;
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

// src -> string with XML text
std::string SXML::XML::TAGname(std::string src){
    WPTool::string_content tag_prop(src, " \n\t<>\"\'=");
    return tag_prop[0];
}

SXML::_tokenType SXML::getTokenType(std::string token){
    int * open_char = new int(token.find_first_of("<")); 
    int * close_char = new int(token.find_first_of(">", *open_char));
    if(*open_char == _NPOS || *close_char == _NPOS){    
        return _text;
    }
    if(*open_char != _NPOS && *close_char != _NPOS && token[*close_char - 1] == '/'){
        return _closeTag;
    }
    return _tag;
}

SXML::XML::XML(std::string source){
    this->source = source;
}

SXML::TAG SXML::XML::getNextTag(int & position){
    TAG result;
    size_t *openChar = new size_t(source.find("<", position));
    size_t *closeChar = new size_t(source.find(">", position)); 
    parceTagAttrs(result, *openChar, *closeChar);
    if(result.type != _closeTag){
        size_t *closeTagPos = new size_t(source.find("</" + result.name + ">", *closeChar));
        if(*closeTagPos != _NPOS){
            parceTagContent(result,*closeChar, *closeTagPos);
            position = source.find_first_of(">", *closeTagPos + 1);
        }
        else{
            position = source.find_first_of(">", position + 1);
        }
        delete closeTagPos;
    }
    else{
        position = source.find_first_of(">", position + 1);
    }
    delete closeChar;
    return result;
}

SXML::TAG SXML::XML::getTAG(std::string tagName, attribTAG _attrib){
    TAG result; 
    size_t *found = new size_t(findTAG(tagName,_attrib)); // tag position
    size_t *closeChar = new size_t(source.find_first_of(">", *found)); 
    parceTagAttrs(result, *found, *closeChar); 
    if(result.type != _closeTag){
        size_t *closeTagPos = new size_t(source.find("</" + tagName + ">", *closeChar));
        if(*closeTagPos != _NPOS){ 
            parceTagContent(result,*closeChar, *closeTagPos - 1);
        }
        delete closeTagPos;
    }
    delete closeChar;
    return result;
}

int SXML::XML::findTAG(std::string tagName, attribTAG _attrib){
    size_t result = 0;
    size_t closeChar;
    attribTAG atrs;
    bool _attrPassed = false;
    while(!_attrPassed){
        setRange(tagName, result, closeChar);
        if(result == _NPOS || closeChar == _NPOS){
            throw "tag not found";
        }
        atrs = attrib(content(source, result, closeChar));
        _attrPassed = compareTagAttrs(atrs, _attrib);
    }
    return result;
}

SXML::TAGArray SXML::XML::select(std::string tagName){
    TAGArray result;
    int * tagPos = new int(source.find("<" + tagName)); // position of tag
    int * closeChar = new int(source.find(">", * tagPos)); 
    for(int count; *tagPos != _NPOS; count++){
        TAG tag; // for collect information
        size_t * closeTagPos = new size_t(source.find("</" + tag.name + ">",*tagPos));
        parceTagAttrs(result[count], *tagPos , *closeChar); // string with tag 
        if(result[count].type != _closeTag){
            if(*closeTagPos != _NPOS){
                parceTagContent(result[count],*closeChar, *closeTagPos);
            }
        }
        *tagPos = source.find("<" + tagName, * tagPos + 1); // get position of next tag
        *closeChar = source.find(">", * tagPos); 
        delete closeTagPos;
        result.push_back(tag);
    } 
    delete closeChar;
    delete tagPos;
    return result;
}

int SXML::XML::count(std::string tagName, attribTAG _attrib){
    int result = 0;       // count 
    size_t _beginPos = 0; // start position 
    size_t _endPos = 0;   // end position
    attribTAG atrs;
    bool _attrPassed = false;
    while(_beginPos != _NPOS && _endPos != _NPOS){
        setRange(tagName, _beginPos, _endPos);
        if(_beginPos == _NPOS || _endPos == _NPOS){
            break;
        }
        atrs = attrib(content(source, _beginPos, _endPos));
        _attrPassed = compareTagAttrs(atrs, _attrib);
        if(_attrPassed){
            result++;
        }
    }
    return result;
}

bool SXML::XML::compareTagAttrs(attribTAG _attrsA, attribTAG _attrsB){
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

void SXML::XML::setRange(std::string _name, size_t & _begin, size_t & _end){
    _begin = source.find("<" + _name, (_begin == 0) ? 0 : _begin + 1);
    _end = source.find_first_of(">", _begin);
    while(_begin != _NPOS && _end != _NPOS){
        if((TAGname(content(source, _begin, _end)) == _name)){
            break;
        }
        _begin = source.find("<" + _name, _begin + 1);
        _end = source.find_first_of(">", _begin);
    }
}

void SXML::XML::parceTagAttrs(TAG & _tag, int _start, int _end){
    std::string *temp = new std::string(content(source, _start, _end));
    _tag.attrib = attrib(*temp);     // set attrib  
    _tag.name = TAGname(*temp);      // set name
    _tag.type = getTokenType(*temp); // set type
    delete temp;
}

void SXML::XML::parceTagContent(TAG & __tag, int _start, int _end){
    std::string *tagContent = new std::string(content(source, _start + 1, _end - 1));
    if(getTokenType(*tagContent) == _text){
        __tag.text = *tagContent; // set tag text
    }
    else{
        for(int tagCount = _start + 1; tagCount < _end - 1; tagCount++){
            TAG subTag = getNextTag(tagCount); 
            if(subTag.name.find("/") == _NPOS){
                __tag.subTAGs.insert(std::make_pair(subTag.name,subTag)); // add sub tags 
            }
        }
    }
    delete tagContent;
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
    temp = new std::string(content(source, *start, *end));
    result = getTAG(TAGname(*temp));
    delete firstIndex;
    delete secondIndex;
    delete start;
    delete end;
    delete temp;
    return result;
}

std::string SXML::XML::encoding(){
    size_t firstIndex = source.find("<?"); // positions with data of encoding and XML version
    size_t secondIndex = source.find("?>");
    if( firstIndex == _NPOS || secondIndex == _NPOS ){
        return S_ENC;
    }
    WPTool::string_content info(content(source,firstIndex + 1, secondIndex - 1), " \t\n=");
    return info[info.find("encoding") + 1];
}

std::string SXML::XML::version(){
    size_t firstIndex = source.find("<?"); // positions with data of encoding and XML version
    size_t secondIndex = source.find("?>");
    if( firstIndex == _NPOS || secondIndex == _NPOS ){
        return S_VER;
    }
    WPTool::string_content info(content(source,firstIndex + 1, secondIndex - 1), " \t\n=");
    return info[info.find("version") + 1];
}

SXML::XMLfree::XMLfree(std::string _src){
    XML parcer(_src);
    this->root = parcer.root();
    this->enc = parcer.encoding();
    this->ver = parcer.version();
}

SXML::XMLfree::XMLfree(XML _XML){
    this->root = _XML.root();
    this->enc = _XML.encoding();
    this->ver = _XML.version();
}

std::string SXML::content(std::string _src, int _posStart, int _posEnd){
    return _src.substr(_posStart, _posEnd - _posStart + 1);
}

int SXML::writeXML(std::string fileName,TAG root,std::string version, std::string enc){
    FSTool::file xmlFile(fileName); // file for write XML
    if(!xmlFile.exists()){
        return 1;
    }
    if(!xmlFile.empty()){
        xmlFile.clear();
    }    
    xmlFile.add("<?xml version=\"" + version + "\" encoding=\"" + enc + "\"?>");
    xmlFile.add(convertTAG(root));
    return 0;
}

std::string SXML::convertTAG(TAG tag){
    std::string tagString = "<" + tag.name;
    tagString += convertTagAttr(tag);
    tagString += ">"; // add char to next line 
    if(tag.type != _closeTag){
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
    }
    else{
        tagString +="\n";
    }
    return tagString;
}

std::string SXML::convertTAG(int & lvl, TAG tag){
    std::string space; 
    std::string tagString;
    for(int count = 0; count < lvl; count++){
        space += "\t";
    }
    tagString = space + "<" + tag.name;
    tagString += convertTagAttr(tag);
    tagString += ">"; // add char to next line 
    if(tag.type != _closeTag){
        if(tag.subTAGs.size() != 0){
            lvl++;
            tagString += "\n";
            for (TAGList::iterator itr = tag.subTAGs.begin();itr != tag.subTAGs.end(); itr++){
                tagString += convertTAG(lvl,itr->second); // get string with child tag 
            }
            lvl--;
            tagString += space + "</" + tag.name + ">\n";
        }
        else{
            tagString += tag.text; 
            tagString += "</" + tag.name + ">\n";
        }
    }
    else{
        tagString +="\n";
    }
    return tagString;
}

std::string SXML::convertTagAttr(TAG _tag){
    std::string result;
    if(_tag.attrib.size() != 0){
        for(attribTAG::iterator it = _tag.attrib.begin(); it != _tag.attrib.end(); it++){
            result += " " + it->first + "=\"" + it->second +"\""; // add tag attrib`s to string
        }
        if(_tag.subTAGs.size() == 0 && _tag.text.empty()){
            result += "/";
        }
    }
    return result;
}