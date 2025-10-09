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

void sp::reader::ent_proc(){
	sp::string_t temp; // строка в с сущностью
	while (cur_char.type == sp::char_type::semicolon) {
		temp += cur_char.data;
		cur_char = get_next_char(); // получаем следующий сивол
	}
	if (ent_table.count(temp) != 0) {
		last_token.data += ent_table[temp];
	}
	cur_char = get_next_char(); // получаем следующий сивол 
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
