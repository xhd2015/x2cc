/*
 * LexicalParser.h
 *
 *  Created on: 2017?3?22?
 *      Author: 13774
 */

#ifndef LEXICALPARSER_H_
#define LEXICALPARSER_H_

#include <cstddef>
#include <map>
#include <string>
#include <vector>
#include <utility>

#define AS_MACRO inline __attribute__((always_inline))
#define ARRSIZE(arr)	(sizeof(arr)/sizeof(arr[0]))
#define BY_FULTON

class LexicalParser {
public:
	typedef LexicalParser This;
	typedef bool (*JUDGE_CHAR)(char ch);
	typedef std::vector<std::pair<std::string,int> > WordStream;
	typedef std::map<int,std::string>	TransMap;
	typedef std::map<char,int>		CharType;
public:
	enum{//accepted STATE is STATE_START
		STATE_START,
//		STATE_S1_START, STATE_S1_DIRECTIVE,STATE_S1_DEFINE_DIRECTIVE,STATE_S1_DEFINE_NAME,STATE_S1_DEFINE_BODY,STATE_S1_INCLUDE,
//STATE_S1_DIRECTIVE_BODY,STATE_S1_END,STATE_DEFINE,
		STATE_LESS,STATE_GREATER,STATE_DOUBLE_GREATER,STATE_EQUAL,STATE_DIVID,STATE_STAR,

		STATE_NOTE,STATE_LINE_NOTE,STATE_MULTILINE_NOTE,STATE_MULTILINE_GOING_END,
		STATE_TRANSFER_COMMON,STATE_TRANSFER_COMMON_X1,STATE_TRANSFER_COMMON_X2,STATE_TRANSFER_COMMON_U1,
		STATE_TRANSFER_CARRIGE,
		STATE_TRANSFER_LANGUAGE,
		STATE_TRANSFER_CHAR,
		STATE_TRANSFER_STRING,STATE_TRANSFER_VALUE,
		STATE_ID,
		STATE_KEEP_SPACE,
		STATE_STRING,	STATE_CHAR,
		STATE_NUMBER, 	STATE_HEX_NUMBER,	STATE_DECILMAL_NUMBER,		STATE_BIN_NUMBER,
		STATE_NO_CARE,
		STATE_END,
		/*ERROR_STATES*/STATE_UNRECOGNIZED_CHAR,STATE_TRANSFER_HEX_NOT_ENOGUH,STATE_TRANSFER_INVALID,

		STATE_LAST //for extension
	};
	enum{
		DONE_ONE_WORD,
		DOING_ONE_WROD
	};
	enum{
		TYPE_OP/*DEPRECATED*/=0,	TYPE_SEMICOLON=1/*;*/,TYPE_SINGLE=44,
		TYPE_EQ=26,/*TYPE_LESS,TYPE_GREATER,*/TYPE_DIV,/*TYPE_STAR,*/TYPE_PLUS,TYPE_AND,TYPE_EXP,TYPE_MOD,TYPE_DOLLAR,TYPE_NOT,
		  TYPE_SUB,TYPE_COMMA,TYPE_WAVE,TYPE_RQUOTE,TYPE_COLON,TYPE_ERECT/*|*/,TYPE_ASK/*?*/,TYPE_POINT,/*TYPE_TRANSFER=42,*/
		  TYPE_UNDERSCORE=48,TYPE_LSQ,TYPE_RSQ/*[ ]*/,TYPE_AT/*@*/,TYPE_QUOTE,TYPE_DQUOTE=53,
		TYPE_NUMBER_DECIMAL=2,	TYPE_NUMBER_BIN=23,TYPE_NUMBER_HEX=24,
		TYPE_ID=3,
		TYPE_STRING=4,
		TYPE_CHAR=5,
		TYPE_TRANSFER=6,
		TYPE_S1=16,/*#*/
		TYPE_NEWLINE=7,
		TYPE_LESS=17,
		TYPE_GREATER=18,
//		TYPE_S1_DEFINE=19,
//		TYPE_S1_INCLUDE=22,
//		TYPE_S1_BODY=20,
//		TYPE_S1_DIRECTIVE=21,
		TYPE_NOTE=8,TYPE_INLINE_NOTE,TYPE_FREE_NOTE,TYPE_DOC_NOTE,
		TYPE_LBRACE=9,
		TYPE_RBRACE=10,
		TYPE_LHBRACE=11,/*{ huge brace*/
		TYPE_RHBRACE=12,
		TYPE_STAR=13,
		TYPE_NONE=14,
		TYPE_UNKNOW=15,
		TYPE_ERROR=25,
		TYPE_UNRECOGNIZED=43,
		TYPE_SPACE=45,
		TYPE_END=46,
		TYPE_INFO_LOST=47,
		TYPE_LAST=54
	};
	enum{
		DIGIT_BIN=0,
		DIGIT_DECIMAL,
		DIGIT_HEX_LOWER,
		DIGIT_HEX_UPPER,
		DIGIT_UNKNOWN
	};
	enum{
		SET_ALPHA,//a-z A-Z
		SET_WORD,//SET_ALPHA + _
		SET_BIN,
		SET_DEC,
		SET_HEX,
		SET_SPACE,
		SET_SPECIAL,
		//=======
		SET_SIZE,
	};
	LexicalParser();
	~LexicalParser();

	static bool isDigitType(char ch,int type);
	static int tellDigitType(char ch);
	static bool isInSet(char ch,const char* buffer,size_t len);
	AS_MACRO static bool isInSet(char ch,int set);
	AS_MACRO static bool isInAlpha(char ch);
	AS_MACRO static bool isInWord(char ch);
	AS_MACRO static bool isInBin(char ch);
	AS_MACRO static bool isInDec(char ch);
	AS_MACRO static bool isInHex(char ch);
	AS_MACRO static bool isInSpace(char ch);
	AS_MACRO static bool isInSpecial(char ch);
	AS_MACRO static int  getSpecialType(char ch);

