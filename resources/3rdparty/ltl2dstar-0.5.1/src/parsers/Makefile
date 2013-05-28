all : nba-parser-lbtt.tab.cpp nba-parser-lbtt.lex.cpp  \
      nba-parser-promela.tab.cpp nba-parser-promela.lex.cpp 

nba-parser-lbtt.tab.cpp : nba-parser-lbtt.ypp 
	bison -d -t nba-parser-lbtt.ypp 

nba-parser-lbtt.lex.cpp : nba-parser-lbtt.lpp 
	flex -o nba-parser-lbtt.lex.cpp nba-parser-lbtt.lpp 

nba-parser-promela.tab.cpp : nba-parser-promela.ypp 
	bison -d -t nba-parser-promela.ypp 

nba-parser-promela.lex.cpp : nba-parser-promela.lpp 
	flex -o nba-parser-promela.lex.cpp nba-parser-promela.lpp 
