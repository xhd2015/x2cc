
/*
 * LexicalParser.cpp
 *
 *  Created on: 2017?3?22?
 *      Author: 13774
 */

#include <LexicalParser.h>
#include <cstdio>
#include <cstdlib>
#       include      <cstdlib>
#include <iostream>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <macros/all.h>

using std::map;
using std::string;
using std::vector;
using std::pair;
/**
 * 	enum{
		SET_ALPHA,//a-z A-Z
		SET_WORD,//SET_ALPHA + _
		SET_BIN,
		SET_DEC,
		SET_HEX,
 */
const LexicalParser::JUDGE_CHAR LexicalParser::judgeArr[LexicalParser::SET_SIZE]={
		LexicalParser::isInAlpha,
		LexicalParser::isInWord,
		LexicalParser::isInBin,
		LexicalParser::isInDec,
		LexicalParser::isInHex,
		LexicalParser::isInSpace,
		LexicalParser::isInSpecial,
};
std::map<int,std::string> LexicalParser::humanInfo={
	{TYPE_OP,"OP"},
	{TYPE_NUMBER_DECIMAL,"NUMBER_DECIMAL"},
	{TYPE_NUMBER_HEX,"NUMBER_HEX"},
	{TYPE_NUMBER_BIN,"NUMBER_BIN"},
	{TYPE_ID,"ID"},
	{TYPE_STRING,"STRING"},
	{TYPE_CHAR,"CHAR"},
	{TYPE_NOTE,"NOTE"},
	{TYPE_STAR,"STAR"},
	{TYPE_NONE,"NONE"},
	{TYPE_UNKNOW,"UNKNOW"},
	{TYPE_ERROR,"ERROR"},
	{TYPE_EQ,"EQ"},
	{TYPE_UNRECOGNIZED,"UNRECOGNIZED"},
	{TYPE_AND,"AND"},
	{TYPE_SINGLE,"SINGLE"},
	{TYPE_SPACE,"SPACE"},

	{TYPE_PLUS,"PLUS"},
	{TYPE_SUB,"SUB"},
	{TYPE_STAR,"STAR"},
	{TYPE_DIV,"DIV"},

	{TYPE_LHBRACE,"LHBRACE"},
	{TYPE_RHBRACE,"RHBRACE"},
	{TYPE_LBRACE,"LBRACE"},
	{TYPE_RBRACE,"RBRACE"},
	{TYPE_LSQ,"LSQ"},
	{TYPE_RSQ,"RSQ"},

	{TYPE_SPACE,"SPACE"},
	{TYPE_NEWLINE,"NEWLINE"},
	{TYPE_EQ,"EQ"},
	{TYPE_COMMA,"COMMA"},
	{TYPE_COLON,"COLON"},
	{TYPE_SEMICOLON,"SEMICOLON"},
	{TYPE_GREATER,"GREATER"},
	{TYPE_LESS,"LESS"},
	{TYPE_ASK,"ASK"},
	{TYPE_NOT,"NOT"},
	{TYPE_DOLLAR,"DOLLAR"},
	{TYPE_EXP,"EXP"},
	{TYPE_AND,"AND"},
	{TYPE_S1,"S1"},
	{TYPE_AT,"AT"},
	{TYPE_WAVE,"WAVE"},
	{TYPE_RQUOTE,"RQUOTE"},
	{TYPE_QUOTE,"QUOTE"},
	{TYPE_DQUOTE,"DQUOTE"},

	{TYPE_POINT,"POINT"},
	{TYPE_UNDERSCORE,"UNDERSCORE"},

	{TYPE_ERECT,"ERECT"},
	{TYPE_TRANSFER,"TRANSFER"},

	{TYPE_END,"END"},
	{TYPE_INFO_LOST,"INFO_LOST"},
};
LexicalParser::CharType	LexicalParser::charType={
    {'+',TYPE_PLUS},
    {'-',TYPE_SUB},
    {'*',TYPE_STAR},
    {'/',TYPE_DIV},

    {'{',TYPE_LHBRACE},
    {'}',TYPE_RHBRACE},
    {'(',TYPE_LBRACE},
    {')',TYPE_RBRACE},
    {'[',TYPE_LSQ},
    {']',TYPE_RSQ},

    {' ',TYPE_SPACE},
    {'\t',TYPE_SPACE},
    {'\n',TYPE_NEWLINE},
    {'=',TYPE_EQ},
    {',',TYPE_COMMA},
    {':',TYPE_COLON},
    {';',TYPE_SEMICOLON},
    {'>',TYPE_GREATER},
    {'<',TYPE_LESS},
    {'?',TYPE_ASK},
    {'!',TYPE_NOT},
    {'$',TYPE_DOLLAR},
    {'^',TYPE_EXP},
    {'&',TYPE_AND},
    {'#',TYPE_S1},
    {'@',TYPE_AT},
    {'~',TYPE_WAVE},
    {'`',TYPE_RQUOTE},
    {'\'',TYPE_QUOTE},
    {'"',TYPE_DQUOTE},

    {'.',TYPE_POINT},
    {'_',TYPE_UNDERSCORE},

    {'|',TYPE_ERECT},
    {'\\',TYPE_TRANSFER},
};


