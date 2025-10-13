#include "spxml.h"

bool is_digit(sp::string_t str) {
	return str.find_first_not_of(_t("0123456789,.+-e")) != sp::string_t::npos;
}

bool is_bool(sp::string_t str) {
	return str == _t("true") || str == _t("false");
}

sp::reader::reader(const sp::char_t * data){
	init_tables();
}

sp::reader::reader(const sp::string_t & data){
	init_tables();
}

sp::reader::reader(const sp::input_stream & data){
	init_tables();
}

sp::char_type sp::reader::get_char_type(sp::char_t ch){
	if (char_table.count(ch)) {
		return char_table[ch];
	}
	else if (std::isalpha(ch) || ch == _t('_')) {
		return sp::char_type::simlpe_char;
	}
	else if (std::isdigit(ch)) {
		return sp::char_type::digit;
	}
	return sp::char_type::unknown;
}

sp::token sp::reader::get_next_token(){
	if (cur_char.data == 0) {
		cur_char = get_next_char();
	}
	last_token.data.clear();
	switch (cur_char.type){
	case sp::char_type::close_brt:
		close_brt_proc();
		break;
	case sp::char_type::open_brt:
		open_brt_proc();
		break;
	case sp::char_type::end:
		last_token.type = sp::token_type::end_of_data;
		break;
	case sp::char_type::space:
		attrib_name_proc();
		break;
	case sp::char_type::eval:
		attrib_val_proc();
		break;
	case sp::char_type::slash:
		cur_char = get_next_char();
		if (cur_char.type != sp::char_type::close_brt) {
			throw sp::error_type::autoclose_tag_error;
		}
		break;
	}
	return last_token;
}

sp::token & sp::reader::get_last_token(){
	return last_token;
}

void sp::reader::init_tables(){
	char_table.insert({ _t(';'), sp::char_type::semicolon });
	char_table.insert({ _t('&'), sp::char_type::ampers });
	char_table.insert({ _t('?'), sp::char_type::question_mark });
	char_table.insert({ _t('!'), sp::char_type::exclam_mark});
	char_table.insert({ _t('<'), sp::char_type::open_brt });
	char_table.insert({ _t('>'), sp::char_type::close_brt });
	char_table.insert({ _t(' '), sp::char_type::space });
	char_table.insert({ _t('\t'), sp::char_type::space });
	char_table.insert({ _t('\n'), sp::char_type::next_line });
	char_table.insert({ _t('\r'), sp::char_type::next_line });
	char_table.insert({ _t('\"'), sp::char_type::quot});
	char_table.insert({ _t('\''), sp::char_type::apos });
	char_table.insert({ _t('/'), sp::char_type::slash });
	char_table.insert({ _t('='), sp::char_type::eval });
	char_table.insert({ _t('\0'), sp::char_type::end });
	ent_table.insert({ _t("&lt"), _t('<') });
	ent_table.insert({ _t("&gt"), _t('>') });
	ent_table.insert({ _t("&amp"), _t('&') });
	ent_table.insert({ _t("&apos"), _t('\'') });
	ent_table.insert({ _t("&quot"), _t('\"') });
}

void sp::reader::open_brt_proc(){
	cur_char = get_next_char(); // получаем следующий сивол 
	switch (cur_char.type){
	case sp::char_type::simlpe_char:
		name_proc();
		last_token.type = sp::token_type::tag_name;
		break;
	case sp::char_type::slash:
		cur_char = get_next_char();
		name_proc();
		last_token.type = sp::token_type::tag_end;
		break;
	case sp::char_type::exclam_mark:
		comment_proc();
		break;
	default:
		throw sp::error_type::uncorrect_char_after_open_brt;
	}
}

void sp::reader::close_brt_proc(){
	cur_char = get_next_char(); // получаем следующий сивол 
	skip_chars({ sp::char_type::space, sp::char_type::next_line });
	if (cur_char.type == sp::char_type::open_brt || cur_char.type == sp::char_type::end) {
		last_token = get_next_token();
	}
	else {
		while (cur_char.type != sp::char_type::open_brt) {
			switch (cur_char.type){
			case sp::char_type::ampers:
				ent_proc();
				break;
			case sp::char_type::end:
				throw sp::error_type::text_end_error;
			default:
				last_token.data += cur_char.data;
			}
			cur_char = get_next_char(); // получаем следующий сивол 
		}
		last_token.type = sp::token_type::tag_text;
	}
}

