// This is a hack to support both Automake <= 1.11.x, and Automake >=
// 1.12.x The problem with is that old versions used to create
// parser.h, and parser.cxx from a parse.yxx grammar, while new
// versions create parser.hxx and parser.cxx.
//
// We want to support both version of Automake, because 1.11.x is fairly
// well distributed, and 1.12 did not make it into Debian 7.0.
//
// Yet it's difficult to support both versions because of the name
// change.  Our hack is to rename parse.yxx as parse.y, so that
// automake will generate rule to build parse.h and parse.c, and then
// this parser_.cxx file is used to compile parse.c in C++.  This way
// we always have a parse.h file regardless of the Automake version.
//
// We can fix this mess once Automake 1.12 is available everywhere.
#include "parser.c"