LexicalParser::LexicalParser() {
	// TODO Auto-generated constructor stub

}

LexicalParser::~LexicalParser() {
	// TODO Auto-generated destructor stub
}
bool LexicalParser::isDigitType(char ch,int type)
{
	return This::tellDigitType(ch)<=type;
}
int LexicalParser::tellDigitType(char ch)
{
	if(ch=='0' || ch=='1')
		return DIGIT_BIN;
	if(ch>='2' && ch<='9')
		return DIGIT_DECIMAL;
	if(ch>='a' && ch<='f')
		return DIGIT_HEX_LOWER;
	if(ch>='A' && ch<='F')
		return DIGIT_HEX_UPPER;
	return DIGIT_UNKNOWN;
}
bool LexicalParser::isInSet(char ch, const char* buffer, size_t len)
{
	for(int i=0;i<(int)len;i++)
	{
		if(ch==buffer[i])return true;
	}
	return false;
}

//int LexicalParser::evalString(const std::string& s,int type)
//{
//  int base;
//  switch(type)
//  {
//    case TYPE_NUMBER_BIN:
//      base=2;break;
//    case TYPE_NUMBER_HEX:
//      base=16;break;
//    case TYPE_NUMBER_DECIMAL:
//      base=10;break;
//    default:
//      return 0;
//  }
////  std::cout<<s<<std::endl;
//  int val=0;
//  for(std::string::const_iterator it=s.begin(),itend=s.end();it!=itend;it++)
//    {
//      if(This::isInDec(*it))val = base*val + ((*it)-'0');
//      else if(*it>='a' && *it<='f') val=base*val + ((*it)-'a');
//      else if(*it >='A' && *it<='F') val=base*val + ((*it)-'A');
//      else break;
//    }
////  std::cout << "very"<<std::endl;
////  std::cout << val <<std::endl;
//  return val;
//}
//int LexicalParser::evalString(const char* buffer,size_t len,int type)
//{
//  return This::evalString(std::string(buffer,len), type);
//}
char	LexicalParser::findValue(char key,const char* buffer,size_t len)
{
	for(int i=0;i<(int)len;i+=2)
	{
		if(buffer[i]==key)return buffer[i+1];
	}
	return 0;
}


