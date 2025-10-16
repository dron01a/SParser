#ifndef  __SPXML__

#include <fstream>
#include <string>
#include <cctype>
#include <map>
#include <vector>
#include <algorithm>

#define SPW_MODE

namespace sp {
#ifdef SPW_MODE
	typedef wchar_t char_t;
	typedef std::wstring string_t;
	typedef std::wistream input_stream; 
	typedef std::wostream output_stream;
	typedef std::wifstream ifile;
	typedef std::wofstream ofile;
	typedef std::wios ios;
#define _t(c) L##c
#else
	typedef char char_t;
	typedef std::string string_t;
	typedef std::istream input_stream;
	typedef std::ostream output_stream;
	typedef std::ifstream ifile;
	typedef std::ofstream ofile;
	typedef std::ios ios;
#define _t(c) c
#endif

	class value;
	class attribute;
	class tag;
	class reader;
	class writer;
	class xml_parser;

	typedef std::multimap<sp::string_t, sp::tag> tag_map;
	typedef std::map<sp::string_t, sp::attribute> attribute_map;
	typedef sp::attribute_map::iterator attr_iterator;
	typedef sp::attribute_map::const_iterator const_attr_iterator;
	typedef sp::tag_map::iterator tag_iterator;
	typedef sp::tag_map::const_iterator const_tag_iterator;
	typedef std::vector<sp::tag *> tag_vector;

	enum class error_type {
		none = 0,
		uncorrect_char_after_open_brt, 
		error_tag_name,
		comment_error,
		text_end_error,
		attrib_name_wrong,
		atribute_name_error,
		attribute_value_exec_open_quot,
		attribute_value_exec_close_quot,
		attribute_value_error,
		unknown_ent, 
		declaration_error,
		autoclose_tag_error,
		error_value_type,
		atribute_exist,
		atribute_not_exist,
		tag_not_exist,
		file_not_found,
		string_is_empty,
		thread_is_bad,
		not_found_end_of_tag,
		attribute_parse_error,
		invalid_standalone
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

	enum class tag_type {
		tag = 0, // простой тег
		autoclose_tag, // самозакрывающийся тег
	};

	enum class write_result {
		write_ok,
		write_error
	};

	enum class parse_result {
		parse_ok = 0,
		parse_error
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
		bool check(attribute_table & table);
	
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
		attribute_map table; // map с атрибутами
	};

	// класс описывающий тег
	class tag {
	public:
		// консрукторы класса
		tag();
		tag(const sp::string_t & name);
		tag(const sp::string_t & name, sp::attribute_table table);
		tag(const sp::string_t & name, sp::tag_map childs);
		tag(const sp::string_t & name, sp::attribute_table table, sp::tag_map childs);
		tag(const sp::char_t * name);
		tag(const sp::char_t * name, sp::attribute_table table);
		tag(const sp::char_t * name, sp::tag_map childs);
		tag(const sp::char_t * name, sp::attribute_table table, sp::tag_map childs);
		
		~tag() { };

		// возвращает имя тега
		sp::string_t name() const;
		sp::string_t & name();

		// возвращает тип тега
		sp::tag_type type() const;
		sp::tag_type & type();

		// возвращает текст тега
		sp::value text() const;
		sp::value & text();

		// возвращает атрибуты тега
		sp::attribute_table attributes() const;
		sp::attribute_table & attributes();

		// возвращает тег 
		sp::tag * get_tag(const sp::char_t * name);
		sp::tag * get_tag(const sp::char_t * name, sp::attribute_table attrib);
		sp::tag * get_tag(const sp::string_t & name);
		sp::tag * get_tag(const sp::string_t & name, sp::attribute_table attrib);

		// возвращает все теги с заданым именем и атрибутами
		sp::tag_vector select(const sp::char_t * name);
		sp::tag_vector select(const sp::char_t * name, sp::attribute_table attrib);
		sp::tag_vector select(const sp::string_t & name);
		sp::tag_vector select(const sp::string_t & name, sp::attribute_table attrib);

		// добавляет тег
		void add_tag(sp::tag & new_child);
		void add_tag(sp::tag * new_child);

		// удаляет тег
		void remove_tag(const sp::char_t * name);
		void remove_tag(const sp::string_t & name);

		// проверяет наличие тега
		bool check_tag(const sp::char_t * name);
		bool check_tag(const sp::string_t & name);

		// возвращает итератор начала map с вложенными тегами
		sp::const_tag_iterator begin() const;
		sp::tag_iterator begin();

		// возвращает итератор конца map с вложенными тегами
		sp::const_tag_iterator end() const;
		sp::tag_iterator end();

