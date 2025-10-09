#include "spxml.h"

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
	if (last_char.data == 0) {
		last_char = get_next_char();
	}
	switch (last_char.type){
	case sp::char_type::open_brt:
		open_brt_proc();
		break;
	case sp::char_type::close_brt:
		break;

	
	default:
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
	char_table.insert({ _t('/'), sp::char_type::slash });
	char_table.insert({ _t('\0'), sp::char_type::end });
	ent_table.insert({ _t('<'), _t("&lt;") });
	ent_table.insert({ _t('>'), _t("&gt;") });
	ent_table.insert({ _t('&'), _t("&amp;") });
	ent_table.insert({ _t('\''), _t("&apos;") });
	ent_table.insert({ _t('\"'), _t("&quot;") });
}

void sp::reader::open_brt_proc(){
	last_char = get_next_char(); // получаем следующий сивол 
	switch (last_char.type){
	case sp::char_type::simlpe_char:
		name_proc();
		last_token.type = sp::token_type::tag_name;
		break;
	case sp::char_type::slash:
		last_char = get_next_char();
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

void sp::reader::name_proc(){
	while (last_char.type != sp::char_type::close_brt && last_char.type != sp::char_type::space) {
		switch (last_char.type){
		case sp::char_type::digit:
		case sp::char_type::simlpe_char:
			last_token.data += last_char.data; // добавляем символ к имени тега
			break;
		default:
			throw sp::error_type::error_tag_name; // ошибка в имени тега
		}
		last_char = get_next_char(); // получаем следующий сивол 
	}
}

void sp::reader::comment_proc(){
	last_char = get_next_char(); // получаем следующий сивол 
	if (last_char.data == _t('-')) {
		last_char = get_next_char(); // получаем следующий сивол 
		if (last_char.data == _t('-')) {
			while(last_char.type != sp::char_type::end){
				if (last_char.data == _t('-')) {
					last_char = get_next_char(); // получаем следующий сивол 
					if (last_char.data == _t('-')) {
						last_char = get_next_char(); // получаем следующий сивол 
						if (last_char.type == sp::char_type::close_brt) {
							return;
						}
						break;
					}
				}
				last_char = get_next_char(); // получаем следующий сивол 
			}
		}
	}
	throw sp::error_type::comment_error;
}

void sp::reader::skip_chars(std::vector<sp::char_type> vect){
	while (std::count(vect.begin(), vect.end(), last_char.type) != 0) {
		last_char = get_next_char(); // получаем следующий сивол 
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
