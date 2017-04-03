/*
 * testParseNumber.cpp
 *
 *  Created on: 2017?3?22?
 *      Author: 13774
 */

#include <LexicalParser.h>
#include <SemanticParser.h>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <fstream>
#include <iostream>
#include <GrammaUtils.h>

#include <macros/all.h>
using namespace std;
using namespace x2;

#define BUFLEN 1024
char buffer[BUFLEN];

void testFile();
void testParseWord();
void testString();
void testNumber();
void testNote();
void testRecursiveMacro();
void testGramma();


int main()
{
//  testFile();
//  testParseWord();
//    testString();
//    testNumber();
//  testNote();
  testGramma();
}
void testGramma()
{
  Gramma g({
    {GrammaSymbols::TYPE_EMPTY,"<EMPTY>"},
    {GrammaSymbols::TYPE_TERM,"a"},
    {GrammaSymbols::TYPE_TERM,"b"},
    {GrammaSymbols::TYPE_VAR,"S"},//2,start符号
    {GrammaSymbols::TYPE_VAR,"B"},
    {GrammaSymbols::TYPE_VAR,"C"},
    {GrammaSymbols::TYPE_VAR,"D"}, //5
    {GrammaSymbols::TYPE_TERM,"c"},//6
    {GrammaSymbols::TYPE_VAR,"F"},//7
    {GrammaSymbols::TYPE_TERM,"<END>"},//8,end符号
  },
	 {
	     {2,{3,3}}, //S->BB
	     {3,{0,3}}, //B->aB
	     {3,{1}},   //B->b
	     {3,{-1}},    //B->EMPTY
	     {4,{5,2,3,0}}, //C->D S B a
	     {4,{5,2,3,0}}, //C->D S B a
	     {4,{5,2,3,1}}, //C->D S B b
	     {4,{5,2,2,1}}, //C->D S S b
	     {5,{5,0}},//D->D a
	     {5,{5,1}},//D->D b
	     {5,{2,3}},//D->S B
	     {5,{3,2}},//D->B S
//	     {5,{5}}, //D -> D
	     {7,{6}},//F->c
	     });

  //S->BB B->aB B->b  S->BBC C->BS , 这种文法可能引起空判定无限递归
//  g.syms.push_back(GrammaSymbol(GrammaSymbol::SYM_EMPTY));

  //==========打印符号表
  std::cout<< g.gsyms.toString()<<std::endl;

  //========查找'S'的下标
  int n=g.gsyms.findSymbolIndex("S");
  std::cout<<"S is at "<<n<<std::endl;

  //=======打印语法表
  std::cout<<g.toString()<<std::endl;

  //========确定符号是否包含空集
  std::cout << "S can be empty?" << g.canSymbolEmpty(2)<<std::endl;//can S empty?

  //========替换表达式
//  std::cout << "first replacement "<<std::endl;
//  g.replaceFirstProduction(2, 3);
//  std::cout << g.toString() <<std::endl;
//  std::cout << "second replacement "<<std::endl;
//  g.replaceFirstProduction(2, 3);
//  std::cout << g.toString() <<std::endl;
//  std::cout << "third replacement "<<std::endl;
//  g.replaceFirstProduction(2, 3);
//  std::cout << g.toString() <<std::endl;

  //======提取左因子的辅助函数 ： reduce
//  std::vector<int> subset={0,1,2};
//  int newvar=g.reduce(g.prods[5],g.gsyms.getString(5), 0, 2, subset);
//  std::cout << "reduce subset"<<std::endl;
//  std::cout << g.toString() <<std::endl;
//  std::cout << "reduce C' : [1 2] at (0,1)"<<std::endl;
//  subset={1,2};
//  g.reduce(g.prods[newvar],g.gsyms.getString(newvar), 0, 1, subset);
//  std::cout << g.toString() << std::endl;

//  //=======消除左因子
//  std::cout << "reduce left factors"<<std::endl;
//  std::vector<int> subset={0,1,2,3};
//  g.reduceLeftFactor(g.prods[5],g.gsyms.getString(5),0, subset);
//  std::cout << g.toString() << std::endl;

//  //=======消除直接左递归
//  std::cout << "reduce direct left recursive"<<std::endl;
//  g.reduceDirectLeftRecursive();
//  std::cout << g.toString() << std::endl;

  //=======消除所有左因子
  std::cout << "reduce all left factors"<<std::endl;
  g.reduceLeftFactor();
  std::cout << g.toString() << std::endl;

  //===========消除左递归
  std::cout << "reduce left recursive" << std::endl;
  g.reduceLeftRecursive();
  std::cout << g.toString() << std::endl;

  //===========消除重复产生式
  std::cout << "eliminate duplication" << std::endl;
  g.eliminateDuplication();
  std::cout << g.toString() << std::endl;

  //===========计算FIRST集
  std::cout << "calculating FIRST set" <<std::endl;
  Gramma::SetsType firstset=std::move(g.calcFirst());
  std::string strfirstset;
  std::for_each(firstset.begin(),firstset.end(),[&strfirstset,&g](const Gramma::SetsType::value_type &it){
    strfirstset += g.gsyms.getString(it.first) + "{\n\t\t";
    std::for_each(it.second.begin(),it.second.end(), [&strfirstset,&g](int i){
      strfirstset += g.gsyms.getString(i) + ", ";
    });
    strfirstset += std::string("\n}\n");
  });
  std::cout << strfirstset << std::endl;

  //==========计算FOLLOW集
  std::cout << "calculating FOLLOW set" << std::endl;
  Gramma::SetsType followset=std::move(g.calcFollow(firstset,2,8));
  std::string strfollwset;
  std::for_each(followset.begin(),followset.end(),[&strfollwset,&g](const Gramma::SetsType::value_type &it){
    strfollwset += g.gsyms.getString(it.first) + "{\n\t\t";
    std::for_each(it.second.begin(),it.second.end(), [&strfollwset,&g](int i){
      strfollwset += g.gsyms.getString(i) + ", ";
    });
    strfollwset += std::string("\n}\n");
  });
  std::cout << strfollwset << std::endl;

  //========拓广的LR文法
  LRGramma lrg(
	  {
		  {GrammaSymbols::TYPE_EMPTY,"<EMPTY>"},
		  {GrammaSymbols::TYPE_VAR,"E"}, //0
		  {GrammaSymbols::TYPE_VAR,"T"}, //1
		  {GrammaSymbols::TYPE_VAR,"F"}, //2
		  {GrammaSymbols::TYPE_TERM,"("}, //3
		  {GrammaSymbols::TYPE_TERM,")"}, //4
		  {GrammaSymbols::TYPE_TERM,"+"},//5
		  {GrammaSymbols::TYPE_TERM,"*"},//6
		  {GrammaSymbols::TYPE_TERM,"id"}//7
	  },
	  {
			  {0,{0,5,1}},
			  {0,{1}},
			  {1,{1,6,2}},
			  {1,{2}},
			  {2,{3,0,4}},
			  {2,{7}}
	  },
	  0,
	  "E'"
  );
  std::cout << "LRGramma(this example can be found in Aho's dragon book,page 155)" << std::endl;
  std::cout << lrg.Gramma::toString() << std::endl;
//  lrg.setDotString("<DOT>");

  //=======辅助函数 : 产生某个符号的项目集
  int iEd=lrg.gsyms.findSymbolIndex("E'");
  auto itemone = std::make_tuple(iEd,0,0);//E' -> .E
  LRGramma::ClosureType C=std::move(lrg.getClosure(itemone));
  std::cout << "Closure of E'->.E " << std::endl;
  std::cout << lrg.toString(C)<<std::endl;

  //========辅助函数：产生一个项集的GOTO集
  int iE=lrg.gsyms.findSymbolIndex("E");
  LRGramma::ClosureType GOTO=std::move(lrg.getGoto(C, iE));
  std::cout << "GOTO of CLOSURE({E'->.E})" << std::endl;
  std::cout << lrg.toString(GOTO);
  std::cout <<"inputing '+',getting next GOTO" << std::endl;
  int iplus=lrg.gsyms.findSymbolIndex("+");
  GOTO=std::move(lrg.getGoto(GOTO, iplus));
  std::cout << lrg.toString(GOTO) << std::endl;

  //=========规范集项目族
  std::cout << "CLOSURES & GOTO" << std::endl;
  auto tup= lrg.getAllClosures();
  std::cout << lrg.toString(tup) << std::endl;

  //===========do you agree with me?





}
#define A(i,j) i() k
void testRecursiveMacro()
{

}
void testNote()
{
  LexicalParser lp;
  const char *wordbuf="//0x4e5fz*\n0x\\/";
  size_t index=0;
  int type;
  std::string s;
//  lp.parseNote(wordbuf, index, strlen(wordbuf),s,type);
//  cout << LexicalParser::isInWord(' ')<<endl;
//  return;
  auto hmap=LexicalParser::getHumanReadable();

  cout << "("<<hmap[type]<<","<<s<<")"<<endl;
}