	AS_MACRO static int getCharVal(char ch);
	AS_MACRO static int getCharVal(const char* str)=delete;

	static int evalString(const std::string& s,int type=TYPE_NUMBER_DECIMAL)=delete;
	static int evalString(const char* buffer,size_t len,int type=TYPE_NUMBER_DECIMAL)=delete;
	AS_MACRO static const TransMap& getHumanReadable();
	AS_MACRO static bool	registerTypeString(int type,const std::string& s);
	AS_MACRO static const std::string& getTypeString(int type);

	static char	findValue(char key,const char* buffer,size_t len);

	/**
	 * parsers
	 */
	void parseNumber(const char *buffer,size_t len)=delete;
	void parseString(const char *buffer,size_t &index,size_t len)=delete;
	void parseWords(WordStream &stream,const char *buffer,size_t &index,size_t len);
	void parsePreps(const char *buffer,size_t &index,size_t len)=delete;//each parser has a method: parse
								    //it will give out the parsed results
	//================ABOVE uses integrated DFA===============

	//================FOLLOWING uses module design can be transfered to standard DFA=============
	void parseId(const char *buffer,size_t &index,size_t len,std::string& out,int &type)=delete;
	void parseNumber(const char *buffer,size_t &index,size_t len,std::string &out,int &type)=delete;
	/**
	 *  \x十六进制两位数
	 *  \\
	 *  \ NEWLINE		忽略这个换行
	 *  \其他任何字符		即使这个字符是特殊字符，也将其作为ID的一部分
	 */
	void parseTransferId(const char* buffer,size_t &index,size_t len,std::string &out,int &type)=delete;
	/**
	 *  \x十六进制
	 */
	void parseTransferValue(const char *buffer,size_t &index,size_t len,std::string& out,int &type)=delete;
	void parseTransferAny(const char *buffer,size_t &index,size_t len,std::string &out,int &type)=delete;//common part for both ID & Value

	void parseString(const char *buffer,size_t &index,size_t len,std::string &out,int &type)=delete;
	void parseNote(const char *buffer,size_t &index,size_t len,std::string &out,int &type)=delete;
	AS_MACRO void	skipSpaces(const char *buffer,size_t &index,size_t len)=delete;

	void error(const char *msg)const;

	static const JUDGE_CHAR judgeArr[SET_SIZE];
	static TransMap humanInfo;
	static CharType	charType;//one basic requirement : all are expressed as TYPE_XXX,not the character itself.
};

class PrintDebugger{
public:
	PrintDebugger();
	~PrintDebugger();

	void setDoOutput(bool doOut);
	void info(const char *str)const;
protected:
	bool doOut;
};

class LexicalParserBase{
public:
  virtual void parse(const char *buffer,size_t len,size_t &index)=0;
  virtual void error(const char *buffer,size_t len,size_t &index,int &state)=0;//state will be set to the next state
  virtual int getNextState(int curState,char ch)=0;
};

class LexicalPrep:public LexicalParserBase{
public:
  enum{
    STATE_LAST=-1,
    STATE_ERROR=-2,
    STATE_START=0,
    STATE_TRANSFER,
    STATE_ID_START,STATE_ID_END,
    STATE_NUMBER,
    STATE_DIRECTIVE_START,STATE_DIRECTIVE_END,
  };
  enum{
    SET_START=-1000,
    SET_BIN,SET_HEX,SET_DEC,
    SET_LOWER,SET_UPPER,SET_LETTER,SET_WORD_START,SET_WORD_END,
    SET_INLINE_SPACE,SET_SPACE,
    SET_NON_WORD

  };
  typedef LexicalPrep				This;
  typedef std::map<int,std::map<int,int>>	TransMap; //int means a charset

public:
  LexicalPrep()=default;
  LexicalPrep(int i);
  void parse(const char *buffer,size_t len,size_t &index);
  int	getNextState(int curState,char ch);
  void	error(const char *buffer,size_t len,size_t &index,int &state);

protected:
  TransMap transMap;
};

class Preprocessor{//based on the word analyser,do macro syntax analyse.
public:
  enum{//directives
    D_DEFINE,
    D_INCLUDE,
    D_IF,
    D_ELIF,
    D_ELSE,
    D_END,
    D_DEFINED,
    D_IFDEF,
    D_IFNDEF,
  };
public:

};

class DefinePreprocessor{
public:
  typedef std::vector<std::string> StringList;
  typedef std::vector<int>	   IntList;

  DefinePreprocessor()=default;
  ~DefinePreprocessor();

  void initWithLine(const char *s,size_t &index,size_t len,int &type);
  void initWithLine(const std::string& s,size_t &index,int &type);




  //====setters and getters
  const StringList&  getArgs () const;
  void  setArgs (const StringList& args);
  const IntList&   getBody () const;
  void  setBody (const IntList& body);
  const std::string&  getMacroName () const;
  void  setMacroName (const std::string& macroName);
  const std::string& getDefinition()const;


public:
  std::string	originalDef;
  std::string macroName;
  StringList	args;
  IntList	body;
};

//===========function macros

bool	LexicalParser::registerTypeString(int type,const std::string& s)
{
  if(humanInfo.find(type)!=humanInfo.end())return false;
  humanInfo[type]=s;
  return true;
}
 const std::string& LexicalParser::getTypeString(int type)
 {
   if(humanInfo.find(type)==humanInfo.end())return humanInfo[TYPE_INFO_LOST];
   return humanInfo[type];
 }

#endif /* LEXICALPARSER_H_ */
