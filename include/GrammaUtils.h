/*
 * GrammaUtils.h
 *
 *  Created on: 2017年3月31日
 *      Author: 13774
 */

#ifndef GRAMMAUTILS_H_
#define GRAMMAUTILS_H_
#include <string>
#include <vector>
#include <map>
#include <set>
#include <initializer_list>
#include <def.h>
#include <iostream>
#include <algorithm>
#include <tuple>
namespace x2
{
  //=======================
  /**
   * 文法符号的约定：每一个文法符号
   */
//  class GrammaSymbol/*文法符号*/
//  {
//  public:
//    enum{
//      SYM_EMPTY=0,
//      SYM_TERM=1,
//      SYM_ERROR=2,
//    };
//  public:
//    GrammaSymbol()=default;
//    GrammaSymbol(int type,const std::string& s);
//    GrammaSymbol(int type,std::string&& s="");
//    const GrammaSymbol& operator=(int type);
//    ~GrammaSymbol ();
//    AS_MACRO bool isTerm()const;
//    AS_MACRO bool isEmpty()const;
//    AS_MACRO bool isVar()const;
//    AS_MACRO int getType()const;
//    AS_MACRO std::string getValue()const;
//    AS_MACRO static GrammaSymbol makeEmpty();
//    AS_MACRO std::string toString()const;
//  public:
//    int type;
//    std::string value;
//  };
//  AS_MACRO bool operator<(const GrammaSymbol &a,const GrammaSymbol& b){if(a.type!=b.type)return a.type<b.type;else return a.value<b.value;}

  //====================
  /**
   * 对于typeString而言
   *  如果是TYPE_TERM,则长度为至少是1，一般情况下是1
   *  如果是TYPE_EMPTY,可以是任意值
   *  如果是其它的，长度至少是1，并且建议大写字母
   *
   *  任何时刻，保证表中的empty只有一项
   * 这个类将由其它所有的类共享
   *   empty项必须存在，且仅有一项
   *   内置的属性值：TYPE_UNDEFINED "UNDEFINED"
   */
  class GrammaSymbols{/*符号管理表*/
  public:
    enum{
      TYPE_TERM,
      TYPE_VAR,
      TYPE_EMPTY,
      TYPE_UNDEFINED
    };
    static std::map<int,std::string> TYPES_INFO;
    typedef std::map<int,int> TypeInfo;
    typedef std::map<int,std::string> TypeString;

    AS_MACRO GrammaSymbols();
    GrammaSymbols(const GrammaSymbols &gsyms)=default;
    GrammaSymbols(GrammaSymbols &&gsyms);
    GrammaSymbols(const std::initializer_list<std::pair<int,std::string> >& list);
    GrammaSymbols(std::initializer_list<std::pair<int,std::string> >&& list);
    GrammaSymbols & operator=(const GrammaSymbols & gs);
    GrammaSymbols & operator=(GrammaSymbols && gs);

    AS_MACRO int addTerm(std::string &&s);
    AS_MACRO int addTerm(const std::string &s);//return the index that generated
    AS_MACRO int addVar(std::string &&s);
    AS_MACRO int addVar(const std::string & s);
    AS_MACRO int addEmpty(const std::string & s);
    AS_MACRO int addEmpty(std::string && s);
    AS_MACRO bool isSymbolEmpty(int i)const;
    AS_MACRO bool isSymbolTerm(int i)const;
    AS_MACRO bool isSymbolVar(int i)const;
    AS_MACRO bool isSymbolUndefined(int i)const;
    AS_MACRO int getSymbolType(int i)const;
    AS_MACRO int findEmpty()const;
    AS_MACRO int findSymbolIndex(const std::string & s)const;
    AS_MACRO void lock();//lock to avoid any modifying
    AS_MACRO void unlock();//unlock

    int add(int type,std::string &&s);
    int add(int type,const std::string &s);
    void deleteNo(int no);//删除某一项
    const std::string& getString(int i)const;
    std::string	toString()const;
  public:
    int max; //-1 就是empty产生式
    std::map<int,int>	typeInfo;
    std::map<int,std::string> typeString;
    static std::string	UNDEFINED;
  };
  //=====================================
  /**
   * GrammaSentence约定：
   * 	如果GrammaSentence为空产生式，则含有语法变量 SYM_EMPTY
   * 	长度至少为1
   */
  class GrammaSentence{/*句型*/
    typedef  std::vector<int>	SymsList;
    typedef  SymsList::size_type size_type;
  public:
    GrammaSentence()=default;
    GrammaSentence(GrammaSentence&& gs)=default;
    GrammaSentence(const GrammaSentence& gs)=default;
    GrammaSentence(const SymsList & slist);
    GrammaSentence(SymsList && slist);
    GrammaSentence(std::initializer_list<int>&& list);
    GrammaSentence(const std::initializer_list<int>& list);
    GrammaSentence& operator=(const GrammaSentence& gs);