//
//void LexicalParser::parseNumber(const char * buffer,size_t n)
//{
//	char bin_digit[]={'0','1'};
//	char dec_digit[]={'0','1','2','3','4','5','6','7','8','9'};
//	char hex_digit[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','A','B','C','D','E','F'};
//	enum{
//		STATE_START,
//		STATE_BIN,
//		STATE_DECIMAL,
//		STATE_HEX,
//		STATE_BIN_HEX_DEC
//	};
//	int curState=STATE_START;
//	int nextState;
//	char curInput;
//	int val=0;
//	int i;
//	for(i=0;i<n;i++)
//	{
//		curInput=buffer[i];
//		if(curState==STATE_START)
//		{
//			if(curInput=='0')
//			{
//				nextState=STATE_BIN_HEX_DEC;
//			}else if(curInput>='0' && curInput<='9'){
//				val = 10*val + curInput - '0';
//				nextState=STATE_DECIMAL;
//			}else
//				goto error;
//		}else if(curState==STATE_DECIMAL){
//			nextState=STATE_DECIMAL;
//			if(curInput>= '0' && curInput<= '9')
//			{
//				val = 10*val + curInput - '0';
//			}else
//				goto error;
//		}else if(curState==STATE_HEX){
//			nextState=STATE_HEX;
//			if(This::isDigitType(curInput, DIGIT_BIN)||
//					This::isDigitType(curInput, DIGIT_DECIMAL)
//			)
//			{
//				val = val*16 + curInput - '0';
//			}else if(This::isDigitType(curInput, DIGIT_HEX_LOWER))
//			{
//				val = val*16 + curInput - 'a'+10;
//			}else if(This::isDigitType(curInput,DIGIT_HEX_UPPER))
//			{
//				val=val*16+curInput - 'A'+10;
//			}else
//				goto error;
//		}else if(curState==STATE_BIN){
//			nextState=STATE_BIN;
//			if(This::isDigitType(curInput,DIGIT_BIN))
//			{
//				val = 2*val + curInput - '0';
//			}
//		}else if(curState==STATE_BIN_HEX_DEC){
//			if(curInput=='x')
//			{
//				nextState=STATE_HEX;
//			}else if(curInput=='b'){
//				nextState=STATE_BIN;
//			}else if(curInput>='0' || curInput<='9'){
//				val = val*10 + curInput-'0';
//				nextState=STATE_DECIMAL;
//			}
//		}
//		curState=nextState;
//	}
//	printf("input is %s,parsed is %d\n",buffer,val);
//
//	return;
//	error:
//		printf("error: i = %d , state = %d\n",i,curState);
//		This::error("error in parseNumber");
//}
/**
 * General purposed automata
 *
 *		typedef int (*GET_NEXT_STATE)(int,char)
 * 		int getNextState(int state,char ch);
 *
 * 		typedef void (*PROCESS_OUTPUT)(int state,char ch,char *buffer,size_t &bufindex)
 * 		void processOutput(int state,char ch,char *buffer,size_t &bufindex);
 *
 * 		void errorProcess(int errState,char ch);
 *
 *
 * 		parseElement(const char *buffer,size_t &index,size_t len,char *buf,size_t &bufi,
 * 		int &curState,
 * 		int errState,int endState,GET_NEXT_STATE getNextState,PROCESS_OUTPUT processOutput)
 * 		{
 * 			int i;
 * 			char ch;
 * 			int nextState;
 *			for(i=index;i<size_t;i++)
 *			{
 *				if(curState==errState)
 *				{
 *					return;
 *				}else if(curState==endState){
 *					return;
 *				}
 *				ch=buffer[i];
 *				nextState=getNextState(curState,ch);
 *				processOutput(curState,ch,buf,bufi);
 *				curState=nextState;
 *			}
 *
 *
 * 		}
 */
//void LexicalParser::parseString(const char* buffer, size_t& index, size_t len)
//{
//	const char transfer[]={'n','\n','b','\b','\\','\\','t','\t'};
//	enum{
//		STATE_START,STATE_SWALLOW,STATE_TRANSFER,STATE_END
//	};
//	char ch;
//	int i;
//	int curState=STATE_START,nextState;
//	char *buf=new char[100];
//	int bufi=0;
//	for(i=index;i<len;i++)
//	{
//		ch=buffer[i];
//		if(curState==STATE_START)
//		{
//			if(ch=='"')
//			{
//				nextState=STATE_SWALLOW;
//			}else{
//				goto error;
//			}
//		}else if(curState==STATE_SWALLOW){
//			if(ch=='"'){
//				buf[bufi++]=0;
//				nextState=STATE_END;
//			}
//			else if(ch=='\\')
//			{
//				nextState=STATE_TRANSFER;
//			}else{
//				buf[bufi++]=ch;
//			}
//		}else if(curState==STATE_TRANSFER){
//			if(ch=='\n'){
//				nextState=STATE_SWALLOW;
//			}else if(ch=='n' || ch=='t' || ch=='b'||ch=='\\'){
//				buf[bufi++]=This::findValue(ch, transfer,ARRSIZE(transfer));
//				nextState=STATE_SWALLOW;
//			}else{
//				goto error;//There is an error about transferring
//			}
//		}else if(curState==STATE_END){
//			break;
//		}
//		curState=nextState;
//	}
//	index +=i;
//	if(curState!=STATE_END)
//	{
//		This::error("input is not a valid string");
//	}else{
//		printf("input : %.*s, output \"%s\"(\" in the border is not part of it) ,index is at %d\n",len,buffer,buf,index);
//	}
//	delete [] buf;
//	return;
//	error:
//		This::error("error in parseString");
//}
/**
 * first loop,merge '\\' '\n' to NOTHING
 * 			 	merge [\s\t]* to NOTHING
 * 			 	merge \n to NOTHING if start is not #[\s\t\n]*define
 * 			 		else merge it into MACROEND
 * 			 	recognise '..'	to CHAR,take care of \'
 *
 * 			 	recognise "..." to STRING,take care of \"
 *
 */