void sp::reader::name_proc(){
	while (cur_char.type != sp::char_type::close_brt && cur_char.type != sp::char_type::space) {
		switch (cur_char.type){
		case sp::char_type::digit:
		case sp::char_type::simlpe_char:
			last_token.data += cur_char.data; // добавляем символ к имени тега
			break;
		default:
			throw sp::error_type::error_tag_name; // ошибка в имени тега
		}
		cur_char = get_next_char(); // получаем следующий сивол 
	}
}

void sp::reader::comment_proc(){
	cur_char = get_next_char(); // получаем следующий сивол 
	if (cur_char.data == _t('-')) {
		cur_char = get_next_char(); // получаем следующий сивол 
		if (cur_char.data == _t('-')) {
			while(cur_char.type != sp::char_type::end){
				if (cur_char.data == _t('-')) {
					cur_char = get_next_char(); // получаем следующий сивол 
					if (cur_char.data == _t('-')) {
						cur_char = get_next_char(); // получаем следующий сивол 
						if (cur_char.type == sp::char_type::close_brt) {
							return;
						}
						break;
					}
				}
				cur_char = get_next_char(); // получаем следующий сивол 
			}
		}
	}
	throw sp::error_type::comment_error;
}

void sp::reader::attrib_name_proc(){
	skip_chars({ sp::char_type::space });
	if (cur_char.type != sp::char_type::simlpe_char) {
		last_token = get_next_token();
		return;
	}
	while (cur_char.type != sp::char_type::eval && cur_char.type != sp::char_type::space) {
		switch (cur_char.type){
		case sp::char_type::simlpe_char:
		case sp::char_type::digit:
			last_token.data += cur_char.data;
			break;
		case sp::char_type::end:
			throw sp::error_type::attrib_name_wrong;
		default:
			throw sp::error_type::atribute_name_error;
		}
		cur_char = get_next_char(); // получаем следующий сивол 
	}
	skip_chars({ sp::char_type::space });
	last_token.type = sp::token_type::atribute_name;
}

void sp::reader::attrib_val_proc(){
	cur_char = get_next_char(); // получаем следующий сивол 
	skip_chars({ sp::char_type::space });
	if (cur_char.type != sp::char_type::quot && cur_char.type != sp::char_type::apos) {
		throw sp::error_type::attribute_value_exec_open_quot;
	}
	sp::char_type quot_t = cur_char.type;
	cur_char = get_next_char(); // получаем следующий сивол 
	while (cur_char.type != quot_t) {
		switch (cur_char.type) {
		case sp::char_type::quot:
		case sp::char_type::apos:
			throw sp::error_type::attribute_value_error;
		case sp::char_type::ampers:
			ent_proc();
			break;
		case sp::char_type::end:
			throw sp::error_type::attribute_value_exec_close_quot;
		default:
			last_token.data += cur_char.data;
		}
		cur_char = get_next_char(); // получаем следующий сивол 
	} 
	cur_char = get_next_char(); // получаем следующий сивол
	last_token.type = sp::token_type::atribute_value;
}

void sp::reader::ent_proc(){
	sp::string_t temp; // строка в с сущностью
	while (cur_char.type != sp::char_type::semicolon) {
		temp += cur_char.data;
		cur_char = get_next_char(); // получаем следующий сивол
	}
	if (ent_table.count(temp) == 0) {
		throw sp::error_type::unknown_ent;
	}
	last_token.data += ent_table[temp];
}

void sp::reader::skip_chars(std::vector<sp::char_type> vect){
	while (std::count(vect.begin(), vect.end(), cur_char.type) != 0) {
		cur_char = get_next_char(); // получаем следующий сивол 
	}
}

sp::string_reader::string_reader(const sp::char_t * data) : reader(data) {
	this->data = data;
}