    void replaceFirst(const GrammaSentence& gs,int empty);
    std::vector<GrammaSentence> replaceFirst(const std::vector<GrammaSentence>& vgs,int empty);
    GrammaSentence duplicate()const;
    AS_MACRO int getFirstSymbol()const;
    AS_MACRO bool startsWith(int i)const;
    AS_MACRO size_type getLength()const;
  public:
    SymsList syms;
  };

  //===================================
  /**
   * Gramma状态约定：
   * 	如果syms中存在一个非终结符号，则prods必然包含至少一个产生式（统一性）
   * 	prods中的所有产生式，左部只能来自syms表的非终结符，右部只能是syms符号的一个组合
   *
   * 	First集中只能含有空符号或者终结符
   *
   * 	我的优化的原意是消除冗余，但是现在看来没有那个必要了
   *
   * 	some conditions:
   * 		S -> S  毫无意义， 会在消除直接左递归的过程中消除掉，但是这属于循环推导，可能会引起消除间接左递归的失败
   *
   *
   */
  class Gramma{/*文法*/
  public:
    typedef Gramma This;
    typedef std::map<int,std::vector<GrammaSentence> > ProductionsType;
    typedef std::map<int,std::set<int> > SetsType;
    typedef std::vector<int>::size_type size_type;
    Gramma()=default;
    Gramma(const GrammaSymbols & gsyms);
    Gramma(GrammaSymbols&& gsyms);
    Gramma(std::initializer_list<std::pair<int,std::string> > &&list);
    Gramma(const std::initializer_list<std::pair<int,std::string> > &list);

    Gramma(const std::initializer_list<std::pair<int,std::string> > &list,
	   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist);
    Gramma(std::initializer_list<std::pair<int,std::string> > &&list,
	   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist);
    Gramma(const std::initializer_list<std::pair<int,std::string> > &list,
	   std::initializer_list<std::pair<int,GrammaSentence> > &&prodlist);
    Gramma(std::initializer_list<std::pair<int,std::string> > &&list,
	   std::initializer_list<std::pair<int,GrammaSentence> > &&prodlist);


    void addProduction(int i,const GrammaSentence &gs);
    void addProduction(int i,GrammaSentence &&gs);

    bool canSymbolEmpty(int i);
    bool canSentenceEmpty(const GrammaSentence& s);//可以构造一个非空表，以及一个关于句型的第一个非空符号的位置（左起最长空串的长度）
    bool canSentenceEmpty(const GrammaSentence& s,int end);
    bool canSentenceEmpty(const GrammaSentence& s,int start,int end);
    AS_MACRO const std::vector<GrammaSentence>& getRightSentences(int i)const;
    AS_MACRO std::vector<GrammaSentence>& getRightSentences(int i);

    /**
     * 对 i -> j ..进行替换
     */
    void replaceFirstProduction(int i,const int j);//将每个形如 i -> j a | j b  | j c 的形式替换成j的所有表达式
    void reduceLeftRecursive();//定义顺序,不会引入新的符号
    void eliminateSelfDeduction(int i);//消除循环推导
    AS_MACRO void eliminateSelfDeduction();//可能改变prods的结构，当符号D的生成式仅仅是D->D时，D会被完全删除
    void eliminateDuplication(int i);
    AS_MACRO void eliminateDuplication();

    void reduceDirectLeftRecursive(int i);//消除直接左递归，引入新的符号
    AS_MACRO void reduceDirectLeftRecursive();

    AS_MACRO void reduceLeftFactor();//消除所有的左因子
    AS_MACRO void reduceLeftFactor(int i);//消除产生式i的左因子
    /**
     * start from start, end at end,not including end
     */
    int reduce(std::vector<GrammaSentence> &data,const std::string & varname,int start,int end,std::vector<int>& subset);
   // void reduce(std::vector<GrammaSentence> &data,const std::string & varname,int start,int end,std::vector<int> subset);
    void reduceLeftFactor(std::vector<GrammaSentence> & data,const std::string & varname,int i);