void LexicalParser::parseWords(WordStream &stream,const char* buffer, size_t& index, size_t len)
{

	char ch;
	int i;
//	char *buf=new char[100];
//	int bufi=0;
	int curState=STATE_START,nextState=STATE_START,lastState;
	bool	prepP=false;
	int wordP=DOING_ONE_WROD;
	int curType;
	int val,times;
//	vector<char>	oneword={'<','>'}; //include algorithm for that
//	vector<char>	ignore={'\t','\ '};
	string currentOutput;
	for(i=index;i<=(int)len;i++)
	{
	    if(i==(int)len)ch=0;
	    else	ch=buffer[i];
	    if(wordP==DONE_ONE_WORD)wordP=DOING_ONE_WROD;
		switch(curState)
		{
		case STATE_START:
			if(This::isInAlpha(ch)||ch=='_')
			  {
			    currentOutput.push_back(ch);
			    nextState=STATE_ID;
			  }
			else if(This::isInSpecial(ch))
			{
			    if(ch=='#'){prepP=true;}
			   if(ch=='/'){
				nextState=STATE_NOTE;
//				printf("going note\n");
			    }else{
			      wordP=DONE_ONE_WORD;
			      curType=This::charType[ch];
			      currentOutput.push_back(ch);
			    }
			}else if(ch=='\r'){
			    //pass
			}else if(ch=='\t' || ch==' '){
			    if(prepP)//keep format information
			      {
				nextState=STATE_KEEP_SPACE;
				currentOutput.push_back(ch);
			      }else{

			      }
			    //pass
			}else if(ch=='\n'){
			    if(prepP)
			      {
				prepP=false;
				wordP=DONE_ONE_WORD;
				curType=TYPE_NEWLINE;
				currentOutput.push_back(ch);
			      }
			}else if(ch=='0'){
				currentOutput.push_back(ch);
				nextState=STATE_NUMBER;
			}else if(ch>='1' && ch<='9'){
				currentOutput.push_back(ch);
				nextState=STATE_DECILMAL_NUMBER;
			}else if(ch=='"'){
			    nextState=STATE_STRING;
			}else if(ch=='\''){
			    nextState=STATE_CHAR;
			}else if(ch==0){
			    i--;
			    nextState=STATE_END;
			}else if(ch=='\\'){
			    lastState=STATE_START;
			    nextState=STATE_TRANSFER_LANGUAGE;
			}else{
			    nextState=STATE_UNRECOGNIZED_CHAR;
			    currentOutput.push_back(ch);
			}
			break;
		case STATE_ID:
			if(This::isInWord(ch)){
				currentOutput.push_back(ch);//state not changed
			}else if(ch=='\\'){
			    lastState=STATE_ID;
			    nextState=STATE_TRANSFER_LANGUAGE;
			}else{
				i--;
				nextState=STATE_START;
				wordP=DONE_ONE_WORD;
				curType=TYPE_ID;
			}
			break;
		case STATE_KEEP_SPACE:
		      if(ch==' '||ch=='\t')currentOutput.push_back(ch);
		      else{
			  i--;nextState=STATE_START;wordP=DONE_ONE_WORD;curType=TYPE_SPACE;
		      }
		      break;
		case STATE_NUMBER:
//			printf("in number ch='%c',out=\"%s\"\n",ch,currentOutput.c_str());
			if(ch=='x' || ch=='X'){
				nextState=STATE_HEX_NUMBER;
			}else if(ch=='b' || ch=='B'){
				nextState=STATE_BIN_NUMBER;
			}else if(ch>='0' && ch<='9'){
				currentOutput.push_back(ch);
				nextState=STATE_DECILMAL_NUMBER;
			}else{
				i--;
				nextState=STATE_START;
				curType=TYPE_NUMBER_DECIMAL;
				wordP=DONE_ONE_WORD;
			}
			break;
		case STATE_DECILMAL_NUMBER:
			if(ch>='0' && ch<='9'){
				currentOutput.push_back(ch);
				nextState=STATE_DECILMAL_NUMBER;
			}else{
				i--;
				wordP=DONE_ONE_WORD;
				curType=TYPE_NUMBER_DECIMAL;
				nextState=STATE_START;
			}
			break;
		case STATE_HEX_NUMBER:
			if( (ch>='0' && ch<='9') || (ch>='a' && ch<='z')||(ch>='A'&&ch<='Z'))
			{
				currentOutput.push_back(ch);
			}else if(ch=='_'){
			      //pass
			}
			else{
				i--;
				nextState=STATE_START;
				curType=TYPE_NUMBER_HEX;
				wordP=DONE_ONE_WORD;
			}
			break;
		case STATE_BIN_NUMBER:
			if(ch=='0' || ch=='1'){
				currentOutput.push_back(ch);
			}else if(ch=='_'){
					//do nothing,as an extension, we support number like 0b0011_0100
			}else{
				i--;
				wordP=DONE_ONE_WORD;
				curType=TYPE_NUMBER_BIN;
				nextState=STATE_START;
			}
			break;
		case STATE_STRING:
//			printf("in string,current is '%c'\n",ch);
			if(ch=='"'){
				wordP=DONE_ONE_WORD;
				curType=TYPE_STRING;
				nextState=STATE_START;

			}
			else if(ch=='\\')
			{
//				printf("go here '\\'\n");
				lastState=curState;
				nextState=STATE_TRANSFER_STRING;
			}
			else
			{
				currentOutput.push_back(ch);
			}

			break;
		case STATE_TRANSFER_CARRIGE:
			if(ch=='\n')
			{
				nextState=lastState;
			}else{
				currentOutput.push_back(ch);
				nextState=lastState;
			}
			break;
		case STATE_TRANSFER_COMMON:
			if(ch=='\n')
			{
				nextState=lastState;
			}else if(ch=='x'){
				val=0;
				nextState=STATE_TRANSFER_COMMON_X1;
			}else if(ch=='u'){//unicode
				times=0;val=0;
				nextState=STATE_TRANSFER_COMMON_U1;
			}else{
			    i--;
			    nextState=STATE_TRANSFER_INVALID;
			}

			break;
		case STATE_TRANSFER_COMMON_X1:
			if(This::isInHex(ch)){val=This::getCharVal(ch);nextState=STATE_TRANSFER_COMMON_X2;}
			else {nextState=STATE_TRANSFER_HEX_NOT_ENOGUH;i--;}
			break;
		case STATE_TRANSFER_COMMON_X2:
			if(This::isInHex(ch)){val=16*val + This::getCharVal(ch);currentOutput.push_back((char)val);nextState=lastState;}
			else {nextState=STATE_TRANSFER_HEX_NOT_ENOGUH;i--;}
			break;
		case STATE_TRANSFER_COMMON_U1:
			 if(This::isInHex(ch)){val=16*val+This::getCharVal(ch);}
			 else{
			     nextState=STATE_TRANSFER_HEX_NOT_ENOGUH;i--;
			 }
			if(++times==4)
			  {
			    currentOutput.push_back((char)(val));
			    currentOutput.push_back((char)(val>>8));//high 8bits
			    nextState=lastState;
			  }
			break;
		case STATE_TRANSFER_LANGUAGE:
			if(This::isInSpecial(ch)||ch=='\''||ch=='"'){nextState=lastState;currentOutput.push_back(ch);}
			else {i--;nextState=STATE_TRANSFER_COMMON;}
			break;
		case STATE_TRANSFER_STRING:
			if(ch=='"'){nextState=lastState;wordP=DONE_ONE_WORD;currentOutput.push_back(ch);}
			else if(ch=='{')//macro expansion
			  {
			    nextState=lastState;
			    currentOutput.push_back(ch);
			  }
			else {i--;nextState=STATE_TRANSFER_VALUE;}
			break;
		case STATE_TRANSFER_CHAR:
			if(ch=='\''){nextState=lastState;wordP=DONE_ONE_WORD;currentOutput.push_back(ch);}
			else {i--;nextState=STATE_TRANSFER_VALUE;}
			break;
		case STATE_TRANSFER_VALUE:
			switch(ch)
			{
			  case 'n':
			    currentOutput.push_back('\n'); goto END_VALUE;
			  case 'a':
			    currentOutput.push_back('\a'); goto END_VALUE;
			  case 't':
			    currentOutput.push_back('\t'); goto END_VALUE;
			  case 'r':
			    currentOutput.push_back('\r'); goto END_VALUE;
			  default:
			     i--;
			     nextState=STATE_TRANSFER_COMMON;
			     goto NEXT_STATE;
			}
			END_VALUE:
			  nextState=lastState;
			NEXT_STATE:
			break;
			/* no break */
		case STATE_CHAR:
			if(ch=='\\')
			{
				curType=TYPE_CHAR;
				lastState=curState;
				nextState=STATE_TRANSFER_CHAR;
			}else if(ch=='\''){
				wordP=DONE_ONE_WORD;
				curType=TYPE_CHAR;
				nextState=STATE_START;
			}
			else
			{
				currentOutput.push_back(ch);
			}
			break;
		case STATE_NOTE:
			if(ch=='/'){nextState=STATE_LINE_NOTE;}
			else if(ch=='*')nextState=STATE_MULTILINE_NOTE;
			else {i--;nextState=STATE_START;currentOutput.push_back('/');curType=TYPE_SINGLE;wordP=DONE_ONE_WORD;}
			break;
		case STATE_LINE_NOTE:
			if(ch=='\r'){
			}//pass
			else if(ch=='\n' || ch==0){
				wordP=DONE_ONE_WORD;
				curType=TYPE_NOTE;
				nextState=STATE_START;//done
//				printf("end line note , nextState is %d\n",nextState);
			}else{
//				printf("push note %c(%x)\n",ch,ch);
				currentOutput.push_back(ch);
			}
			break;
		case STATE_MULTILINE_NOTE:
			if(ch=='*'){
				currentOutput.push_back(ch);
				nextState=STATE_MULTILINE_GOING_END;
			}else{
				currentOutput.push_back(ch);
			}
			break;
		case STATE_MULTILINE_GOING_END:
			if(ch=='/'){
				currentOutput.erase(currentOutput.size() - 1);
				wordP=DONE_ONE_WORD;
				curType=TYPE_NOTE;
				nextState=STATE_START;
			}else if(ch=='*'){
				currentOutput.push_back(ch);
			}else{
				currentOutput.push_back(ch);
				nextState=STATE_MULTILINE_NOTE;
			}
			break;
		case STATE_UNRECOGNIZED_CHAR:
		      wordP=DONE_ONE_WORD;
		      curType=TYPE_UNRECOGNIZED;
		      nextState=STATE_START;
		      i--;
		      break;
		case STATE_TRANSFER_INVALID:
		      wordP=DONE_ONE_WORD;

		      nextState=STATE_START;
		  break;
		case STATE_END:
		    //good end
		    wordP=DONE_ONE_WORD;//stop compliation
		    stream.push_back(pair<string,int>("",TYPE_END));
		    goto HARD_END;
		    break;
		default:
			//nothing
			printf("get an error, unexpected STATE: %d, ch is %c(%x)\n",curState,ch,ch);
			nextState=STATE_START;
			break;
		}
		if(wordP==DONE_ONE_WORD)
		{
			printf("(%s,%s)\n",currentOutput.c_str(),humanInfo[curType].c_str());
			stream.push_back(pair<string,int>(currentOutput,curType));
			currentOutput.clear();
			curType=TYPE_NONE;
//			printf("next state=%d\n",nextState);
		}
//		printf("setting curState %d \n",nextState);
		curState=nextState;
	}
	HARD_END:
	if(curState!=STATE_END && curState!=STATE_START && wordP!=DONE_ONE_WORD)//not done a word
	  {
	    std::cout << "get lexical error : unexpected EOF for ("<<humanInfo[curType]<<","<<currentOutput<<")"<<std::endl;
	  }

}
/**
 *
 */
