#ifndef  __SPXML__

#include <fstream>
#include <string>
#include <cctype>
#include <map>
#include <vector>

#define SPW_MODE

namespace sp {
#ifdef SPW_MODE
	typedef wchar_t char_t;
	typedef std::wstring string_t;
	typedef std::wistream input_stream; 
	typedef std::wostream output_stream;
	typedef std::wifstream ifile;
	typedef std::wofstream ofile;
#define _t(c) L##c
#else
	typedef char char_t;
	typedef std::string string_t;
	typedef std::istream input_stream;
	typedef std::ostream output_stream;
	typedef std::ifstream ifile;
	typedef std::ofstream ofile;
#define _t(c) c
#endif

	enum class error_type {
		uncorrect_char_after_open_brt = 0, 
		error_tag_name,
		comment_error,
		text_end_error,
		attrib_name_wrong,
		atribute_name_error,
		attribute_value_exec_open_quot,
		attribute_value_exec_close_quot,
		attribute_value_error,
		unknown_ent, 
		autoclose_tag_error,
	};

	enum class char_type {
		simlpe_char = 0, 
		digit,
		semicolon, 
		ampers,
		question_mark,
		exclam_mark,
		open_brt,
		close_brt,
		apos,
		quot,
		space,
		next_line,
		slash,
		eval,
		end, 
		unknown
	};

	enum class token_type {
		none = 0,
		prol_begin,
		prol_end, 
		tag_name, 
		tag_text,
		tag_autoclose_end,
		tag_end,
		atribute_name,
		atribute_value,
		comment,
		end_of_data
	};

	// структура получаемого символа
	struct xml_char {
		sp::char_t data;
		sp::char_type type;
	};

	// стуруктура токена 
	struct token {
		sp::string_t data; 
		sp::token_type type;
	};

	// базовый класс для получения данных
	class reader {
	public:
		// конструктор класса
		reader(const sp::char_t * data);
		reader(const sp::string_t & data);
		reader(const sp::input_stream & data);

		// возвращает следующий сивол 
		virtual sp::xml_char get_next_char() = 0;

		// возвращает true если класс чтения готов к использованию
		virtual bool good() = 0;

		// возвращает тип символа
		sp::char_type get_char_type(sp::char_t ch);

		// возвращает следующий токен
		sp::token get_next_token();

		// возвращает последний полученный токен
		sp::token & get_last_token();
	
	private:
		// инициализирует таблицы символов и сущностей
		void init_tables();

		// обработка открывающей скобки
		void open_brt_proc();

		// обработка открывающей скобки
		void close_brt_proc();

		// обработка имени тега
		void name_proc();

		// обрабока комментария 
		void comment_proc();

		// обработка имени атрибута
		void attrib_name_proc();

		// обработка занчения атрибута
		void attrib_val_proc();

		// обработка сущностей
		void ent_proc();

		// пропускает символы
		void skip_chars(std::vector<sp::char_type> vect);

		std::map<sp::char_t, sp::char_type> char_table; // таблица символов
		std::map<sp::string_t, sp::char_t > ent_table; // таблица сущностей
		xml_char cur_char = { 0, sp::char_type::unknown }; // последний полученный сивол 
		token last_token; // последний полученный токен
	};

	class string_reader : public sp::reader {
	public:
		// конструктор класса
		string_reader(const sp::char_t * data);
		string_reader(const sp::string_t & data);

		// возвращает следующий сивол 
		sp::xml_char get_next_char();

		// возвращает true если класс чтения готов к использованию
		bool good();

	private:
		sp::string_t data; // строка с XML
		size_t position = 0; // позиция 
	};

	class file_reader : public sp::reader {
	public:
		// конструктор класса
		file_reader(const sp::char_t * file_name);
		file_reader(const sp::string_t & file_name);

		// деструктор класса 
		~file_reader();

		// возвращает следующий сивол 
		sp::xml_char get_next_char();

		// возвращает true если класс чтения готов к использованию
		bool good();
	private:
		sp::ifile file; // файл с данными
	};

	class stream_reader : public sp::reader {
	public:
		// конструктор класса
		stream_reader(sp::input_stream & data);

		// десруктор класса
		~stream_reader();

		// возвращает следующий сивол 
		sp::xml_char get_next_char();

		// возвращает true если класс чтения готов к использованию
		bool good();
	private:
		sp::input_stream * stream; // поток с данными
	};

};

#endif