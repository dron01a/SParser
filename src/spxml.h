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

	class value;
	class attribute;
	class tag;

	typedef std::map<sp::string_t, sp::attribute>::iterator attr_iterator;
	typedef std::map<sp::string_t, sp::attribute>::const_iterator const_attr_iterator;
	typedef std::map<sp::string_t, sp::tag>::iterator tag_iterator;
	typedef std::map<sp::string_t, sp::tag>::const_iterator const_tag_iterator;

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
		error_value_type,
		atribute_exist,
		atribute_not_exist,
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

	enum class value_type {
		_string,
		_double,
		_bool
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

	// класс значеня атрибута или текста
	class value {
	public:
		// конструктор класса
		value();
		value(const sp::char_t * data);
		value(const sp::string_t & data);
		value(sp::char_t data);
		value(int data);
		value(double data);
		value(bool data);

		// возвращает тип значения
		sp::value_type type();
		sp::value_type type() const;

		// возвращает как строку
		sp::string_t to_string() const;

		// возвращает как целое число
		int to_int() const;

		// возвращает как число с плавющей точкой
		double to_double() const;

		// возвращает булево значение
		bool to_bool() const;

		// устанавливае значение
		void set(const sp::char_t * data);
		void set(const sp::string_t & data);
		void set(sp::char_t data);
		void set(int data);
		void set(double data);
		void set(bool data);
		
		// операторы сравнения
		bool operator==(const sp::value & val) const;
		bool operator!=(const sp::value & val) const;
		bool operator>(const sp::value & val) const;
		bool operator>=(const sp::value & val) const;
		bool operator<(const sp::value & val) const;
		bool operator<=(const sp::value & val) const;

		//операторы присваивания
		sp::value & operator=(const sp::char_t * data);
		sp::value & operator=(const sp::string_t & data);
		sp::value & operator=(sp::char_t data);
		sp::value & operator=(int data);
		sp::value & operator=(double data);
		sp::value & operator=(bool data);
	private:
		sp::value_type _type = sp::value_type::_string; // тип значения
		sp::string_t data; // данные
	};

	// класс атрибута
	class attribute {
	public:
		// конструктор класса
		attribute();
		attribute(const sp::string_t & name, sp::value val);
		attribute(const sp::char_t * name, sp::value val);

		// возвращает и устанавливает имя
		sp::string_t name() const;
		void name(const sp::string_t & str);
		void name(const sp::char_t * str);

		// возвращает и устанавливает значение
		sp::value value() const;
		void value(sp::value val);

		// операторы сравнения
		bool operator==(const sp::attribute & attrib) const;
		bool operator!=(const sp::attribute & attrib) const;
		bool operator>(const sp::attribute & attrib) const;
		bool operator>=(const sp::attribute & attrib) const;
		bool operator<(const sp::attribute & attrib) const;
		bool operator<=(const sp::attribute & attrib) const;

	private:
		sp::value _value; // значение  
		sp::string_t _name; // имя 
	};

	// таблица с атрибутами (обертка над классом sd::map)
	class attribute_table {
	public:
		// конструктор класса
		attribute_table();
		attribute_table(std::initializer_list<attribute> attribute_list);
	
		// оператор выдачи по индексу 
		sp::attribute & operator[](const sp::char_t * name);
		sp::attribute & operator[](const sp::string_t & name);
		sp::attribute operator[](const sp::char_t * name) const;
		sp::attribute operator[](const sp::string_t & name) const;

		// добавляет атрибут
		void add(const sp::char_t * name, sp::value val);
		void add(const sp::string_t & name, sp::value val);
		void add(attribute & attribute);

		// удаляет атрибут
		void remove(const sp::string_t & name);
		void remove(const sp::char_t * name);

		// проверяет наличие атрибута
		bool check(const sp::char_t * name);
		bool check(const sp::string_t & name);
		bool check(const sp::char_t * name, sp::value val);
		bool check(const sp::string_t & name, sp::value val);
		bool check(attribute & attribute);
	
		// возвращает размер таблицы
		size_t size();

		// возвращает итератор на начало таблицы
		sp::const_attr_iterator begin() const;
		sp::attr_iterator begin();

		// возвращает итератор на конец таблицы
		sp::const_attr_iterator end() const;
		sp::attr_iterator end();

		// операторы сравнения
		bool operator==(const sp::attribute_table & attrib_table);
		bool operator!=(const sp::attribute_table & attrib_table);

	private:
		std::map<sp::string_t, sp::attribute> table; // map с атрибутами
	};

};

#endif