void testNumber()
{
  LexicalParser lp;
  const char *wordbuf="0x4e5fz0x";
  size_t index=0;
  int type;
  std::string s;
//  lp.parseNumber(wordbuf, index, strlen(wordbuf),s,type);
//  cout << LexicalParser::isInWord(' ')<<endl;
//  return;
  auto hmap=LexicalParser::getHumanReadable();

  cout << "("<<hmap[type]<<","<<s<<")"<<endl;
}
void testString()
{
  LexicalParser lp;
  const char *wordbuf="\"eu\\x68asdf \" d";
  size_t index=0;
  int type;
  std::string s;
//  lp.parseString(wordbuf, index, strlen(wordbuf),s,type);
//  cout << LexicalParser::isInWord(' ')<<endl;
//  return;
  auto hmap=LexicalParser::getHumanReadable();

  cout << "("<<hmap[type]<<","<<s<<")"<<endl;
}

void testParseWord()
{
  LexicalParser lp;
  const char *wordbuf="xU_hand23ed ll0";
  size_t index=0;
  int type;
  std::string s;
//  lp.parseId(wordbuf, index, strlen(wordbuf),s,type);
//  cout << LexicalParser::isInWord(' ')<<endl;
//  return;
  auto hmap=LexicalParser::getHumanReadable();

  cout << "("<<hmap[type]<<","<<s<<")"<<endl;
}

