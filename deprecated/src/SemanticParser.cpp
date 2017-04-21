

#include <List.h>  //for Tree
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <cstdio>

#include <SemanticParser.h>



LL1Macro::LL1Macro(const LL1Macro::WordStream& s,LL1Macro::WordStream::size_type start):
s(s),index(start),len(s.size())
{

}
LL1Macro::~LL1Macro()
{

}


void LL1Macro::SPACES()
{
  printf("matching SPACES\n");
  int curType=getCurType();
  if(curType==TYPE_SPACE)
    {
      matchType(TYPE_SPACE);
      SPACES();
    }else{
	return;
    }
}
void LL1Macro::HASARG()
{
  printf("matching HASARG\n");
  if(getCurType()==LexicalParser::TYPE_LBRACE){
    matchType(LexicalParser::TYPE_LBRACE);
    SPACES();
    HASARG_A();
    matchType(LexicalParser::TYPE_RBRACE);
  }else{
      error();
  }
}
void LL1Macro::HASARG_A()
{
  printf("matching HASARG_A\n");
  if(getCurType()==TYPE_SPACE||getCurType()==TYPE_ID)
    {
      MARGS();
    }else if(getCurType()==LexicalParser::TYPE_RBRACE){
	//pass
    }else{
	error();
    }
}
void LL1Macro::S()
{
  printf("matching S\n");
  if(getCurType()==LexicalParser::TYPE_S1)
    {
     matchType(LexicalParser::TYPE_S1);
    SPACES();
    this->matchType(TYPE_DEFINE);
    SPACES();
    this->matchType(TYPE_ID);
    SPACES();
    S_A();
    BODY();
    S_AA();
    }
  else{
      error();
  }
}
void LL1Macro::S_A()
{
  printf("matching S_A\n");
  if(getCurType()==LexicalParser::TYPE_LBRACE)
    {
      HASARG();
      SPACES();
    }else{
	//pass
    }
}
void LL1Macro::S_AA()
{
  if(getCurType()==TYPE_NEWLINE)
    {
      matchType(TYPE_NEWLINE);
    }else if(getCurType()==TYPE_END)
      {
	//pass
      }else{
	  error();
      }
}
void LL1Macro::MARG()
{
  printf("matching MARG\n");
  if(getCurType()==TYPE_SPACE || getCurType()==TYPE_ID)
    {
      SPACES();
      this->matchType(TYPE_ID);
      SPACES();
    }
//  else if(getCurChar()==')'||getCurChar()==','){
//	   }
  else{
	error();
    }
}
void LL1Macro::MARGS()
{
  printf("matching MARGS\n");
  if(getCurType()==LexicalParser::TYPE_LBRACE)
    {
      return;
    }
  else if(getCurType()==TYPE_SPACE ||
	  getCurType()==TYPE_ID
  ){
      MARG();
      MARGS_A();
  }else{
      error();
  }
}
void LL1Macro::MARGS_A()
{
  if(getCurType()==LexicalParser::TYPE_COMMA)
    {
      matchType(LexicalParser::TYPE_COMMA);
      MARGS();
    }else if(getCurType()==LexicalParser::TYPE_RBRACE)
      {
	//pass
      }else{
	  error();
      }
}
void LL1Macro::BODY()
{
  printf("matching body\n");
  while(getCurType()!=TYPE_END && getCurType()!=TYPE_NEWLINE)
    {
      moveNext();
    }
  //else  newline or eof
    return;
}

void LL1Macro::matchType(int word)
{
  printf("mathcing type : %s\n",LexicalParser::getTypeString(word).c_str());
  if(getCurType()==word){
      index++;
  }
  else {

      error();
  }
}
void LL1Macro::matchSingle(char ch)
{
  printf("matching single : %c\n",ch);
  if(s[index].first.size()==1 && getCurChar()==ch)
    {
      index++;
    }else{
	printf("UNEXPECTED : %c(%x), but EXPECTED : %c(%x)\n",getCurChar(),getCurChar(),ch,ch);
	error();
    }
}

void LL1Macro::error()
{
  std::cout<<"doesn't match at "<<index<<"\n";
}