//void LexicalParser::parseId(const char *buffer,size_t &index,size_t len,std::string& out,int &type)
//{
////	type=TYPE_NONE;
//	size_t &i=index;
//	char ch;
//	if(i>=len)return;
//	if(isInAlpha(ch=buffer[i])  || ch=='_')
//	{
//	    out.push_back(ch);
//	      i++;
//	      while(i < len )
//		{
//		  if(isInWord(ch=buffer[i]))
//		    {out.push_back(ch);i++;}
//		  else if(len - i>=1 && buffer[i]=='\\' && buffer[i+1]=='\n')i+=2;
//		}
//	      type=TYPE_ID;
//	}else{
//	    type=TYPE_ERROR;//not start with valid ID
//	}
//
//}
//void LexicalParser::parseNumber(const char *buffer,size_t &index,size_t len,std::string &out,int &type)
//{
//	size_t &i=index;
//	if(i>=len)return;
//	if(!isInDec(buffer[i]))return;
//	if(buffer[i]!='0'){out.push_back(buffer[i]);type=TYPE_NUMBER_DECIMAL;i++;}
//	else if(len - i >= 2)
//	{
//			i++;
//			switch(buffer[i])
//			{
//			case 'x':
//			case 'X':
//				type=TYPE_NUMBER_HEX;
//				i++;
//				break;
//			case 'b':
//			case 'B':
//				type=TYPE_NUMBER_BIN;
//				i++;
//				break;
//			default:
//				if(!isInDec(buffer[i]))
//				{
//					type=TYPE_NONE;
//					return;
//				}else{
//					type=TYPE_NUMBER_DECIMAL;
//					out.push_back(buffer[i]);
//					i++;
//				}
//				break;
//			}
//	}
//	int set;
//	if(type==TYPE_NUMBER_DECIMAL)set=SET_DEC;
//	else if(type==TYPE_NUMBER_BIN)set=SET_BIN;
//	else if(type==TYPE_NUMBER_HEX)set=SET_HEX;
//
//	while(i<len)
//		if(isInSet(buffer[i],set))
//				out.push_back(buffer[i++]);
//		else if(buffer[i] == '_')
//				i++;
//		else
//		  break;
//
//}