sp::string_reader::string_reader(const sp::string_t & data) : reader(data) {
	this->data = data;
}

sp::xml_char sp::string_reader::get_next_char(){
	sp::char_t c = 0;
	if (position < data.size()) {
		c = data[position++];
	}
	else {
		c = '\0';
	}
	return{ c, get_char_type(c) };
}

bool sp::string_reader::good(){
	return !data.empty();
}

sp::file_reader::file_reader(const sp::char_t * file_name) : reader(file_name){
	file.open(file_name, std::ios::binary | std::ios::in );
}

sp::file_reader::file_reader(const sp::string_t & file_name) : reader(file_name){
	file.open(file_name, std::ios::binary | std::ios::in);
}

sp::file_reader::~file_reader(){
	file.close();
}

sp::xml_char sp::file_reader::get_next_char(){
	sp::char_t c = file.get();
	if (file.eof()) {
		c = '\0';
	}
	return{ c, get_char_type(c) };
}

bool sp::file_reader::good(){
	return file.good();
}

sp::stream_reader::stream_reader(sp::input_stream & data) : reader(data) {
	stream = &data;
}

sp::stream_reader::~stream_reader(){
	stream = nullptr;
	delete stream;
}

sp::xml_char sp::stream_reader::get_next_char(){
	sp::char_t c = stream->get();
	if (stream->eof()) {
		c = '\0';
	}
	return{ c, get_char_type(c) };
}

bool sp::stream_reader::good(){
	return stream->good();
}

sp::value::value(){ }

sp::value::value(const sp::char_t * data){
	set(data);
}

sp::value::value(const sp::string_t & data){
	set(data);
}

sp::value::value(sp::char_t data){
	set(data);
}

sp::value::value(int data){
	set(data);
}

sp::value::value(double data){
	set(data);
}

sp::value::value(bool data){
	set(data);
}

sp::value_type sp::value::type(){
	return _type;
}

sp::value_type sp::value::type() const {
	return _type;
}

sp::string_t sp::value::to_string() const {
	return data;
}

int sp::value::to_int() const {
	if (_type == sp::value_type::_double) {
		return std::stoi(data);
	}
	throw sp::error_type::error_value_type;
}

double sp::value::to_double() const {
	if (_type == sp::value_type::_double) {
		return std::stod(data);
	}
	throw sp::error_type::error_value_type;
}

bool sp::value::to_bool() const {
	return data[0] != '0';
}

void sp::value::set(const sp::char_t * data){
	if (is_digit(data)) {
		_type = sp::value_type::_double;
	}
	if (is_bool(data)) {
		_type = sp::value_type::_bool;
	}
	this->data = data;
}

void sp::value::set(const sp::string_t & data) {
	if (is_digit(data)) {
		_type = sp::value_type::_double;
	}
	this->data = data;
}

void sp::value::set(sp::char_t data){
	this->data = sp::string_t(data, 1);
}

void sp::value::set(int data){
	_type = sp::value_type::_double;
#ifdef  SPW_MODE
	this->data = std::to_wstring(data);
#else
	this->data = std::to_string(data);
#endif 
}

void sp::value::set(double data){
	_type = sp::value_type::_double;
#ifdef  SPW_MODE
	this->data = std::to_wstring(data);
#else
	this->data = std::to_string(data);
#endif 
}

void sp::value::set(bool data) {
	_type = sp::value_type::_bool;
#ifdef  SPW_MODE
	this->data = std::to_wstring(data);
#else
	this->data = std::to_string(data);
#endif 
}

bool sp::value::operator==(const sp::value & val) const{
	if (_type != val.type()) {
		return false;
	}
	switch (_type){
	case sp::value_type::_bool:
		return this->to_bool() == val.to_bool();
	case sp::value_type::_double:
		return this->to_double() == val.to_double();
	case sp::value_type::_string:
		return this->to_string() == val.to_string();
	}
	return false;
}

bool sp::value::operator!=(const sp::value & val) const{
	return !(*this == val);
}

bool sp::value::operator>(const sp::value & val) const{
	if (_type != val.type()) {
		return false;
	}
	switch (_type) {
	case sp::value_type::_bool:
		return this->to_bool() > val.to_bool();
	case sp::value_type::_double:
		return this->to_double() > val.to_double();
	}
	return false;
}