    /**
     * 获得当前的产生式的所有键，以避免发生prods修改时，迭代器失效
     */
    std::vector<int>	getProductionsHead()const;

    /**
     * 消除 i中所有以j开头的左因子
     * 	查找以最长公共前缀，将前缀后面的串替换成一个新的语法变量，然后删除重复的串
     *
     * 引入的语法变量名称为原变量加上'
     */
    void reduceLeftFactor(int i,int j);
    SetsType calcFirst();
    std::set<int> calcFirst(const GrammaSentence & gs,int start,int end,const SetsType& firstset);//根据句型获得FIRST集
    SetsType calcFollow(int start,int end);
    SetsType calcFollow(const SetsType& firstset,int start,int end);


    Gramma duplicate(); //symbols , productions will change
    Gramma duplicateShareSymbols();//Symbols will not change,but if any one change,then everything changed

    AS_MACRO std::string toString()const;
    AS_MACRO std::string toString(int i)const;
    AS_MACRO std::string toString(const GrammaSentence &gs)const;
    AS_MACRO std::string toString(int i,const std::vector<GrammaSentence>& gss)const;
    std::string toString(const SetsType& set)const;
    std::string toString(const std::set<int> & set)const;

//    void addTerm();// 2 --> 345 list of int
//    void addVar();
//    void isTermIn();

    AS_MACRO const GrammaSymbols & getGrammaSymbols();





  public:
    GrammaSymbols gsyms;
    ProductionsType prods;
//    SetsType	first;
//    SetsType	follow;
  };

  /**
   * LRGramma
   */
  class LRGramma:public Gramma{
  public:
	  typedef LRGramma This;
	  typedef Gramma	Father;
	  typedef std::tuple<int,int,int>	ItemType;
	  /**
	   * 这里的ClosureType和教材上类型有所不同，教材上的类型是
	   *  	std::map<std::set<ItemType>,std::set<ItemType>> ClosureType
	   * 这里为了方便，先定义单个项的闭包
	   * 如果是多个项
	   *   Closure(I1,I2,...,In) = Closure(I1) u Closure(I2) u ... u Closure(In)
	   *
	   */
	  typedef std::set<ItemType> ClosureType;
	  typedef std::vector<ClosureType> ClosuresVector;//对所有的项目集进行管理，所有的运算基于闭包在ClosuresVector中的下标


	  LRGramma()=default;

	  //======从一个现有的一般性文法构造一个LRGramma文法
	  LRGramma(const Gramma& g,int oristart,int oriend,const std::string & strstart);
	  LRGramma(Gramma&& g,int oristart,int oriend,const std::string & strstart);
	  LRGramma(const std::initializer_list<std::pair<int,std::string> > &list,
		   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist,int oristart,int oriend,const std::string & strstart);
	  LRGramma(const GrammaSymbols & gs,const ProductionsType & prods,int oristart,int oriend,const std::string & strstart);
	  LRGramma(const GrammaSymbols & gs,ProductionsType && prods,int oristart,int oriend,const std::string & strstart);
	  LRGramma( GrammaSymbols && gs,const ProductionsType & prods,int oristart,int oriend,const std::string & strstart);
	  LRGramma(GrammaSymbols && gs,ProductionsType && prods,int oristart,int oriend,const std::string & strstart);

	  /**
	   * return a tuple
	   * 		tuple[0] : contains closures that indexes in tuple[1] & tuple[2] point to
	   * 		tuple[1] : a set of integers, those integers denote the concanical item sets of the gramma
	   * 		tuple[2] : the goto map. the key type is pair<int,int>, the first int denotes a closure in tuple[0],the second is a possible gramma symbol.
	   *
	   *  call toString(tup) to dump those information.
	   */
	  std::tuple<ClosuresVector,std::set<int>,std::map<std::pair<int,int>,int>> getAllClosures();

	  /**
	   * HIGH-WAY getClosure ==  shut down//deprecated
	   * 可能产生新的闭包
	   */
	  int 			getClosureInVector(const ItemType& i,ClosuresVector &itemsets, std::map<ItemType,int>&		C0);
	  ClosureType getClosure(const ItemType& i);
	  void			getClosure(const ItemType& i,ClosureType& C);//adding to C
	  ClosureType getClosure(const ClosureType & C);
	  /**
	   * HIGH-WAY getGoto == shut down//deprecated
	   */
	  ClosureType getGotoInVector(int iclo,int x, ClosuresVector& itemsets, std::map<ItemType,int>	&	C0);
	  ClosureType getGoto(const ClosureType & items,int x);
	  int			getClosure(int i);//求下标i对应的项目集