void testFile()
{
	LexicalParser lp;
//	const char* str="0x899aad";
//	size_t n=strlen(str);
//	lp.parseNumber(str,n);
//	const char* strbuf="\"ab\\b\\\nadafd\"";
//	size_t index=0;
//	size_t len=strlen(strbuf);
//	lp.parseString(strbuf, index, len);
//	const char *file="examples/normal_test1.cpp";
	const char *file="examples/define_test1.cpp";
	FILE *fp=fopen(file,"r+");
	assert(fp!=NULL);
	size_t readlen=fread(buffer,sizeof(char),BUFLEN,fp);
//	std::string t(buffer,readlen);
	size_t index=0;
	LexicalParser::WordStream stream;
	LexicalParser::TransMap		humanInfo=LexicalParser::getHumanReadable();
	lp.parseWords(stream,buffer, index, BUFLEN);
	printf("As a result,we get stream :\n<start>\n");
	LexicalParser::WordStream::iterator it=stream.begin(),itend=stream.end();
	while(it!=itend)
	{
//	    if(it->second!=LexicalParser::TYPE_SPACE && it->first.size()==1 && LexicalParser::isInSpace(it->first[0]))
//	      printf("%s ",it->first.c_str());
//	    else
	      printf("%s ",humanInfo[it->second].c_str());
	    it++;
	}
	printf("\n<end>\n");
	printf("is that proper for you to do syntax analyze?\n");

	//===Do define macro expansions
	LL1Macro llm(stream,0);
	llm.S();

}





