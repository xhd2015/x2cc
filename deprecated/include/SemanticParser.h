/*
 * SematicParser.h
 *
 *  Created on: 2017年3月29日
 *      Author: 13774
 */

#ifndef SEMANTICPARSER_H_
#define SEMANTICPARSER_H_


#include <LexicalParser.h>
#include <List.h>  //for Tree
#include <vector>
#include <string>
#include <utility>

class FindReplace{
public:
  FindReplace();
  ~FindReplace();
};

class LL1Macro{
public:
  typedef LL1Macro	This;
  typedef LexicalParser::WordStream WordStream;
  enum{
    TYPE_DEFINE=LexicalParser::TYPE_ID,
    TYPE_SPACE=LexicalParser::TYPE_SPACE,
    TYPE_ID=LexicalParser::TYPE_ID,
    TYPE_NEWLINE=LexicalParser::TYPE_NEWLINE,
    TYPE_END=LexicalParser::TYPE_END,
  };
  enum{
    CHAR_EOF=0
  };
public:
  LL1Macro()=default;
  LL1Macro(const WordStream& s,WordStream::size_type start);
  ~LL1Macro();


  void SPACES();
  void HASARG();
  void HASARG_A();
  void S();
  void S_A();
  void S_AA();
  void MARG();
  void MARGS();
  void MARGS_A();
  void BODY();

  void matchType(int word);
  void matchSingle(char ch);
  AS_MACRO char getCurChar()const;
  AS_MACRO int 	getCurType()const;//using getCurType() will always ensure to be right
  AS_MACRO void moveNext();
  AS_MACRO void movePrevious();
  void error();
public:
  const WordStream &s;
  const WordStream::size_type len;
  WordStream::size_type index;

};

//a find-replace
class SemanticParser{
public:
  SemanticParser()=default;
  ~SemanticParser();

  void defineSyntax(int type);
  void constructSyntaxTree();
//  void
protected:


};

//============function macros
void LL1Macro::moveNext()
{
  if(index<len)index++;
}
char LL1Macro::getCurChar()const
{
  return index<len?s[index].first[0]:CHAR_EOF;
}
 int 	LL1Macro::getCurType()const
 {
   return index<len?s[index].second:TYPE_END;
 }

#endif /* SEMANTICPARSER_H_ */