bool sp::value::operator>=(const sp::value & val) const{
	if (_type != val.type()) {
		return false;
	}
	switch (_type) {
	case sp::value_type::_bool:
		return this->to_bool() >= val.to_bool();
	case sp::value_type::_double:
		return this->to_double() >= val.to_double();
	}
	return false;
}

bool sp::value::operator<(const sp::value & val) const{
	return !(*this >= val);
}

bool sp::value::operator<=(const sp::value & val) const{
	return !(*this > val);
}

sp::value & sp::value::operator=(const sp::char_t * data){
	set(data);
	return *this;
}

sp::value & sp::value::operator=(const sp::string_t & data) {
	set(data);
	return *this;
}

sp::value & sp::value::operator=(sp::char_t data) {
	set(data);
	return *this;
}

sp::value & sp::value::operator=(int data) {
	set(data);
	return *this;
}

sp::value & sp::value::operator=(double data) {
	set(data);
	return *this;
}

sp::value & sp::value::operator=(bool data) {
	set(data);
	return *this;
}

sp::attribute::attribute(){}

sp::attribute::attribute(const sp::string_t & name, sp::value val){ 
	this->_name = name;
	this->_value = val;
}

sp::attribute::attribute(const sp::char_t * name, sp::value val){
	this->_name = name;
	this->_value = val;
}

sp::string_t sp::attribute::name() const{
	return _name;
}

void sp::attribute::name(const sp::string_t & str){
	_name = str;
}

void sp::attribute::name(const sp::char_t * str){
	_name = str;
}

sp::value sp::attribute::value() const{
	return _value;
}

void sp::attribute::value(sp::value val){
	_value = val;
}

bool sp::attribute::operator==(const sp::attribute & attrib) const{
	return this->_value == attrib._value && this->_name == attrib._name;
}

bool sp::attribute::operator!=(const sp::attribute & attrib) const{
	return this->_value != attrib._value && this->_name != attrib._name;
}

bool sp::attribute::operator>(const sp::attribute & attrib) const{
	return this->_value > attrib._value && this->_name == attrib._name;
}

bool sp::attribute::operator>=(const sp::attribute & attrib) const{
	return this->_value >= attrib._value && this->_name != attrib._name;
}

bool sp::attribute::operator<(const sp::attribute & attrib) const {
	return this->_value < attrib._value && this->_name == attrib._name;
}

bool sp::attribute::operator<=(const sp::attribute & attrib) const {
	return this->_value <= attrib._value && this->_name != attrib._name;
}

sp::attribute_table::attribute_table(){}

sp::attribute_table::attribute_table(std::initializer_list<attribute> list){
	for (std::initializer_list<attribute>::iterator it = list.begin(); it != list.end(); it++) {
		table.insert({ it->name(), *it });
	}
}

sp::attribute & sp::attribute_table::operator[](const sp::char_t * name){
	return table[name];
}

sp::attribute & sp::attribute_table::operator[](const sp::string_t & name) {
	return table[name];
}

sp::attribute sp::attribute_table::operator[](const sp::char_t * name) const {
	return const_cast<sp::attribute_table *>(this)->operator[](name);
}

sp::attribute sp::attribute_table::operator[](const sp::string_t & name) const {
	return const_cast<sp::attribute_table *>(this)->operator[](name);
}

void sp::attribute_table::add(const sp::char_t * name, sp::value val){
	if (check(name)) {
		throw sp::error_type::atribute_exist;
	}
	table.insert({ name, sp::attribute(name, val) });
}

void sp::attribute_table::add(const sp::string_t & name, sp::value val){
	this->add(name.c_str(), val);
}

void sp::attribute_table::add(attribute & attribute){
	if (check(attribute.name())) {
		throw sp::error_type::atribute_exist;
	}
	table.insert({ attribute.name(), attribute });
}

void sp::attribute_table::remove(const sp::char_t * name){
	if (check(name)) {
		throw sp::error_type::atribute_not_exist;
	}
	table.erase(name);
}