	  AS_MACRO int		  getFirstSymbolAfterDot(const ItemType & i);
	  int				  getNthSymboleAfterDot(const ItemType & i,unsigned int j);

	  AS_MACRO int	getGStart()const;
	  AS_MACRO int  getGEnd()const;

	  AS_MACRO		std::string toString()const;
	  std::string toString(const ItemType& item);
	  std::string toString(const ClosureType& closure);
	  std::string toString(const  std::tuple<ClosuresVector,std::set<int>,std::map<std::pair<int,int>,int>> & tups);
	  std::string toStringSet(int i);
	  AS_MACRO void		  setDotString(const std::string & s);

  public:
	  int sstart;//拓广之后的start
	  int send;//必须定义结束符号
	  std::string dotString;
  };

  /**
   * LR1 Gramma
   *   each item is a 4-tuple: LR0Item plus a terminator
   */
  class LR1Gramma:public LRGramma{
  public:
	  typedef LR1Gramma	This;
	  typedef LRGramma	Father;
	  typedef std::tuple<int,int,int,int> ItemType;
	  typedef std::set<ItemType> 		ClosureType;
	  typedef std::vector<ClosureType>	ClosuresVector;
	  typedef std::tuple<ClosuresVector,std::set<int>,std::map<std::pair<int,int>,int>> InfoType;
  public:
	  AS_MACRO LR1Gramma(const std::initializer_list<std::pair<int,std::string> > &list,
		   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist,int oristart,int oriend,const std::string & strstart);

	  ClosureType	getClosure(const ItemType& i);
	  ClosureType	getClosure(const ItemType& i,const Gramma::SetsType &firstset);
	  void			getClosure(const ItemType& i,ClosureType& C,const Gramma::SetsType &firstset);
	  ClosureType	getClosure(const ClosureType & C,const Gramma::SetsType &firstset);
	  ClosureType	getGoto(const ClosureType& clo,int x,const Gramma::SetsType &firstset);
	  /**
	   * insert i in C, if really updated,return true;
	   */
	  AS_MACRO static bool			doInsert(ClosureType & C,const ItemType &i);
	  InfoType				getAllClosures();
	  AS_MACRO int		  getFirstSymbolAfterDot(const ItemType & i);
	  int		  getNthSymboleAfterDot(const ItemType & i,unsigned int j);

	  AS_MACRO std::string toString()const;
	  std::string toString(const ItemType& i);
	  std::string toString(const ClosureType& closure);
	  std::string toString(const InfoType& info);
  };

  //===========function macrso

  //====class GrammaSymbols
  GrammaSymbols::GrammaSymbols():
      max(0)
  {
    this->add(TYPE_EMPTY, "EMPTY");
  }
  int GrammaSymbols::addTerm(std::string &&s)
  {
    return add(TYPE_TERM,s);
  }
  int GrammaSymbols::addTerm(const std::string &s)
  {
    return add(TYPE_TERM,s);
  }
  int GrammaSymbols::addVar(std::string &&s)
  {
    return add(TYPE_VAR,s);
  }
  int GrammaSymbols::addVar(const std::string & s)
  {
    return add(TYPE_VAR,s);
  }

  int GrammaSymbols::addEmpty(const std::string & s)
  {
    return add(TYPE_EMPTY,s);
  }
  int GrammaSymbols::addEmpty(std::string && s)
  {
    return add(TYPE_EMPTY,s);
  }
  bool GrammaSymbols::isSymbolEmpty(int i)const
  {
    return getSymbolType(i)==TYPE_EMPTY;
  }
  bool GrammaSymbols::isSymbolTerm(int i)const
  {
    return getSymbolType(i)==TYPE_TERM;
  }
  bool GrammaSymbols::isSymbolVar(int i)const
  {
    return getSymbolType(i)==TYPE_VAR;
  }
  bool GrammaSymbols::isSymbolUndefined(int i)const
  {
    return getSymbolType(i)==TYPE_UNDEFINED;
  }
  int GrammaSymbols::getSymbolType(int i)const
  {
    TypeInfo::const_iterator it=typeInfo.find(i);
    if(it!=typeInfo.end())return it->second;
    return TYPE_UNDEFINED;
  }
  int GrammaSymbols::findEmpty()const
  {
    return -1;
  }
  int GrammaSymbols::findSymbolIndex(const std::string & s)const
  {
    TypeString::const_iterator it=std::begin(this->typeString),itend=std::end(this->typeString);
    for(;it!=itend;it++)
      {
	if(it->second.compare(s)==0)return it->first;
      }
    return -1;
  }

