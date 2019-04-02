/*
cpptempl
=================
This is a template engine for C++.

Syntax
=================
Variables: {$variable_name}
Loops: {% for person in people %}Name: {$person.name}{% endfor %}
If: {% for person.name == "Bob" %}Full name: Robert{% endif %}

Copyright
==================
Author: Ryan Ginstrom
MIT License

Usage
=======================
    std::string text = "{% if item %}{$item}{% endif %}\n"
		"{% if thing %}{$thing}{% endif %}" ;
	cpptempl::data_map data ;
	data["item"] = cpptempl::make_data("aaa") ;
	data["thing"] = cpptempl::make_data("bbb") ;

    std::string result = cpptempl::parse(text, data) ;

Handy Functions
========================
make_data() : Feed it a string, data_map, or data_list to create a data entry.
Example:
	data_map person ;
	person["name"] = make_data("Bob") ;
	person["occupation"] = make_data("Plumber") ;
	data_map data ;
	data["person"] = make_data(person) ;
    std::string result = parse(templ_text, data) ;

*/
#pragma once

#ifdef _WIN32
#pragma warning( disable : 4996 ) // 'std::copy': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
#pragma warning( disable : 4512 ) // 'std::copy': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
#endif

#include <string>
#include <vector>
#include <map>							
#include <memory>
#include <unordered_map>
#include <boost/lexical_cast.hpp>

#include <iostream>

namespace cpptempl
{
	// various typedefs

	// data classes
	class Data ;
	class DataValue ;
	class DataList ;
	class DataMap ;

	class data_ptr {
	public:
		data_ptr() {}
		template<typename T> data_ptr(const T& data) {
			this->operator =(data);
		}
        data_ptr(DataValue* data);
        data_ptr(DataList* data);
        data_ptr(DataMap* data);
		data_ptr(const data_ptr& data) {
			ptr = data.ptr;
		}
		template<typename T> void operator = (const T& data);
		void push_back(const data_ptr& data);
		virtual ~data_ptr() {}
		Data* operator ->() {
			return ptr.get();
		}
	private:
		std::shared_ptr<Data> ptr;
	};
	typedef std::vector<data_ptr> data_list ;

	class data_map {
	public:
		data_ptr& operator [](const std::string& key);
		bool empty();
		bool has(const std::string& key);
	private:
		std::unordered_map<std::string, data_ptr> data;
	};

	template<> inline void data_ptr::operator = (const data_ptr& data);
	template<> void data_ptr::operator = (const std::string& data);
	template<> void data_ptr::operator = (const std::string& data);
	template<> void data_ptr::operator = (const data_map& data);
	template<typename T>
	void data_ptr::operator = (const T& data) {
		std::string data_str = boost::lexical_cast<std::string>(data);
		this->operator =(data_str);
	}

	// token classes
	class Token ;
	typedef std::shared_ptr<Token> token_ptr ;
	typedef std::vector<token_ptr> token_vector ;

	// Custom exception class for library errors
	class TemplateException : public std::exception
	{
	public:
		TemplateException(std::string reason) : m_reason(reason){}
		~TemplateException() {}
		const char* what() const noexcept {
			return m_reason.c_str();
		}
	private:
		std::string m_reason;
	};

	// Data types used in templates
	class Data
	{
	public:
		virtual ~Data() {}
		virtual bool empty() = 0 ;
		virtual std::string getvalue();
		virtual data_list& getlist();
		virtual data_map& getmap() ;
	};

	class DataValue : public Data
	{
        std::string m_value ;
	public:
		DataValue(std::string value) : m_value(value){}
        std::string getvalue();
		bool empty();
	};

	class DataList : public Data
	{
		data_list m_items ;
	public:
		DataList(const data_list &items) : m_items(items){}
		data_list& getlist() ;
		bool empty();
	};

	class DataMap : public Data
	{
		data_map m_items ;
	public:
		DataMap(const data_map &items) : m_items(items){}
		data_map& getmap();
		bool empty();
	};

	// convenience functions for making data objects
	inline data_ptr make_data(std::string val)
	{
		return data_ptr(new DataValue(val)) ;
	}
	inline data_ptr make_data(data_list &val)
	{
		return data_ptr(new DataList(val)) ;
	}
	inline data_ptr make_data(data_map &val)
	{
		return data_ptr(new DataMap(val)) ;
	}
	// get a data value from a data map
	// e.g. foo.bar => data["foo"]["bar"]
	data_ptr parse_val(std::string key, data_map &data) ;

	typedef enum 
	{
		TOKEN_TYPE_NONE,
		TOKEN_TYPE_TEXT,
		TOKEN_TYPE_VAR,
		TOKEN_TYPE_IF,
		TOKEN_TYPE_FOR,
		TOKEN_TYPE_ENDIF,
		TOKEN_TYPE_ENDFOR,
	} TokenType;

	// Template tokens
	// base class for all token types
	class Token
	{
	public:
		virtual ~Token() {};
		virtual TokenType gettype() = 0 ;
		virtual void gettext(std::ostream &stream, data_map &data) = 0 ;
		virtual void set_children(token_vector &children);
		virtual token_vector & get_children();
	};

	// normal text
	class TokenText : public Token
	{
        std::string m_text ;
	public:
		TokenText(std::string text) : m_text(text){}
		TokenType gettype();
		void gettext(std::ostream &stream, data_map &data);
	};

	// variable
	class TokenVar : public Token
	{
        std::string m_key ;
	public:
		TokenVar(std::string key) : m_key(key){}
		TokenType gettype();
		void gettext(std::ostream &stream, data_map &data);
	};

	// for block
	class TokenFor : public Token 
	{
	public:
        std::string m_key ;
        std::string m_val ;
		token_vector m_children ;
		TokenFor(std::string expr);
		TokenType gettype();
		void gettext(std::ostream &stream, data_map &data);
		void set_children(token_vector &children);
		token_vector &get_children();
	};

	// if block
	class TokenIf : public Token
	{
	public:
        std::string m_expr ;
		token_vector m_children ;
		TokenIf(std::string expr) : m_expr(expr){}
		TokenType gettype();
		void gettext(std::ostream &stream, data_map &data);
		bool is_true(std::string expr, data_map &data);
		void set_children(token_vector &children);
		token_vector &get_children();
	};

	// end of block
	class TokenEnd : public Token // end of control block
	{
        std::string m_type ;
	public:
		TokenEnd(std::string text) : m_type(text){}
		TokenType gettype();
		void gettext(std::ostream &stream, data_map &data);
	};

    std::string gettext(token_ptr token, data_map &data) ;

	void parse_tree(token_vector &tokens, token_vector &tree, TokenType until=TOKEN_TYPE_NONE) ;
	token_vector & tokenize(std::string text, token_vector &tokens) ;

	// The big daddy. Pass in the template and data, 
	// and get out a completed doc.
	void parse(std::ostream &stream, std::string templ_text, data_map &data) ;
    std::string parse(std::string templ_text, data_map &data);
	std::string parse(std::string templ_text, data_map &data);
}