void sp::attribute_table::remove(const sp::string_t & name){
	this->remove(name.c_str());
}

bool sp::attribute_table::check(const sp::char_t * name){
	return table.count(name) != 0;
}

bool sp::attribute_table::check(const sp::string_t & name) {
	return table.count(name) != 0;
}

bool sp::attribute_table::check(const sp::char_t * name, sp::value val){
	if(this->check(name)){
		return table[name].value() == val;
	}
	return false;
}

bool sp::attribute_table::check(const sp::string_t & name, sp::value val){
	return this->check(name.c_str(), val);
}

bool sp::attribute_table::check(attribute & attribute){
	if (this->check(attribute.name())) {
		return table[attribute.name()] == attribute;
	}
	return false;
}

size_t sp::attribute_table::size(){
	return table.size();
}

sp::const_attr_iterator sp::attribute_table::begin() const{
	return table.begin();
}

sp::attr_iterator sp::attribute_table::begin(){
	return table.begin();
}

sp::const_attr_iterator sp::attribute_table::end() const{
	return table.end();
}

sp::attr_iterator sp::attribute_table::end(){
	return table.end();
}

bool sp::attribute_table::operator==(const sp::attribute_table & attrib_table){
	return table == attrib_table.table;
}

bool sp::attribute_table::operator!=(const sp::attribute_table & attrib_table){
	return !(*this == attrib_table);
}

sp::tag::tag() {}

sp::tag::tag(const sp::string_t & name) {
	this->_name = name;
}

sp::tag::tag(const sp::string_t & name, sp::attribute_table table) {
	this->_name = name;
	this->attrs = table;
}

sp::tag::tag(const sp::string_t & name, sp::tag_map childs) {
	this->_name = name;
	this->childs = childs;
}

sp::tag::tag(const sp::string_t & name, sp::attribute_table table, sp::tag_map childs) {
	this->_name = name;
	this->attrs = table;
	this->childs = childs;
}

sp::tag::tag(const sp::char_t * name) {
	this->_name = name;
}

sp::tag::tag(const sp::char_t * name, sp::attribute_table table) {
	this->_name = name;
	this->attrs = table;
}

sp::tag::tag(const sp::char_t * name, sp::tag_map childs) {
	this->_name = name;
	this->childs = childs;
}

sp::tag::tag(const sp::char_t * name, sp::attribute_table table, sp::tag_map childs) {
	this->_name = name;
	this->attrs = table;
	this->childs = childs;
}

sp::tag_type sp::tag::type() const {
	return _type;
}

sp::tag_type & sp::tag::type(){
	return _type;
}

sp::value sp::tag::text() const{
	return _text;
}

sp::value & sp::tag::text(){
	return _text;
}

sp::attribute_table sp::tag::attributes() const{
	return this->attrs;
}

sp::attribute_table & sp::tag::attributes(){
	return this->attrs;
}

void sp::tag::add_tag(sp::tag & new_child){
	if (this->check_tag(new_child._name)) {
		throw sp::error_type::tag_exist;
	}
	this->childs.insert({ new_child._name, new_child });
}

void sp::tag::add_tag(sp::tag * new_child){
	if (this->check_tag(new_child->_name)) {
		throw sp::error_type::tag_exist;
	}
	this->childs.insert({ new_child->_name, *new_child });
}

void sp::tag::remove_tag(const sp::char_t * name){
	if (this->check_tag(name)) {
		throw sp::error_type::tag_exist;
	}
	this->childs.erase(name);
}

void sp::tag::remove_tag(const sp::string_t & name){
	remove_tag(name.c_str());
}

bool sp::tag::check_tag(const sp::char_t * name){
	return this->childs.count(name) != 0;
}

bool sp::tag::check_tag(const sp::string_t & name){
	return this->childs.count(name) != 0;
}

sp::const_tag_iterator sp::tag::begin() const{
	return this->childs.begin();
}

sp::tag_iterator sp::tag::begin(){
	return this->childs.begin();
}

sp::const_tag_iterator sp::tag::end() const{
	return this->childs.end();
}

sp::tag_iterator sp::tag::end(){
	return this->childs.end();
}