//void
//LexicalParser::parseTransferValue (const char* buffer, size_t& index,
//				   size_t len, std::string& out, int& type)
//{
////  std::cout<<"parseTransferValue"<<std::endl;
//  size_t &i=index; if(i>=len)return;
//  if(buffer[i]!='\\' || len - i < 2)return;
//  i++;
//  if( (type==TYPE_STRING && buffer[i]=='"')||
//      (type==TYPE_CHAR && buffer[i]=='\''))
//	{out.push_back(buffer[i++]);return;}
//  else { i--;this->parseTransferAny(buffer, i, len, out, type);}
//}
//
//void
//LexicalParser::parseTransferAny (const char* buffer, size_t& index, size_t len,
//				 std::string& out, int& type)
//{
//  size_t &i=index; if(i>=len)return;
//  if(buffer[i]=='\\')
//    {
//      i++;
//      if(i>=len){type=TYPE_ERROR;return;}
//      switch(buffer[i])
//      {
//	case '\n':
//	  i++;return;
//	  break;
//	case 'x':
//	  i++;
//	  if(len - i < 2 || !This::isInHex(buffer[i]) || !This::isInHex(buffer[i+1])){type=TYPE_ERROR;return;}
//	  out.push_back((char)This::evalString(buffer+i,2, TYPE_NUMBER_HEX));
////	  std::cout<<"OK"<<std::endl;
//	  i+=2;
//	  break;
//	case '\\':
//	  out.push_back(buffer[i]);i++;break; // \  the same in three conditions
//	default:
//	  break;
//      }
//    }
//}

