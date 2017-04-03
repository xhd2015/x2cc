/*
 * LexicalParser_macros.h
 *
 *  Created on: 2017年3月26日
 *      Author: 13774
 */

#ifndef MACROS_LEXICALPARSER_MACROS_H_
#define MACROS_LEXICALPARSER_MACROS_H_


#include <LexicalParser.h>


bool LexicalParser::isInSet(char ch, int set) {
	return (set>=0 && set < LexicalParser::SET_SIZE)?LexicalParser::judgeArr[set](ch):false;
}

bool LexicalParser::isInAlpha(
		char ch) {
	return (ch>='a'&&ch<='z') || (ch>='A' && ch<='Z');
}

bool LexicalParser::isInWord(
		char ch) {
	return isInAlpha(ch) || isInDec(ch) || ch=='_';
}

bool LexicalParser::isInBin(
		char ch) {
	return ch=='1' || ch=='0';
}

bool LexicalParser::isInDec(
		char ch) {
	return ch>='0' && ch<='9';
}

bool LexicalParser::isInHex(
		char ch) {
	return isInDec(ch)||(ch>='a' && ch<='f')||(ch>='A' && ch<='F');
}
bool LexicalParser::isInSpace(char ch)
{
	return ch=='\n' || ch==' ' || ch=='\t';
}
bool LexicalParser::isInSpecial(char ch)
{
  return (ch=='<' || ch=='>' || ch=='#' || ch=='='|| ch=='(' || ch==')' || ch=='{' || ch=='}' ||
			    ch=='/' ||ch=='-' || ch=='+' || ch=='*' || ch=='[' || ch==']' || ch==',' ||ch==':' ||ch==';' || ch=='`' ||ch=='~'||
			    ch=='&' || ch=='^' || ch=='%' || ch=='$' || ch=='@'|| ch=='!' || ch=='.'||ch=='#' || ch=='?');
}
int LexicalParser::getCharVal(char ch)
{
  if(This::isInDec(ch))return ch-'0';
  else if(ch>='a' && ch<='z')return ch-'a'+10;
  else if(ch>='A' && ch<='Z')return ch-'A'+10;
}
//int LexicalParser::getCharVal(const char* str)
//{
//
//}
const std::map<int,std::string>& LexicalParser::getHumanReadable()
    {
      return This::humanInfo;
    }
//void	LexicalParser::skipSpaces(const char *buffer,size_t &index,size_t len)
//{
//  while(index < len && LexicalParser::isInSpace(buffer[index++]));//skip all spaces
//}

#endif /* MACROS_LEXICALPARSER_MACROS_H_ */