	private:
		sp::string_t _name; // имя тега
		sp::value _text; // текст тега
		sp::attribute_table attrs; // атрибуты тега
		sp::tag_type _type = sp::tag_type::tag; // тип тега
		sp::tag_map childs; // вложенные теги

	};

	// базовый класс для записи данных 
	class writer {
	public:
		// конструктор класса
		writer(sp::string_t & data);
		writer(const sp::char_t * data);
		writer(sp::output_stream & stream);

		// функция начинающая запись
		sp::write_result write(sp::tag & root);

		// установка флага форматирования 
		void format(bool val);

		// возвращает true если готов записывать данные
		virtual bool ready() = 0;

		// функция вывода информации
		virtual void write_data(sp::char_t data) = 0;
		virtual void write_data(const sp::char_t * data) = 0;
		virtual void write_data(const sp::string_t & data) = 0;

	private:
		// печать тега
		void write_tag(sp::tag & tag);

		// печать атрибута
		void write_attr(sp::attribute & attr);

		// печать текста
		void write_text(sp::value & text);

		// вставка табуляций
		void insert_tabs(int n);

		bool _format = true; // флаг форатирования
		int level = 0;

	};

	// запись в файл
	class file_writer : public writer {
	public:
		// конструктор класса
		file_writer(sp::string_t & file_name);
		file_writer(const sp::char_t * file_name);

		// деструктор класса
		~file_writer();

		// функция вывода информации
		void write_data(sp::char_t data);
		void write_data(const sp::char_t * data);
		void write_data(const sp::string_t & data);

		// возвращает true если готов записывать данные
		bool ready();

	private:
		sp::ofile file; // файл вывода
	};

	// запись в строку
	class string_writer : public writer {
	public:
		// конструктор класса
		string_writer(sp::string_t & result);

		// деструктор класса
		~string_writer();

		// функция вывода информации
		void write_data(sp::char_t data);
		void write_data(const sp::char_t * data);
		void write_data(const sp::string_t & data);

		// возвращает true если готов записывать данные
		bool ready();

	private:
		sp::string_t * _result = nullptr; // указатель на строку
	};

	// запись в поток
	class stream_writer : public writer {
	public:
		// конструктор класса
		stream_writer(sp::output_stream & stream);

		// деструктор класса
		~stream_writer();

		// функция вывода информации
		void write_data(sp::char_t data);
		void write_data(const sp::char_t * data);
		void write_data(const sp::string_t & data);

		// возвращает true если готов записывать данные
		bool ready();

	private:
		sp::output_stream * stream; // ссылка на поток
	};

	// класс парсера xml
	class xml_parser {
	public:
		// конструктор класса
		xml_parser();

		// деструтор класса 
		~xml_parser();

		// возвращает корневой тег
		sp::tag * root(); 

		// возвращает кодировку 
		sp::string_t encoding();
		void encoding(const sp::char_t * enc);
		void encoding(const sp::string_t & enc);

		// возвращает версию 
		sp::string_t version();
		void version(const sp::char_t * version);
		void version(const sp::string_t & version);

		// возвращает standalone 
		sp::string_t standalone();
		void standalone(const sp::char_t * standalone);
		void standalone(const sp::string_t & standalone);

		// загрузка из файла
		sp::parse_result load_from_file(const sp::char_t * file_name);
		sp::parse_result load_from_file(const sp::string_t & file_name);

		// загрузка из строки
		sp::parse_result load_from_string(const sp::char_t * data);
		sp::parse_result load_from_string(const sp::string_t & data);

		// загрузка из потока
		sp::parse_result load_from_stream(sp::input_stream & stream);

		// загрузка данных в файл 
		sp::write_result load_to_file(const sp::string_t & file_name);
		sp::write_result load_to_file(const sp::char_t * file_name);

		// загрузка данных в строку
		sp::write_result load_to_string(sp::string_t & result);

		// загрузка в поток
	    sp::write_result load_to_stream(sp::output_stream & stream);

		// функция возвращающая последнюю полученную ошибку
		sp::error_type get_last_error();
	private:
		// функция запускающая парсинг
		sp::parse_result parse();
		
		// парсинг тега
		sp::tag * parse_tag();

		// парсинг атрибута
		sp::attribute parse_attribute();

		// парсинг пролога
		sp::tag * parse_declaration();

		sp::reader * reader = nullptr; // для получения текста
		sp::tag * _root = nullptr; // корневой тег
		sp::tag * declaration = nullptr; // пролог
		sp::error_type last_error = sp::error_type::none; // последняя полученная ошибка

	};
};

#endif