//void
//LexicalParser::parseString (const char* buffer, size_t& index, size_t len,
//			    std::string& out, int& type)
//{
////  std::cout<<"parseString"<<std::endl;
//  size_t &i=index;if(i>=len)return;
//  if(buffer[i]!='"')return;
//  i++;
//  type=TYPE_STRING;
//  while( i<len && buffer[i]!='"')
//    {
//      if(buffer[i]=='\\')
//	{
//	  this->parseTransferValue(buffer, i, len, out, type);
//	  if(type==TYPE_ERROR)return;
//	}else out.push_back(buffer[i++]);
//    }
//  if(i==len){type=TYPE_ERROR;}//no matching "
//}
/**
 *  / is the leading character of notation,
 *  however this has arisen some problems,such as that we can never use / as another leading character again.
 *
 */
//void
//LexicalParser::parseNote(const char *buffer,size_t &index,size_t len,std::string &out,int &type)
//{
//  size_t &i=index;if(i>=len)return;
//  if(buffer[i]!='/')return;
//  i++;if(i>=len){type=TYPE_ERROR;return;}
//
//  type=TYPE_NOTE;
//  bool goodEnd=false;
//  if(buffer[i]=='/')//single line
//    {
//      i++;
//      while(i<len && buffer[i]!='\n')//never error
//	{
////	  std::cout<<buffer[i]<<std::endl;
//	  if(buffer[i]=='\\' && len - i>1 && buffer[i+1]=='\n')i+=2;
//	  else out.push_back(buffer[i++]);
//	}
//    }else if(buffer[i]=='*'){//multiline
//	i++;
//	while(i<len)
//	  {
////	    std::cout<<buffer[i]<<std::endl;
//	    if(buffer[i]=='*')
//	     {
//	      if(i>=len){type=TYPE_ERROR;return;}
//	      else if(buffer[i+1]=='/'){goodEnd=true;break;}
//	      else out.push_back(buffer[i++]);
//	    }else if(buffer[i]=='\\' && len -i>1 && buffer[i+1]=='\n')i+=2;
//	    else out.push_back(buffer[i++]);
//	  }
//	if(!goodEnd)type=TYPE_ERROR;
//    }else{
//	type=TYPE_ERROR;
//    }
//}

//void
//LexicalParser::parsePreps (const char* buffer, size_t& index, size_t len)
//{
//}

void LexicalParser::error(const char* msg) const
{
	printf(" [ ERROR ]: %s\n",msg);
}

//===========class : PrinterDebugger

PrintDebugger::PrintDebugger():
		doOut(true)
{
}

