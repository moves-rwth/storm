#ifdef _MSC_VER
#include "stdafx.h"
#endif

#include "cpptempl.h"

#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace cpptempl
{
	//////////////////////////////////////////////////////////////////////////
	// Data classes
	//////////////////////////////////////////////////////////////////////////

	// data_map
	data_ptr& data_map::operator [](const std::wstring& key) {
		return data[key];
	}
	data_ptr& data_map::operator [](const std::string& key) {
		return data[utf8_to_wide(key)];
	}
	bool data_map::empty() {
		return data.empty();
	}
	bool data_map::has(const wstring& key) {
		return data.find(key) != data.end();
	}

	// data_ptr
	template<>
	inline void data_ptr::operator = (const data_ptr& data) {
		ptr = data.ptr;
	}

	template<>
	void data_ptr::operator = (const std::string& data) {
		ptr.reset(new DataValue(utf8_to_wide(data)));
	}

	template<>
	void data_ptr::operator = (const std::wstring& data) {
		ptr.reset(new DataValue(data));
	}

	template<>
	void data_ptr::operator = (const data_map& data) {
		ptr.reset(new DataMap(data));
	}

	void data_ptr::push_back(const data_ptr& data) {
		if (!ptr) {
			ptr.reset(new DataList(data_list()));
		}
		data_list& list = ptr->getlist();
		list.push_back(data);
	}

	// base data
	wstring Data::getvalue()
	{
		throw TemplateException("Data item is not a value") ;
	}

	data_list& Data::getlist()
	{
		throw TemplateException("Data item is not a list") ;
	}
	data_map& Data::getmap()
	{
		throw TemplateException("Data item is not a dictionary") ;
	}
	// data value
	wstring DataValue::getvalue()
	{
		return m_value ;
	}
	bool DataValue::empty()
	{
		return m_value.empty();
	}
	// data list
	data_list& DataList::getlist()
	{
		return m_items ;
	}

	bool DataList::empty()
	{
		return m_items.empty();
	}
	// data map
	data_map& DataMap:: getmap()
	{
		return m_items ;
	}
	bool DataMap::empty()
	{
		return m_items.empty();
	}
	//////////////////////////////////////////////////////////////////////////
	// parse_val
	//////////////////////////////////////////////////////////////////////////
	data_ptr parse_val(wstring key, data_map &data)
	{
		// quoted string
		if (key[0] == L'\"')
		{
			return make_data(boost::trim_copy_if(key, boost::is_any_of(L"\""))) ;
		}
		// check for dotted notation, i.e [foo.bar]
		size_t index = key.find(L".") ;
		if (index == wstring::npos)
		{
			if (!data.has(key))
			{
				return make_data(L"{$" + key + L"}") ;
			}
			return data[key] ;
		}

		wstring sub_key = key.substr(0, index) ;
		if (!data.has(sub_key))
		{
			return make_data(L"{$" + key + L"}") ;
		}
		data_ptr item = data[sub_key] ;
		return parse_val(key.substr(index+1), item->getmap()) ;
	}

	//////////////////////////////////////////////////////////////////////////
	// Token classes
	//////////////////////////////////////////////////////////////////////////

	// defaults, overridden by subclasses with children
	void Token::set_children( token_vector & )
	{
		throw TemplateException("This token type cannot have children") ;
	}

	token_vector & Token::get_children()
	{
		throw TemplateException("This token type cannot have children") ;
	}

	// TokenText
	TokenType TokenText::gettype()
	{
		return TOKEN_TYPE_TEXT ;
	}

	void TokenText::gettext( std::wostream &stream, data_map & )
	{
		stream << m_text ;
	}

	// TokenVar
	TokenType TokenVar::gettype()
	{
		return TOKEN_TYPE_VAR ;
	}

	void TokenVar::gettext( std::wostream &stream, data_map &data )
	{
		stream << parse_val(m_key, data)->getvalue() ;
	}

	// TokenFor
	TokenFor::TokenFor(wstring expr)
	{
		std::vector<wstring> elements ;
		boost::split(elements, expr, boost::is_space()) ;
		if (elements.size() != 4u)
		{
			throw TemplateException("Invalid syntax in for statement") ;
		}
		m_val = elements[1] ;
		m_key = elements[3] ;
	}

	TokenType TokenFor::gettype()
	{
		return TOKEN_TYPE_FOR ;
	}

	void TokenFor::gettext( std::wostream &stream, data_map &data )
	{
		data_ptr value = parse_val(m_key, data) ;
		data_list &items = value->getlist() ;
		for (size_t i = 0 ; i < items.size() ; ++i)
		{
			data_map loop ;
			loop[L"index"] = make_data(boost::lexical_cast<wstring>(i+1)) ;
			loop[L"index0"] = make_data(boost::lexical_cast<wstring>(i)) ;
			data[L"loop"] = make_data(loop);
			data[m_val] = items[i] ;
			for(size_t j = 0 ; j < m_children.size() ; ++j)
			{
				m_children[j]->gettext(stream, data) ;
			}
		}
	}

	void TokenFor::set_children( token_vector &children )
	{
		m_children.assign(children.begin(), children.end()) ;
	}

	token_vector & TokenFor::get_children()
	{
		return m_children;
	}

	// TokenIf
	TokenType TokenIf::gettype()
	{
		return TOKEN_TYPE_IF ;
	}

	void TokenIf::gettext( std::wostream &stream, data_map &data )
	{
		if (is_true(m_expr, data))
		{
			for(size_t j = 0 ; j < m_children.size() ; ++j)
			{
				m_children[j]->gettext(stream, data) ;
			}
		}
	}

	bool TokenIf::is_true( wstring expr, data_map &data )
	{
		std::vector<wstring> elements ;
		boost::split(elements, expr, boost::is_space()) ;

		if (elements[1] == L"not")
		{
			return parse_val(elements[2], data)->empty() ;
		}
		if (elements.size() == 2)
		{
			return ! parse_val(elements[1], data)->empty() ;
		}
		data_ptr lhs = parse_val(elements[1], data) ;
		data_ptr rhs = parse_val(elements[3], data) ;
		if (elements[2] == L"==")
		{
			return lhs->getvalue() == rhs->getvalue() ;
		}
		return lhs->getvalue() != rhs->getvalue() ;
	}

	void TokenIf::set_children( token_vector &children )
	{
		m_children.assign(children.begin(), children.end()) ;
	}

	token_vector & TokenIf::get_children()
	{
		return m_children;
	}

	// TokenEnd
	TokenType TokenEnd::gettype()
	{
		return m_type == L"endfor" ? TOKEN_TYPE_ENDFOR : TOKEN_TYPE_ENDIF ;
	}

	void TokenEnd::gettext( std::wostream &, data_map &)
	{
		throw TemplateException("End-of-control statements have no associated text") ;
	}

	// gettext
	// generic helper for getting text from tokens.

	wstring gettext(token_ptr token, data_map &data)
	{
		std::wostringstream stream ;
		token->gettext(stream, data) ;
		return stream.str() ;
	}
	//////////////////////////////////////////////////////////////////////////
	// parse_tree
	// recursively parses list of tokens into a tree
	//////////////////////////////////////////////////////////////////////////
	void parse_tree(token_vector &tokens, token_vector &tree, TokenType until)
	{
		while(! tokens.empty())
		{
			// 'pops' first item off list
			token_ptr token = tokens[0] ;
			tokens.erase(tokens.begin()) ;

			if (token->gettype() == TOKEN_TYPE_FOR)
			{
				token_vector children ;
				parse_tree(tokens, children, TOKEN_TYPE_ENDFOR) ;
				token->set_children(children) ;
			}
			else if (token->gettype() == TOKEN_TYPE_IF)
			{
				token_vector children ;
				parse_tree(tokens, children, TOKEN_TYPE_ENDIF) ;
				token->set_children(children) ;
			}
			else if (token->gettype() == until)
			{
				return ;
			}
			tree.push_back(token) ;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// tokenize
	// parses a template into tokens (text, for, if, variable)
	//////////////////////////////////////////////////////////////////////////
	token_vector & tokenize(wstring text, token_vector &tokens)
	{
		while(! text.empty())
		{
			size_t pos = text.find(L"{") ;
			if (pos == wstring::npos)
			{
				if (! text.empty())
				{
					tokens.push_back(token_ptr(new TokenText(text))) ;
				}
				return tokens ;
			}
			wstring pre_text = text.substr(0, pos) ;
			if (! pre_text.empty())
			{
				tokens.push_back(token_ptr(new TokenText(pre_text))) ;
			}
			text = text.substr(pos+1) ;
			if (text.empty())
			{
				tokens.push_back(token_ptr(new TokenText(L"{"))) ;
				return tokens ;
			}

			// variable
			if (text[0] == L'$')
			{
				pos = text.find(L"}") ;
				if (pos != wstring::npos)
				{
					tokens.push_back(token_ptr (new TokenVar(text.substr(1, pos-1)))) ;
					text = text.substr(pos+1) ;
				}
			}
			// control statement
			else if (text[0] == L'%')
			{
				pos = text.find(L"}") ;
				if (pos != wstring::npos)
				{
					wstring expression = boost::trim_copy(text.substr(1, pos-2)) ;
					text = text.substr(pos+1) ;
					if (boost::starts_with(expression, L"for"))
					{
						tokens.push_back(token_ptr (new TokenFor(expression))) ;
					}
					else if (boost::starts_with(expression, L"if"))
					{
						tokens.push_back(token_ptr (new TokenIf(expression))) ;
					}
					else
					{
						tokens.push_back(token_ptr (new TokenEnd(boost::trim_copy(expression)))) ;
					}
				}
			}
			else
			{
				tokens.push_back(token_ptr(new TokenText(L"{"))) ;
			}
		}
		return tokens ;
	}

	/************************************************************************
	* parse
	*
	*  1. tokenizes template
	*  2. parses tokens into tree
	*  3. resolves template
	*  4. returns converted text
	************************************************************************/
	wstring parse(wstring templ_text, data_map &data)
	{
		std::wostringstream stream ;
		parse(stream, templ_text, data) ;
		return stream.str() ;
	}
	std::string parse(std::string templ_text, data_map &data)
	{
		return wide_to_utf8(parse(utf8_to_wide(templ_text), data));
	}
	void parse(std::wostream &stream, wstring templ_text, data_map &data)
	{
		token_vector tokens ;
		tokenize(templ_text, tokens) ;
		token_vector tree ;
		parse_tree(tokens, tree) ;

		for (size_t i = 0 ; i < tree.size() ; ++i)
		{
			// Recursively calls gettext on each node in the tree.
			// gettext returns the appropriate text for that node.
			// for text, itself;
			// for variable, substitution;
			// for control statement, recursively gets kids
			tree[i]->gettext(stream, data) ;
		}
	}
}