  //====class GrammarSentence
  int GrammaSentence::getFirstSymbol()const
  {
    return this->syms.size() > 0?this->syms[0]:-1;
  }
  bool GrammaSentence::startsWith(int i)const
  {
    return getFirstSymbol()==i;
  }
  GrammaSentence::size_type GrammaSentence::getLength()const
  {
    return this->syms.size();
  }

  //=====class GrammaProduction


  //======class Gramma
  const std::vector<GrammaSentence>& Gramma::getRightSentences(int i)const
   {
     return this->prods.at(i);
   }
  std::vector<GrammaSentence>& Gramma::getRightSentences(int i)
   {
     return this->prods[i];
   }
  void Gramma::eliminateSelfDeduction()
  {
    auto vec=std::move(getProductionsHead());
    std::for_each(vec.begin(),vec.end(), [this](int i){
      this->eliminateSelfDeduction(i);
    });
  }
  void Gramma::eliminateDuplication()
  {
      auto vec=std::move(getProductionsHead());
  std::for_each(vec.begin(),vec.end(), [this](int i){
    this->eliminateDuplication(i);
  });
  }
  void Gramma::reduceDirectLeftRecursive()
  {
    auto vec=std::move(getProductionsHead());
    std::for_each(vec.begin(),vec.end(), [this](int i){
      this->reduceDirectLeftRecursive(i);
    });
  }
 void Gramma::reduceLeftFactor()
  {
     //这些方法之所以可能是错误的，是因为在遍历的过程中修改了prods的结构
//   for(auto it=prods.begin(),itend=prods.end();it!=itend;it++)reduceLeftFactor(it->first);
   auto vec=std::move(getProductionsHead());
   std::for_each(vec.begin(),vec.end(), [this](int i){
     this->reduceLeftFactor(i);
   });
  }
  void Gramma::reduceLeftFactor(int i)
  {
    reduceLeftFactor(prods[i] , gsyms.getString(i),0);
  }
  std::string Gramma::toString()const
  {
    std::string s;
//    ProductionsType::const_iterator it=prods.begin();it->
    std::for_each(std::begin(this->prods),std::end(this->prods), [&s,this](const ProductionsType::value_type& it){
		    s+=this->toString(it.first,it.second)+"\n";
    });
    return s;
  }
  AS_MACRO std::string Gramma::toString(int i)const
  {
      return this->gsyms.getString(i);
  }
  std::string Gramma::toString(const GrammaSentence& gs)const
  {
    std::string s;
    std::for_each(std::begin(gs.syms),std::end(gs.syms),[&s,this](int t){s+=this->gsyms.getString(t)+" ";});
    return s;
  }

  std::string Gramma::toString(int i,const std::vector<GrammaSentence>& gss)const
  {
    std::string s;
    std::string head=this->toString(i);
    std::for_each(std::begin(gss),std::end(gss),[&s,this,&head](const GrammaSentence& gs){
	s+=head + " -> " + this->toString(gs) + "\n";
    });
    return s;
  }
  //============class LRGramma
  int  LRGramma::getFirstSymbolAfterDot(const ItemType & i)
  {
 	 return getNthSymboleAfterDot(i,1);
  }
  int			LRGramma::getGStart()const
  {
	  return sstart;
  }
  int LRGramma::getGEnd()const
  {
	  return send;
  }
  std::string LRGramma::toString()const
  {
	  return Gramma::toString();
  }
  void		  LRGramma::setDotString(const std::string & s)
  {
	  this->dotString=s;
  }

  //===========class LR1Gramma
  LR1Gramma::LR1Gramma(const std::initializer_list<std::pair<int,std::string> > &list,
  		   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist,int oristart,int oriend,const std::string & strstart):
  				   LRGramma(list,prodlist,oristart,oriend,strstart)
	{

	}
  /**
   * return false (means no update)
   */
  AS_MACRO bool	LR1Gramma::doInsert(ClosureType & C,const ItemType &i)
  {
	  if(C.find(i)!=C.end())return false;
	  C.insert(i);
	  return true;
  }
  int		  LR1Gramma::getFirstSymbolAfterDot(const ItemType & i)
  {
 	 return getNthSymboleAfterDot(i,1);
  }
  std::string LR1Gramma::toString()const
  {
	  return LRGramma::toString();
  }

} /* namespace x2 */

#endif /* GRAMMAUTILS_H_ */
