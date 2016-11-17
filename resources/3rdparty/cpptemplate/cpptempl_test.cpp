
#include "cpptempl.h"

#ifdef UNIT_TEST

#include <boost/test/unit_test.hpp>

#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE cpptemplTests
#endif

#pragma warning( disable : 4996 ) // doesn't like wcstombs

#include "unit_testing.h"

using namespace std ;

BOOST_AUTO_TEST_SUITE( TestCppData )

	using namespace cpptempl ;
	
	// DataMap
	BOOST_AUTO_TEST_CASE(test_DataMap_getvalue)
	{
		data_map items ;
		data_ptr data(new DataMap(items)) ;
		BOOST_CHECK_THROW( data->getvalue(), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataMap_getlist_throws)
	{
		data_map items ;
		data_ptr data(new DataMap(items)) ;

		BOOST_CHECK_THROW( data->getlist(), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataMap_getitem_throws)
	{
		data_map items ;
		items[L"key"] = data_ptr(new DataValue(L"foo")) ;
		data_ptr data(new DataMap(items)) ;

		BOOST_CHECK_EQUAL( data->getmap()[L"key"]->getvalue(), L"foo" ) ;
	}
	// DataList
	BOOST_AUTO_TEST_CASE(test_DataList_getvalue)
	{
		data_list items ;
		data_ptr data(new DataList(items)) ;

		BOOST_CHECK_THROW( data->getvalue(), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataList_getlist_throws)
	{
		data_list items ;
		items.push_back(make_data(L"bar")) ;
		data_ptr data(new DataList(items)) ;

		BOOST_CHECK_EQUAL( data->getlist().size(), 1u ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataList_getitem_throws)
	{
		data_list items ;
		data_ptr data(new DataList(items)) ;

		BOOST_CHECK_THROW( data->getmap(), TemplateException ) ;
	}
	// DataValue
	BOOST_AUTO_TEST_CASE(test_DataValue_getvalue)
	{
		data_ptr data(new DataValue(L"foo")) ;

		BOOST_CHECK_EQUAL( data->getvalue(), L"foo" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataValue_getlist_throws)
	{
		data_ptr data(new DataValue(L"foo")) ;

		BOOST_CHECK_THROW( data->getlist(), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataValue_getitem_throws)
	{
		data_ptr data(new DataValue(L"foo")) ;

		BOOST_CHECK_THROW( data->getmap(), TemplateException ) ;
	}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( TestCppParseVal )

	using namespace cpptempl ;
	BOOST_AUTO_TEST_CASE(test_quoted)
	{
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;
		data_ptr value = parse_val(L"\"foo\"", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"foo" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_value)
	{
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;
		data_ptr value = parse_val(L"foo", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"bar" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_not_found)
	{
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;
		data_ptr value = parse_val(L"kettle", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"{$kettle}" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_not_found_dotted)
	{
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;
		data_ptr value = parse_val(L"kettle.black", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"{$kettle.black}" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_my_ax)
	{
		data_map data ;
		data[L"item"] = make_data(L"my ax") ;
		BOOST_CHECK_EQUAL( parse_val(L"item", data)->getvalue(), L"my ax" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_list)
	{
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"bar")) ;
		data[L"foo"] = data_ptr(new DataList(items)) ;
		data_ptr value = parse_val(L"foo", data) ;

		BOOST_CHECK_EQUAL( value->getlist().size(), 1u ) ;
	}
	BOOST_AUTO_TEST_CASE(test_dotted)
	{
		data_map data ;
		data_map subdata ;
		subdata[L"b"] = data_ptr(new DataValue(L"c")) ;
		data[L"a"] = data_ptr(new DataMap(subdata)) ;
		data_ptr value = parse_val(L"a.b", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"c" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_double_dotted)
	{
		data_map data ;
		data_map sub_data ;
		data_map sub_sub_data ;
		sub_sub_data[L"c"] = data_ptr(new DataValue(L"d")) ;
		sub_data[L"b"] = data_ptr(new DataMap(sub_sub_data)) ;
		data[L"a"] = data_ptr(new DataMap(sub_data)) ;
		data_ptr value = parse_val(L"a.b.c", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"d" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_dotted_to_list)
	{
		data_list friends ;
		friends.push_back(make_data(L"Bob")) ;
		data_map person ;
		person[L"friends"] = make_data(friends) ;
		data_map data ;
		data[L"person"] = make_data(person) ;
		data_ptr value = parse_val(L"person.friends", data) ;

		BOOST_CHECK_EQUAL( value->getlist().size(), 1u ) ;
	}
	BOOST_AUTO_TEST_CASE(test_dotted_to_dict_list)
	{
		data_map bob ;
		bob[L"name"] = make_data(L"Bob") ;
		data_map betty ;
		betty[L"name"] = make_data(L"Betty") ;
		data_list friends ;
		friends.push_back(make_data(bob)) ;
		friends.push_back(make_data(betty)) ;
		data_map person ;
		person[L"friends"] = make_data(friends) ;
		data_map data ;
		data[L"person"] = make_data(person) ;
		data_ptr value = parse_val(L"person.friends", data) ;

		BOOST_CHECK_EQUAL( value->getlist()[0]->getmap()[L"name"]->getvalue(), L"Bob" ) ;
	}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( TestCppToken )

	using namespace cpptempl ;

	// TokenVar
	BOOST_AUTO_TEST_CASE(TestTokenVarType)
	{
		TokenVar token(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettype(), TOKEN_TYPE_VAR ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenVar)
	{
		token_ptr token(new TokenVar(L"foo")) ;
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"bar" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenVarCantHaveChildren)
	{
		TokenVar token(L"foo") ;
		token_vector children ;
		BOOST_CHECK_THROW(token.set_children(children), TemplateException) ;
	}
	// TokenText
	BOOST_AUTO_TEST_CASE(TestTokenTextType)
	{
		TokenText token(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettype(), TOKEN_TYPE_TEXT ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenText)
	{
		token_ptr token(new TokenText(L"foo")) ;
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"foo" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenTextCantHaveChildrenSet)
	{
		TokenText token(L"foo") ;
		token_vector children ;
		BOOST_CHECK_THROW(token.set_children(children), TemplateException) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenTextCantHaveChildrenGet)
	{
		TokenText token(L"foo") ;
		token_vector children ;
		BOOST_CHECK_THROW(token.get_children(), TemplateException) ;
	}
	// TokenFor
	BOOST_AUTO_TEST_CASE(TestTokenForBadSyntax)
	{
		BOOST_CHECK_THROW(TokenFor token(L"foo"), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForType)
	{
		TokenFor token(L"for item in items") ;
		BOOST_CHECK_EQUAL( token.gettype(), TOKEN_TYPE_FOR ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForTextEmpty)
	{
		token_ptr token(new TokenFor(L"for item in items")) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForTextOneVar)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		token_ptr token(new TokenFor(L"for item in items")) ;
		token->set_children(children) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first ")); 
		items.push_back(make_data(L"second ")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"first second " ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForTextOneVarLoop)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"loop.index"))) ;
		token_ptr token(new TokenFor(L"for item in items")) ;
		token->set_children(children) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first ")); 
		items.push_back(make_data(L"second ")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"12" ) ;
	}	
	BOOST_AUTO_TEST_CASE(TestTokenForLoopTextVar)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"loop.index"))) ;
		children.push_back(token_ptr(new TokenText(L". "))) ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		children.push_back(token_ptr(new TokenText(L" "))) ;
		token_ptr token(new TokenFor(L"for item in items")) ;
		token->set_children(children) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first")); 
		items.push_back(make_data(L"second")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"1. first 2. second " ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForLoopTextVarDottedKeyAndVal)
	{
		TokenFor token(L"for friend in person.friends") ;
		BOOST_CHECK_EQUAL( token.m_key, L"person.friends" ) ;
		BOOST_CHECK_EQUAL( token.m_val, L"friend" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForLoopTextVarDotted)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"loop.index"))) ;
		children.push_back(token_ptr(new TokenText(L". "))) ;
		children.push_back(token_ptr(new TokenVar(L"friend.name"))) ;
		children.push_back(token_ptr(new TokenText(L" "))) ;
		token_ptr token(new TokenFor(L"for friend in person.friends")) ;
		token->set_children(children) ;

		data_map bob ;
		bob[L"name"] = make_data(L"Bob") ;
		data_map betty ;
		betty[L"name"] = make_data(L"Betty") ;
		data_list friends ;
		friends.push_back(make_data(bob)) ;
		friends.push_back(make_data(betty)) ;
		data_map person ;
		person[L"friends"] = make_data(friends) ;
		data_map data ;
		data[L"person"] = make_data(person) ;

		BOOST_CHECK_EQUAL( gettext(token, data), L"1. Bob 2. Betty " ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForTextOneText)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		token_ptr token(new TokenFor(L"for item in items")) ;
		token->set_children(children) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first ")); 
		items.push_back(make_data(L"second ")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"{--}{--}" ) ;
	}

	//////////////////////////////////////////////////////////////////////////
	// TokenIf
	//////////////////////////////////////////////////////////////////////////

	BOOST_AUTO_TEST_CASE(TestTokenIfType)
	{
		TokenIf token(L"if items") ;
		BOOST_CHECK_EQUAL( token.gettype(), TOKEN_TYPE_IF ) ;
	}
	// if not empty
	BOOST_AUTO_TEST_CASE(TestTokenIfTrueText)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		token_ptr token(new TokenIf(L"if item")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"{--}" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfTrueVar)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		token_ptr token(new TokenIf(L"if item")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"foo" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfFalse)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		token_ptr token(new TokenIf(L"if item")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"") ;
	}

	

	// ==
	BOOST_AUTO_TEST_CASE(TestTokenIfEqualsTrue)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		token_ptr token(new TokenIf(L"if item == \"foo\"")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"foo" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfEqualsFalse)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		token_ptr token(new TokenIf(L"if item == \"bar\"")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfEqualsTwoVarsTrue)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		token_ptr token(new TokenIf(L"if item == foo")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"x") ;
		data[L"foo"] = make_data(L"x") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"x" ) ;
	}

	// !=
	BOOST_AUTO_TEST_CASE(TestTokenIfNotEqualsTrue)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		token_ptr token(new TokenIf(L"if item != \"foo\"")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfNotEqualsFalse)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		token_ptr token(new TokenIf(L"if item != \"bar\"")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"foo" ) ;
	}

	// not
	BOOST_AUTO_TEST_CASE(TestTokenIfNotTrueText)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		token_ptr token(new TokenIf(L"if not item")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"") ;
	}

	BOOST_AUTO_TEST_CASE(TestTokenIfNotFalseText)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		token_ptr token(new TokenIf(L"if not item")) ;
		token->set_children(children) ;
		data_map data ;
		data[L"item"] = make_data(L"") ;
		BOOST_CHECK_EQUAL( gettext(token, data), L"{--}") ;
	}

	// TokenEnd
	BOOST_AUTO_TEST_CASE(TestTokenEndFor)
	{
		TokenEnd token(L"endfor") ;
		BOOST_CHECK_EQUAL( token.gettype(), TOKEN_TYPE_ENDFOR ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenEndIf)
	{
		TokenEnd token(L"endif") ;
		BOOST_CHECK_EQUAL( token.gettype(), TOKEN_TYPE_ENDIF ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenEndIfCantHaveChildren)
	{
		TokenEnd token(L"endif") ;
		token_vector children ;
		BOOST_CHECK_THROW(token.set_children(children), TemplateException) ;
	}
	BOOST_AUTO_TEST_CASE(test_throws_on_gettext)
	{
		data_map data ;
		token_ptr token(new TokenEnd(L"endif")) ;

		BOOST_CHECK_THROW(gettext(token, data), TemplateException) ;
	}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( TestCppTokenize )

	using namespace cpptempl ;

	BOOST_AUTO_TEST_CASE(test_empty)
	{
		wstring text = L"" ;
		token_vector tokens ;
		tokenize(text, tokens) ;

		BOOST_CHECK_EQUAL( 0u, tokens.size() ) ;
	}
	BOOST_AUTO_TEST_CASE(test_text_only)
	{
		wstring text = L"blah blah blah" ;
		token_vector tokens ;
		tokenize(text, tokens) ;
		data_map data ;

		BOOST_CHECK_EQUAL( 1u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[0], data), L"blah blah blah" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_brackets_no_var)
	{
		wstring text = L"{foo}" ;
		token_vector tokens ;
		tokenize(text, tokens) ;
		data_map data ;

		BOOST_CHECK_EQUAL( 2u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[0], data), L"{" ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[1], data), L"foo}" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_ends_with_bracket)
	{
		wstring text = L"blah blah blah{" ;
		token_vector tokens ;
		tokenize(text, tokens) ;
		data_map data ;

		BOOST_CHECK_EQUAL( 2u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[0], data), L"blah blah blah" ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[1], data), L"{" ) ;
	}
	// var
	BOOST_AUTO_TEST_CASE(test_var)
	{
		wstring text = L"{$foo}" ;
		token_vector tokens ;
		tokenize(text, tokens) ;
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;

		BOOST_CHECK_EQUAL( 1u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[0], data), L"bar" ) ;
	}
	// for
	BOOST_AUTO_TEST_CASE(test_for)
	{
		wstring text = L"{% for item in items %}" ;
		token_vector tokens ;
		tokenize(text, tokens) ;

		BOOST_CHECK_EQUAL( 1u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( tokens[0]->gettype(), TOKEN_TYPE_FOR ) ;
	}
	BOOST_AUTO_TEST_CASE(test_for_full)
	{
		wstring text = L"{% for item in items %}{$item}{% endfor %}" ;
		token_vector tokens ;
		tokenize(text, tokens) ;

		BOOST_CHECK_EQUAL( 3u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( tokens[0]->gettype(), TOKEN_TYPE_FOR ) ;
		BOOST_CHECK_EQUAL( tokens[1]->gettype(), TOKEN_TYPE_VAR ) ;
		BOOST_CHECK_EQUAL( tokens[2]->gettype(), TOKEN_TYPE_ENDFOR ) ;
	}
	BOOST_AUTO_TEST_CASE(test_for_full_with_text)
	{
		wstring text = L"{% for item in items %}*{$item}*{% endfor %}" ;
		token_vector tokens ;
		tokenize(text, tokens) ;
		data_map data ;
		data[L"item"] = make_data(L"my ax") ;

		BOOST_CHECK_EQUAL( 5u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( tokens[0]->gettype(), TOKEN_TYPE_FOR ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[1], data), L"*" ) ;
		BOOST_CHECK_EQUAL( tokens[2]->gettype(), TOKEN_TYPE_VAR ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[2], data), L"my ax" ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[3], data), L"*" ) ;
		BOOST_CHECK_EQUAL( tokens[4]->gettype(), TOKEN_TYPE_ENDFOR ) ;
	}
	// if
	BOOST_AUTO_TEST_CASE(test_if)
	{
		wstring text = L"{% if foo %}" ;
		token_vector tokens ;
		tokenize(text, tokens) ;

		BOOST_CHECK_EQUAL( 1u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( tokens[0]->gettype(), TOKEN_TYPE_IF ) ;
	}
	BOOST_AUTO_TEST_CASE(test_if_full)
	{
		wstring text = L"{% if item %}{$item}{% endif %}" ;
		token_vector tokens ;
		tokenize(text, tokens) ;

		BOOST_CHECK_EQUAL( 3u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( tokens[0]->gettype(), TOKEN_TYPE_IF ) ;
		BOOST_CHECK_EQUAL( tokens[1]->gettype(), TOKEN_TYPE_VAR ) ;
		BOOST_CHECK_EQUAL( tokens[2]->gettype(), TOKEN_TYPE_ENDIF ) ;
	}
	BOOST_AUTO_TEST_CASE(test_if_full_with_text)
	{
		wstring text = L"{% if item %}{{$item}}{% endif %}" ;
		token_vector tokens ;
		tokenize(text, tokens) ;
		data_map data ;
		data[L"item"] = make_data(L"my ax") ;

		BOOST_CHECK_EQUAL( 5u, tokens.size() ) ;
		BOOST_CHECK_EQUAL( tokens[0]->gettype(), TOKEN_TYPE_IF ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[1], data), L"{" ) ;
		BOOST_CHECK_EQUAL( tokens[2]->gettype(), TOKEN_TYPE_VAR ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[2], data), L"my ax" ) ;
		BOOST_CHECK_EQUAL( gettext(tokens[3], data), L"}" ) ;
		BOOST_CHECK_EQUAL( tokens[4]->gettype(), TOKEN_TYPE_ENDIF ) ;
	}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( test_parse_tree )

	using namespace cpptempl ;

	token_ptr make_tt(wstring text)
	{
		return token_ptr(new TokenText(text)) ;
	}
	token_ptr make_for(wstring text)
	{
		return token_ptr(new TokenFor(text)) ;
	}
	token_ptr make_if(wstring text)
	{
		return token_ptr(new TokenIf(text)) ;
	}
	token_ptr make_endfor()
	{
		return token_ptr(new TokenEnd(L"endfor")) ;
	}
	token_ptr make_endif()
	{
		return token_ptr(new TokenEnd(L"endif")) ;
	}
	BOOST_AUTO_TEST_CASE(test_empty)
	{
		token_vector tokens ;
		token_vector tree ;
		parse_tree(tokens, tree) ;
		BOOST_CHECK_EQUAL( 0u, tree.size() ) ;
	}
	BOOST_AUTO_TEST_CASE(test_one)
	{
		token_vector tokens ;
		tokens.push_back(make_tt(L"foo")) ;
		token_vector tree ;
		parse_tree(tokens, tree) ;
		BOOST_CHECK_EQUAL( 1u, tree.size() ) ;
	}
	BOOST_AUTO_TEST_CASE(test_for)
	{
		token_vector tokens ;
		tokens.push_back(make_for(L"for item in items")) ;
		tokens.push_back(make_tt(L"foo")) ;
		tokens.push_back(make_endfor()) ;
		token_vector tree ;
		parse_tree(tokens, tree) ;
		BOOST_CHECK_EQUAL( 1u, tree.size() ) ;
		BOOST_CHECK_EQUAL( 1u, tree[0]->get_children().size()) ;
	}
	BOOST_AUTO_TEST_CASE(test_if)
	{
		token_vector tokens ;
		tokens.push_back(make_if(L"if insane")) ;
		tokens.push_back(make_tt(L"foo")) ;
		tokens.push_back(make_endif()) ;
		token_vector tree ;
		parse_tree(tokens, tree) ;
		BOOST_CHECK_EQUAL( 1u, tree.size() ) ;
		BOOST_CHECK_EQUAL( 1u, tree[0]->get_children().size()) ;
	}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TestCppParse)

	using namespace cpptempl ;

	BOOST_AUTO_TEST_CASE(test_empty)
	{
		wstring text = L"" ;
		data_map data ;
		wstring actual = parse(text, data) ;
		wstring expected = L"" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_no_vars)
	{
		wstring text = L"foo" ;
		data_map data ;
		wstring actual = parse(text, data) ;
		wstring expected = L"foo" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_var)
	{
		wstring text = L"{$foo}" ;
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;
		wstring actual = parse(text, data) ;
		wstring expected = L"bar" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_var_surrounded)
	{
		wstring text = L"aaa{$foo}bbb" ;
		data_map data ;
		data[L"foo"] = make_data(L"---") ;
		wstring actual = parse(text, data) ;
		wstring expected = L"aaa---bbb" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_for)
	{
		wstring text = L"{% for item in items %}{$item}{% endfor %}" ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"0")) ;
		items.push_back(make_data(L"1")) ;
		data[L"items"] = make_data(items) ;
		wstring actual = parse(text, data) ;
		wstring expected = L"01" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_if_false)
	{
		wstring text = L"{% if item %}{$item}{% endif %}" ;
		data_map data ;
		data[L"item"] = make_data(L"") ;
		wstring actual = parse(text, data) ;
		wstring expected = L"" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_if_true)
	{
		wstring text = L"{% if item %}{$item}{% endif %}" ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		wstring actual = parse(text, data) ;
		wstring expected = L"foo" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_nested_for)
	{
		wstring text = L"{% for item in items %}{% for thing in things %}{$item}{$thing}{% endfor %}{% endfor %}" ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"0")) ;
		items.push_back(make_data(L"1")) ;
		data[L"items"] = make_data(items) ;
		data_list things ;
		things.push_back(make_data(L"a")) ;
		things.push_back(make_data(L"b")) ;
		data[L"things"] = make_data(things) ;
		wstring actual = parse(text, data) ;
		wstring expected = L"0a0b1a1b" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_nested_if_false)
	{
		wstring text = L"{% if item %}{% if thing %}{$item}{$thing}{% endif %}{% endif %}" ;
		data_map data ;
		data[L"item"] = make_data(L"aaa") ;
		data[L"thing"] = make_data(L"") ;
		wstring actual = parse(text, data) ;
		wstring expected = L"" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_nested_if_true)
	{
		wstring text = L"{% if item %}{% if thing %}{$item}{$thing}{% endif %}{% endif %}" ;
		data_map data ;
		data[L"item"] = make_data(L"aaa") ;
		data[L"thing"] = make_data(L"bbb") ;
		wstring actual = parse(text, data) ;
		wstring expected = L"aaabbb" ;
		BOOST_CHECK_EQUAL( expected, actual ) ;
	}
	BOOST_AUTO_TEST_CASE(test_usage_example)
	{
		wstring text = L"{% if item %}{$item}{% endif %}\n"
			L"{% if thing %}{$thing}{% endif %}" ;
		cpptempl::data_map data ;
		data[L"item"] = cpptempl::make_data(L"aaa") ;
		data[L"thing"] = cpptempl::make_data(L"bbb") ;

		wstring result = cpptempl::parse(text, data) ;

		wstring expected = L"aaa\nbbb" ;
		BOOST_CHECK_EQUAL( result, expected ) ;
	}
	BOOST_AUTO_TEST_CASE(test_syntax_if)
	{
		wstring text = L"{% if person.name == \"Bob\" %}Full name: Robert{% endif %}" ;
		data_map person ;
		person[L"name"] = make_data(L"Bob") ;
		person[L"occupation"] = make_data(L"Plumber") ;
		data_map data ;
		data[L"person"] = make_data(person) ;

		wstring result = cpptempl::parse(text, data) ;

		wstring expected = L"Full name: Robert" ;
		BOOST_CHECK_EQUAL( result, expected ) ;
	}
	BOOST_AUTO_TEST_CASE(test_syntax_dotted)
	{
		wstring text = L"{% for friend in person.friends %}"
			L"{$loop.index}. {$friend.name} "
			L"{% endfor %}" ;

		data_map bob ;
		bob[L"name"] = make_data(L"Bob") ;
		data_map betty ;
		betty[L"name"] = make_data(L"Betty") ;
		data_list friends ;
		friends.push_back(make_data(bob)) ;
		friends.push_back(make_data(betty)) ;
		data_map person ;
		person[L"friends"] = make_data(friends) ;
		data_map data ;
		data[L"person"] = make_data(person) ;

		wstring result = cpptempl::parse(text, data) ;

		wstring expected = L"1. Bob 2. Betty " ;
		BOOST_CHECK_EQUAL( result, expected ) ;
	}
	BOOST_AUTO_TEST_CASE(test_example_okinawa)
	{
		// The text template
		wstring text = L"I heart {$place}!" ;
		// Data to feed the template engine
		cpptempl::data_map data ;
		// {$place} => Okinawa
		data[L"place"] = cpptempl::make_data(L"Okinawa");
		// parse the template with the supplied data dictionary
		wstring result = cpptempl::parse(text, data) ;

		wstring expected = L"I heart Okinawa!" ;
		BOOST_CHECK_EQUAL( result, expected ) ;
	}
	BOOST_AUTO_TEST_CASE(test_example_ul)
	{
		wstring text = L"<h3>Locations</h3><ul>"
			L"{% for place in places %}"
			L"<li>{$place}</li>"
			L"{% endfor %}"
			L"</ul>" ;

		// Create the list of items
		cpptempl::data_list places;
		places.push_back(cpptempl::make_data(L"Okinawa"));
		places.push_back(cpptempl::make_data(L"San Francisco"));
		// Now set this in the data map
		cpptempl::data_map data ;
		data[L"places"] = cpptempl::make_data(places);
		// parse the template with the supplied data dictionary
		wstring result = cpptempl::parse(text, data) ;
		wstring expected = L"<h3>Locations</h3><ul>"
			L"<li>Okinawa</li>"
			L"<li>San Francisco</li>"
			L"</ul>" ;
		BOOST_CHECK_EQUAL(result, expected) ;
	}
BOOST_AUTO_TEST_SUITE_END()

#endif