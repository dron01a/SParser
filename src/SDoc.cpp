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


std::string sp::transform_tag(sp::tag & _tag){
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

std::string sp::format(std::string & source){
    size_t level = 1;
    for(size_t i = 0; i != source.size(); i++){
        switch (source[i]){
        case '>':
            source.insert(i+1,level,'\t');
            source.insert(i+1,1,'\n');
            level++;
            break;
        case '<':
            if(source[i+1] == '/'){
                level-=2;
                source.insert(i,level,'\t');
                source.insert(i,1,'\n');
                i+=level+1;
            }
            break;
        default:
            break;
        }
    }
}

sp::XML_tree sp::load_XML_from_file(std::string file_name){
    std::string source; 
#ifdef __FSTOOL__H__
    FSTool::file * source_file = open(file_name,FSTool::BINARY);
    source = file->buff();
    delete source_file; 
#else
    std::ifstream f(file_name);
    f.seekg(0, std::ios::end);
    size_t size = f.tellg();
    source.resize(size);
    f.seekg(0);
    f.read(&source[0], size);
#endif
    return sp::get_XML_tree(source);
}

sp::HTML_tree sp::load_HTML_from_file(std::string file_name){
    std::string source; 
#ifdef __FSTOOL__H__
    FSTool::file * source_file = open(file_name,FSTool::BINARY);
    source = file->buff();
    delete source_file; 
#else
    std::ifstream f(file_name);
    f.seekg(0, std::ios::end);
    size_t size = f.tellg();
    source.resize(size);
    f.seekg(0);
    f.read(&source[0], size);
#endif
    return sp::get_HTML_tree(source);
}

void sp::load_from_file(sp::small_free * tree,std::string file_name,md_type type){
    if(type == sp::HTML){
        tree = new sp::HTML_tree(sp::load_HTML_from_file(file_name));
    }
    else if(type == sp::XML){
        tree = new sp::XML_tree(sp::load_XML_from_file(file_name));
    }
    else{
        throw "error type";
    }
}

void sp::load_HTML_to_file(sp::HTML_tree & tree, std::string file_name){
#ifdef __FSTOOL__H__
    FSTool::file * source_file = open(file_name,FSTool::BINARY);
    if(!source_file->exists()){
        source_file->create();
    }
    if(!source_file->empty()){
        source_file->clear();
    }
    source_file->write(transform_HTML_to_string(tree),0);
    delete source_file;
#else
    std::ofstream f(file_name);
    f << transform_HTML_to_string(tree);
    f.close();
#endif
}

void sp::load_XML_to_file(sp::XML_tree & tree, std::string file_name){
#ifdef __FSTOOL__H__
    FSTool::file * source_file = open(file_name,FSTool::BINARY);
    if(!source_file->exists()){
        source_file->create();
    }
    if(!source_file->empty()){
        source_file->clear();
    }
    source_file->write(transform_XML_to_string(tree),0);
    delete source_file;
#else
    std::ofstream f(file_name);
    f << transform_XML_to_string(tree);
    f.close();
#endif
}