PrintDebugger::~PrintDebugger() {
}
void PrintDebugger::setDoOutput(bool doOut)
{
	this->doOut=doOut;
}

void PrintDebugger::info(const char* str) const {
}

//==========class LexicalPrep
//void LexicalPrep::parse(const char *buffer,size_t len,size_t &index)
//{
//  int curState=This::STATE_START,nextState;
//  size_t &i = index;
//  while(i<len)
//    {
//
//      curState=this->transMap[]
//    }
//}

//LexicalPrep::LexicalPrep (int i)
//{
//  //======STATE_START
//  this->transMap[STATE_START]['\\']=STATE_TRANSFER;
//  this->transMap[STATE_START][SET_SPACE]=STATE_START;
//  this->transMap[STATE_START]['#']=STATE_ID_START;
//
//  //=====STATE_DIRECTIVE
//  this->transMap[STATE_DIRECTIVE_START][SET_WORD_START]=STATE_DIRECTIVE_END;
//  this->transMap[STATE_DIRECTIVE_END][SET_WORD_END]
//
//  //=====STATE_COMMON_TRANSFER
//  this->transMap[STATE_TRANSFER]['\n']=STATE_LAST;
//
//  //=====STATE_ID
//  this->transMap[STATE_ID_START][SET_WORD_START]=STATE_ID_END;
//  this->transMap[STATE_ID_END][SET_WORD_END]=STATE_ID_END;
//  this->transMap[STATE_ID_START][SET_NON_WORD]=STATE_START;
//  this->transMap[STATE_ID_END][SET_NON_WORD]=STATE_START;
//
//
//
//
//
//
//  for(char ch='a';ch<='z';ch++)this->transMap[STATE_START][ch]=STATE_ID;
//  for(char ch='A';ch<='Z';ch++)this->transMap[STATE_START][ch]=STATE_ID;
//  for(char ch='0';ch<='9';ch++)this->transMap[STATE_START][ch]=STATE_NUMBER;
//
//  this->transMap[STATE_START][' ']=STATE_START;
//  this->transMap[STATE_START][' ']=STATE_START;
//  this->transMap[STATE_START][' ']=STATE_START;
//  this->transMap[STATE_START][' ']=STATE_START;
//  this->transMap[STATE_START][' ']=STATE_START;
//
//}
//
//int	LexicalPrep::getNextState(int curState,char ch)
//{
//  switch(curState)
//  {
//    case STATE_START:
//      if(ch==' '||ch=='\t'||ch=='\n')
//
//
//      break;
//    default:
//      return this->transMap(This::ToNext(curState,ch));
//      break;
//  }
//}
//==========class DefinePreprocessor
void DefinePreprocessor::initWithLine (const char* s,size_t &index, size_t len,int& type)//this will start parsing after define found.
{
  size_t &i=index;
  LexicalParser lp;
  while(i < len )
    if(s[i]=='\t' || s[i]==' ')i++;
    else if(s[i]=='\\'){
	if(len-i>=1 && s[i+1]=='\n')i+=2;
	else {type=LexicalParser::TYPE_ERROR;return;}
    }
  if(i>=len)return;
  this->macroName.clear();
//  lp.parseId(s, len, i,this->macroName, type);//get name
  if(type==LexicalParser::TYPE_ERROR) return;
  else
	{
		while (i < len)
			if (s[i] == '\t' || s[i] == ' ')
				i++;
			else if (s[i] == '\\')
			{
				if (len - i >= 1 && s[i + 1] == '\n')
					i += 2;
				else
				{
					type = LexicalParser::TYPE_ERROR;
					return;
				}
				if (i < len)
				{
					if (s[i] == '(') //parse arguments
					{
					}
					else
						//no argument
						while (i < len)	//I feel DFA is more easy and graceful to use
						{
						}
				}
			}
	}

}
//
//void DefinePreprocessor::initWithLine (const std::string& s,size_t &index,int& type)
//{
//  this->initWithLine(s.c_str(),index,s.size(),type);
//}
//const DefinePreprocessor::StringList& DefinePreprocessor::getArgs () const
//{
//  return args;
//}
//
//void
//DefinePreprocessor::setArgs (const StringList& args)
//{
//  this->args = args;
//}
//
//const DefinePreprocessor::IntList& DefinePreprocessor::getBody () const
//{
//  return body;
//}
//
//void
//DefinePreprocessor::setBody (const IntList& body)
//{
//  this->body = body;
//}

const std::string&
DefinePreprocessor::getMacroName () const
{
  return macroName;
}



void
DefinePreprocessor::setMacroName (const std::string& macroName)
{
  this->macroName = macroName;
}
