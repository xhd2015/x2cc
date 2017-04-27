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
#include <unordered_map>
#include <unordered_set>
#include <MutualMap.h>
#include <sstream>
namespace x2
{
  //====================
  /**
   * 对于typeString而言
   *  如果是TYPE_TERM,则长度为至少是1，一般情况下是1
   *  如果是TYPE_EMPTY,可以是任意值
   *  如果是其它的，长度至少是1，并且建议大写字母
   *
   *  TYPE分三种类型：终结符  空符号  变量
   *  空符号需要做单独处理，但是不属于存储级别的单独处理
   *
   *
   *  任何时刻，保证表中的empty有且只有一项 -- 实现方式就是令将EMPTY添加到存储结构之后，缓存返回的EMPTY index
   *  EMPTY 只支持初始化，不支持修改
   *
   * 这个类将由其它所有的类共享
   *   empty项必须存在，且仅有一项,且其下标必须是EMPTY_INDEX, EMPTY_INDEX,EMPTY_STRING 需要存储在容器之中;
   *   不可以删除empty项
   *   初始化symString时使用UNDEFINED_STRING
   *
   *   内置的属性值：TYPE_UNDEFINED "UNDEFINED"
   *
   *   仅支持添加操作，暂不支持删除操作
   *
   *   可以使用string来添加文法，查询文法符号
   */
 /*符号管理表*/
  class GrammaSymbols{
  public:
	enum TYPE_DEF{TYPE_TERM,	TYPE_VAR,	TYPE_EMPTY,	TYPE_UNDEFINED };
	//预定义的数字，如果是固定的，可以不必使用string，使用string的优势主要在于可以自定义类型
    typedef std::map<int,int> TypeInfo;
    typedef std::map<int,std::string> TypeString;
    typedef GrammaSymbols This;

    GrammaSymbols(const std::string& emptyString=EMPTY_STRING);/*必须提供emptyString的*/

    GrammaSymbols(const GrammaSymbols &gsyms)=default;
    GrammaSymbols(GrammaSymbols &&gsyms);
    GrammaSymbols(const std::initializer_list<std::pair<int,std::string> >& list);
    GrammaSymbols & operator=(const GrammaSymbols & gs);
    GrammaSymbols & operator=(GrammaSymbols && gs);

    //<block time="2017-04-21 22:02:42“>
//    [[deprecated("use get instead")]]
    AS_MACRO int		get(const std::string & s)const;
    AS_MACRO int		getAdd(const std::string & sym,const std::string & type="TERMINATOR");
    AS_MACRO int		getAdd(const std::string & sym,int type);
    /**
     * add,if exist,replace it.
     */
    int addReplace(const std::string & sym,const std::string & type="TERMINATOR");
    int addReplace(const std::string & sym,int type);
    AS_MACRO int		getEmpty()const;
    AS_MACRO bool	isSymbolEmpty(const std::string &sym)const;
    AS_MACRO bool	isSymbolTerm(const std::string &sym)const;
    AS_MACRO bool	isSymbolUndefined(const std::string & sym)const;
    AS_MACRO int		getSymbolType(const std::string & sym)const;
    //</block>

    AS_MACRO int addTerm(const std::string &s);//return the index that generated
    AS_MACRO int addVar(const std::string & s);
    		 int addEmpty(const std::string& s);
    AS_MACRO bool isSymbolEmpty(int i)const;
    AS_MACRO bool isSymbolTerm(int i)const;
    AS_MACRO bool isSymbolVar(int i)const;
    AS_MACRO bool isSymbolUndefined(int i)const;
    AS_MACRO int getSymbolType(int i)const;

    AS_MACRO void lock();//lock to avoid any modifying
    AS_MACRO void unlock();//unlock

    //==IMPORTANT, core methods
    /**
     * add always,no matter if it exists or not
     */
    AS_MACRO void add(int index,int type);//add by index,non-replace,if failed,return next available;else return index

    //deprecated
//    void deleteNo(int no);//删除某一项
    const std::string& getString(int i)const;
    std::string	toString()const;
  public:
    std::map<int,int>			symInfo;
    x2::IndexedMap<std::string> symString;
    int							emptyIndex;
    static std::string	UNDEFINED_STRING,EMPTY_STRING;
    static int			UNDEFINED_INDEX;

    static x2::MutualMap<int,std::string> TYPES_INFO;
  };
  //=====================================
  /**
   * GrammaSentence约定：
   * 	如果GrammaSentence为空产生式，则含有语法变量 SYM_EMPTY
   * 	长度至少为1
   */
  /*句型*/
  class GrammaSentence{
  public:
    typedef  std::vector<int>	SymsListType;
    typedef  SymsListType::size_type size_type;
  public:
    GrammaSentence()=default;
    GrammaSentence(GrammaSentence&& gs)=default;
    GrammaSentence(const GrammaSentence& gs)=default;
    GrammaSentence(const SymsListType & slist);
    GrammaSentence(SymsListType && slist);
    GrammaSentence(std::initializer_list<int>&& list);
    GrammaSentence(const std::initializer_list<int>& list);
    GrammaSentence& operator=(const GrammaSentence& gs);

    void replaceFirst(const GrammaSentence& gs,int empty);
    std::vector<GrammaSentence> replaceFirst(const std::vector<GrammaSentence>& vgs,int empty);
    GrammaSentence duplicate()const;
    AS_MACRO int getFirstSymbol()const;
    AS_MACRO bool startsWith(int i)const;
    AS_MACRO size_type getLength()const;
    void	push_back(int i);
  public:
    SymsListType syms;
  };

  //===================================
  /**
   * LL Utilities are automatically loadeed
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
  /*文法*/
  class Gramma{
  public:
    typedef Gramma This;
    typedef std::map<int,std::vector<GrammaSentence> > ProductionsType;
    typedef std::map<int,std::set<int> > SetsType;
    typedef std::vector<int>::size_type size_type;

  /**
   * @brief	带有冲突类型的LR分析表
   * @detail 请注意,LR分析表对于所有的LR文法都是通用的.
   * tuple<int,int,int>		<i,j, action>
   */
    typedef std::map<std::pair<int,int>,std::set<std::tuple<int,int,int>>> LRCorruptTableType;
    /**
     * @brief 不带冲突的LR分析表,也称为标准的分析表
     */
    typedef std::map<std::pair<int,int>,std::tuple<int,int,int>> LRAnalyzeTableType;


    Gramma()=default;
    Gramma(const GrammaSymbols & gsyms);
    Gramma(GrammaSymbols&& gsyms);
    Gramma(const std::initializer_list<std::pair<int,std::string> > &list);
    Gramma(const std::string &file);//construct from file
    Gramma(std::istream &in);

    Gramma(const std::initializer_list<std::pair<int,std::string> > &list,
	   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist);
    Gramma(const std::initializer_list<std::pair<std::string,std::initializer_list<std::string>>> &prodlist);
    Gramma(const std::vector<std::pair<std::string,std::vector<std::string>>> &prodlist);
    Gramma(std::initializer_list<std::string> list);//通过"A B C d"的形式,不加中间的箭头,末尾不能有空格
    Gramma(const std::vector<std::string> &list);



    void addProduction(int i,const GrammaSentence &gs);
    void addProduction(int i,GrammaSentence &&gs);

    /**
     * added 2017-04-21 21:49:40
     */
    void addProduction(const std::string& head,const std::vector<std::string>& gs);
    /**
     * added 2017-04-21 21:49:47
     */
    void addProduction(const std::string& head,const std::initializer_list<std::string> & gs);
    void addProduction(const std::string& all);

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
    SetsType calcFollow(const SetsType& firstset,int startSym,int endSym);


    Gramma duplicate(); //symbols , productions will change
    Gramma duplicateShareSymbols();//Symbols will not change,but if any one change,then everything changed

    /**
     * @brief 提供一个最简单的将冲突的分析表转换成标准分析表的方法：仅仅保留第一项
     */
    static LRAnalyzeTableType convertCorruptToStandardSimply(const LRCorruptTableType & table);

    AS_MACRO std::string toString()const;
    AS_MACRO std::string toString(int i)const;
    AS_MACRO std::string toString(const GrammaSentence &gs)const;
    AS_MACRO std::string toString(int i,const std::vector<GrammaSentence>& gss)const;
    AS_MACRO std::string toString(int i,int j)const;//j of i

    /**
     * for FOLLOW & FIRST
     */
    std::string toString(const SetsType& set)const;
    std::string toString(const std::set<int> & set)const;

    AS_MACRO const GrammaSymbols & getGrammaSymbols();

  public:
    GrammaSymbols gsyms;
  protected:
    ProductionsType prods;
  };

//  //<block note="reconstruct" time="2017-04-22 15:27:43">
//  /**
//   * General Model:
//   *  i,j   -->  which production?
//   *  k		-->	 position of the dot
//   *
//   *  //LR1
//   *  s		-->	 symbol of looking ahead
//   *
//   *  known types : LR(0) Items, LR(1) Items, SLR(1) items  LALR(1) items
//   *  从广泛的意义上来说（就行为层面），每个项目类型的 Closure,Goto,Analyse Table,求法是有所不同的
//   *  统一的接口就是 接受某个通用文法作为参数，计算某个项目集的Closure
//   *
//   *  除此之外，toString方法也是不同的
//   *  但是它们的分析表类型都是一样的
//   *
//   */
//  class LRItem{
//  public:
//	  typedef std::set<LRItem> 		ClosureType;
//	  typedef std::vector<ClosureType>	ClosuresVector;
//	  typedef std::tuple<ClosuresVector,std::set<int>,std::map<std::pair<int,int>,int>> InfoType;
//	  typedef std::map<std::pair<int,int>,std::set<std::tuple<int,int,int>>> CorruptTableType;
//
//	  LRItem();
//	  virtual ~LRItem()=default;
//	  virtual int	getNthSymboleAfterDot(unsigned int j)const=0;
//	  virtual int		    getFirstSymbolAfterDot()const=0;
//	  virtual ClosureType	getClosure()const;
//
//  protected:
//	  const Gramma	&g;
////	  virtual ;
//  };
//
//  /**
//   * 不必将 项目集真正地存入语法之中，因为一个语法的项目集要么不求，要么整体求出，通常是一个计算的过程
//   *不必缓存
//   */
//  template<class ItemType>
//  class LRGrammar:public Gramma{
//  public:
//	  enum{
//		 ACTION_SHIFT_IN,
//		 ACTION_REDUCE,
//		 ACTION_ACCEPT,
//		 ACTION_ERROR,
//	 };
//	  using ClosureType			=	ItemType::ClosureType;
//	  using ClosureVector		=	ItemType::ClosureVector;
//	  using InfoType			=	ItemType::InfoType;
//	  using CorruptTableType	=	ItemType::CorruptTableType;
//
//	  typedef LRGrammar<ItemType> This;
//	  typedef Gramma	Father;
//
//	  int	getExtStart()const;
//	  int   getEnd()const;
//
//	  InfoType getAllClosures()const;
//	  virtual ClosureType    getGoto(const ClosureType & items,int x)const;
//	  virtual CorruptTableType	constructAnaylzeTable()const;
//
//
//
//  protected:
//	  int sstart;//拓广之后的start
//	  int send;//必须定义结束符号
//	  std::string dotString;
//	  static const std::string DOT_STRING;
//  };
//  template <class ItemType>
//  const std::string LRGrammar<ItemType>::DOT_STRING=".";
//  //</block>

  /**
   * LRGramma
   */
  class LRGramma:public Gramma{
  public:
	  typedef LRGramma This;
	  typedef Gramma	Father;
	  typedef std::tuple<int,int,int>	ItemType;
	  typedef std::set<ItemType> 		ClosureType;
	  typedef std::vector<ClosureType>	ClosuresVector;
	  typedef std::tuple<ClosuresVector,std::set<int>,std::map<std::pair<int,int>,int>> InfoType;
	  /**
	   * 这里的ClosureType和教材上类型有所不同，教材上的类型是
	   *  	std::map<std::set<ItemType>,std::set<ItemType>> ClosureType
	   * 这里为了方便，先定义单个项的闭包
	   * 如果是多个项
	   *   Closure(I1,I2,...,In) = Closure(I1) u Closure(I2) u ... u Closure(In)
	   *
	   */


	  LRGramma()=default;

	  //======从一个现有的一般性文法构造一个LRGramma文法
	  LRGramma(const LRGramma & lrg)=default;
	  LRGramma(LRGramma &&lrg)=default;
	  LRGramma(const Gramma& g,int oristart,int oriend,const std::string & strstart);
	  LRGramma(Gramma&& g,int oristart,int oriend,const std::string & strstart);
	  LRGramma(const std::initializer_list<std::pair<int,std::string> > &list,
		   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist,int oristart,int oriend,const std::string & strstart);
	  LRGramma(const GrammaSymbols & gs,const ProductionsType & prods,int oristart,int oriend,const std::string & strstart);
	  LRGramma(const GrammaSymbols & gs,ProductionsType && prods,int oristart,int oriend,const std::string & strstart);
	  LRGramma( GrammaSymbols && gs,const ProductionsType & prods,int oristart,int oriend,const std::string & strstart);
	  LRGramma(GrammaSymbols && gs,ProductionsType && prods,int oristart,int oriend,const std::string & strstart);
	  LRGramma(const Gramma & g,const std::string & oriStart,const std::string &oriEnd,const std::string &extStart);
	  LRGramma(Gramma &&g,const std::string & oriStart,const std::string &oriEnd,const std::string &extStart);

	  /**
	   * return a tuple
	   * 		tuple[0] : contains closures that indexes in tuple[1] & tuple[2] point to
	   * 		tuple[1] : a set of integers, those integers denote the concanical item sets of the gramma
	   * 		tuple[2] : the goto map. the key type is pair<int,int>, the first int denotes a closure in tuple[0],the second is a possible gramma symbol.
	   *
	   *  call toString(tup) to dump those information.
	   */
	  InfoType getAllClosures();
	  /**
	   * added 2017-04-22 12:32:52
	   */
	  LRCorruptTableType constructAnalyzeTableLR0(const InfoType& info);
	  LRCorruptTableType constructAnalyzeTableSLR(const InfoType& info);

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

	  AS_MACRO int		  getFirstSymbolAfterDot(const ItemType & i);
	  int				  getNthSymboleAfterDot(const ItemType & i,unsigned int j);

	  AS_MACRO int	getExtStart()const;
	  AS_MACRO int  getEnd()const;

	  AS_MACRO		std::string toString()const;
	  std::string toString(const ItemType& item)const;
	  std::string toString(const ClosureType& closure)const;
	  std::string toString(const  std::tuple<ClosuresVector,std::set<int>,std::map<std::pair<int,int>,int>> & tups)const;
	  std::string toStringSet(int i)const;
	  AS_MACRO void		  setDotString(const std::string & s);

  public:
	  int sstart;//拓广之后的start
	  int send;//必须定义结束符号
	  std::string dotString;
	  static const std::string DOT_STRING;
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
	  typedef std::map<std::pair<int,int>,int> GotoInfoType;
	  typedef std::tuple<ClosuresVector,std::set<int>,GotoInfoType> InfoType;
	 enum ACTION_TYPE{
		 ACTION_SHIFT_IN,
		 ACTION_REDUCE,
		 ACTION_ACCEPT,
		 ACTION_ERROR,
	 };
	 static std::unordered_map<int,std::string> ACTION_INFO_STRING;
  public:
	 //从LR(0)文法构造LR(1)文法
	 AS_MACRO LR1Gramma(const LRGramma& lrg);
	 AS_MACRO LR1Gramma(LRGramma&& lrg);
	  AS_MACRO LR1Gramma(const std::initializer_list<std::pair<int,std::string> > &list,
		   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist,int oristart,int oriend,const std::string & strstart);
	  LR1Gramma(const Gramma & g,const std::string & oriStart,const std::string &oriEnd,const std::string &extStart);
	  LR1Gramma(Gramma && g,const std::string & oriStart,const std::string &oriEnd,const std::string &extStart);

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
	  LRCorruptTableType constructAnalyzeTable(const InfoType& info);
	  AS_MACRO int		  getFirstSymbolAfterDot(const ItemType & i);
	  int		  getNthSymboleAfterDot(const ItemType & i,unsigned int j);


	  /**
	   * 依据优先级和结合性来消除冲突.
	   *
	   *   看优先级,相同优先级看结合性
	   *    具体规则:
	   *    	保留最高优先级的产生式
	   *    	在同优先级的产生式组中,保留归约式中的一个
	   */
//	  static

	  AS_MACRO std::string toString()const;
	  std::string toString(const ItemType& i)const;
	  std::string toString(const ClosureType& closure)const;
	  std::string toString(const InfoType& info)const;
	  std::string toString(const LRAnalyzeTableType& tableInfo)const;
	  std::string toString(const LRCorruptTableType& tableInfo)const;
  };

  //===========function macrso

  //====class GrammaSymbols
  inline GrammaSymbols::GrammaSymbols( const std::string& emptyString):
		symString(UNDEFINED_STRING)
  {
	  emptyIndex=symString.getAdd(emptyString);
	  symInfo[emptyIndex]=TYPE_EMPTY;
  }
  inline GrammaSymbols::GrammaSymbols(GrammaSymbols &&gsyms):
	  symInfo(std::move(gsyms.symInfo)),
      symString(std::move(gsyms.symString)),
	  emptyIndex(gsyms.emptyIndex)
  {

  }

  inline GrammaSymbols::GrammaSymbols(const std::initializer_list<std::pair<int,std::string> >& list):
		GrammaSymbols()
  {
    auto it=begin(list),itend=end(list);
    for(;it!=itend;it++)
      {
    	this->getAdd(it->second,it->first);
      }
  }

  inline GrammaSymbols & GrammaSymbols::operator=(const GrammaSymbols & gs)
  {
	  new (this) GrammaSymbols(gs);
	  return *this;
  }
  inline GrammaSymbols & GrammaSymbols::operator=(GrammaSymbols && gs)
  {
	  new (this) GrammaSymbols(gs);
	  return *this;
  }
  inline int GrammaSymbols::get(const std::string & s)const
   {
 	  return this->symString.get(s);
   }
  inline int GrammaSymbols::getAdd(const std::string & s,const std::string & type)
   {
 	  return this->getAdd(s, This::TYPES_INFO.getT2(type));
   }

  inline int GrammaSymbols::getAdd(const std::string& sym, int type)
   {
 	  int i=this->symString.getAdd(sym);
 	  auto it=this->symInfo.find(i);
 	  if(it==symInfo.end()){
 		  if(type==TYPE_EMPTY)return this->emptyIndex;
 		  else this->add(i,type);
 	  }
 	  return i;
   }

  inline int GrammaSymbols::addReplace(const std::string& sym,
  		const std::string& type)
  {
	  return this->addReplace(sym, This::TYPES_INFO.getT2(type));
  }

  inline int GrammaSymbols::addReplace(const std::string& sym, int type)
  {
 	  int i=this->symString.getAdd(sym);
	  if(type==TYPE_EMPTY)return this->emptyIndex;
	  else this->add(i,type);
 	  return i;
  }

  inline  int GrammaSymbols::getEmpty()const
   {
     return this->emptyIndex;
   }
  inline int GrammaSymbols::addTerm(const std::string &s)
   {
     return this->getAdd(s,TYPE_TERM);
   }
  inline int GrammaSymbols::addVar(const std::string & s)
   {
     return this->getAdd(s,TYPE_VAR);
   }
  inline int GrammaSymbols::addEmpty(const std::string& s)
  {
	  this->symString.set(this->emptyIndex, s);
	  return this->emptyIndex;
  }
  inline bool GrammaSymbols::isSymbolEmpty(int i)const
  {
    return getSymbolType(i)==TYPE_EMPTY;
  }
  inline bool GrammaSymbols::isSymbolTerm(int i)const
  {
    return getSymbolType(i)==TYPE_TERM;
  }
  inline bool GrammaSymbols::isSymbolVar(int i)const
  {
    return getSymbolType(i)==TYPE_VAR;
  }
  inline bool GrammaSymbols::isSymbolUndefined(int i)const
  {
    return getSymbolType(i)==TYPE_UNDEFINED;
  }
  inline int GrammaSymbols::getSymbolType(int i)const
  {
    TypeInfo::const_iterator it=symInfo.find(i);
    if(it!=symInfo.end())return it->second;
    return TYPE_UNDEFINED;
  }
  /**
   * if index already exist, return a index may be used
   */
  inline void GrammaSymbols::add(int index,int type)
  {
	  this->symInfo[index]=type;
  }
  //  void GrammaSymbols::deleteNo(int no)
  //  {
  //    if(no==EMPTY_INDEX || no==UNDEFINED_INDEX)return;//you can not delete EMPTY
  //    TypeInfo::iterator it=symInfo.find(no);
  //    if(it!=symInfo.end())
  //      {
  //    	auto itstr=symString.find(no);
  //    	auto itsym=stringSym.find(itstr->second);
  //
  //    	if(no==max)max--;
  //    	symInfo.erase(it);
  //    	symString.erase(itstr);
  //    	stringSym.erase(itsym);
  ////    	itstr->second.std::string::~string();
  ////    	itsym->first.std::string::~string();
  //      }
  //  }
  inline  const std::string& GrammaSymbols::getString(int i)const
    {
  	  return this->symString.get(i);
    }
  inline  std::string	GrammaSymbols::toString()const
    {
      std::string s;
      std::for_each(std::begin(this->symInfo),std::end(this->symInfo),[&s,this](const TypeInfo::value_type &item){
        s+=std::string("( ") +std::to_string(item.first) + " , " + GrammaSymbols::TYPES_INFO.getT1(item.second) +" , "+ this->getString(item.first) + " ) \n";
      });
      return s;
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
  inline void GrammaSentence::push_back(int i)
  {
	  this->syms.push_back(i);
  }



  //=====class GrammaProduction


  //======class Gramma
  inline Gramma::Gramma(std::initializer_list<std::string> list)
  {
	  for(auto &s:list)
		  this->addProduction(s);
  }
  inline Gramma::Gramma(const std::vector<std::string> &list)
  {
	  for(auto &s:list)
		  this->addProduction(s);
  }

  inline void Gramma::addProduction(int i,const GrammaSentence &gs)
  {
    ProductionsType::iterator it=this->prods.find(i);
    if(it==this->prods.end())
      {
	std::vector<GrammaSentence> v;
	v.push_back(gs);
	this->prods[i]=std::move(v);
      }
    else{
	it->second.push_back(gs);
    }
  }
  inline void Gramma::addProduction(int i,GrammaSentence &&gs)
  {
    ProductionsType::iterator it=this->prods.find(i);
    if(it==this->prods.end())
      {
	std::vector<GrammaSentence> v;
	v.push_back(gs);
	this->prods[i]=std::move(v);
      }
    else{
	it->second.push_back(gs);
    }
  }

  inline void Gramma::addProduction(const std::string& head, const std::vector<std::string>& gs)
  {
	  //headi = this->gsyms.addReplace(head,VAR)
	  int ihead=this->gsyms.addReplace(head,GrammaSymbols::TYPE_VAR);

	  //for s in gs, si = this->gsyms.getAdd(s,TERM)
	  GrammaSentence grammars;
	  for(auto & s:gs)
	  {
		  grammars.push_back(this->gsyms.getAdd(s, GrammaSymbols::TYPE_TERM));
	  }
	  this->addProduction(ihead, std::move(grammars));

  }

  inline void Gramma::addProduction(const std::string& head,const std::initializer_list<std::string>& gs)
  {
	  //headi = this->gsyms.addReplace(head,VAR)
	  int ihead=this->gsyms.addReplace(head,GrammaSymbols::TYPE_VAR);

	  //for s in gs, si = this->gsyms.getAdd(s,TERM)
	  GrammaSentence grammars;
	  for(auto & s:gs)
	  {
		  grammars.push_back(this->gsyms.getAdd(s, GrammaSymbols::TYPE_TERM));
	  }
	  this->addProduction(ihead, std::move(grammars));
  }

  inline void Gramma::addProduction(const std::string& all)
  {
	  std::stringstream ss(all);
	  std::string s;
	  ss>>s;
	  int ihead=this->gsyms.addReplace(s, GrammaSymbols::TYPE_VAR);
	  GrammaSentence grammars;
	  while(!ss.eof())
	  {
		  ss>>s;
		  grammars.push_back(this->gsyms.getAdd(s, GrammaSymbols::TYPE_TERM));
	  }
	  this->addProduction(ihead, std::move(grammars));
  }
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

  inline typename Gramma::LRAnalyzeTableType Gramma::convertCorruptToStandardSimply(
  		const LRCorruptTableType& table)
  {
	  LRAnalyzeTableType goodTable;
	  for(auto &p : table)
	  {
		  goodTable[p.first]=*(p.second.begin());
	  }
	  return goodTable;
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
  std::string Gramma::toString(int i,int j)const
  {
	  return toString(i) + " -> " + this->toString(prods.at(i)[j]);
  }
  //============class LRGramma
  inline LRGramma::LRGramma(const Gramma& g,int oristart,int oriend,const std::string & strstart):
 		Gramma(g),send(oriend),dotString(".")
  {
 	 sstart = gsyms.addVar(strstart);
 	 GrammaSentence gs={oristart};
 	 addProduction(sstart,std::move(gs));
  }
 inline LRGramma::LRGramma(Gramma&& g,int oristart,int oriend,const std::string & strstart):
 		Gramma(std::move(g)),send(oriend),dotString(".")
  {
 	 sstart = gsyms.addVar(strstart);
 	 GrammaSentence gs={oristart};
 	 addProduction(sstart,std::move(gs));
  }
  inline LRGramma::LRGramma(const Gramma& g, const std::string& oriStart,
  		const std::string& oriEnd, const std::string& extStart):
		Gramma(g),
		send(this->gsyms.getAdd(oriEnd,GrammaSymbols::TYPE_TERM)),
		dotString(".")

  {
	  this->sstart = this->gsyms.getAdd(extStart,GrammaSymbols::TYPE_VAR);
	  this->Father::addProduction(extStart,{oriStart});
  }

  inline LRGramma::LRGramma(Gramma&& g, const std::string& oriStart,
  		const std::string& oriEnd, const std::string& extStart):
  				Gramma(std::move(g)),
				send(this->gsyms.getAdd(oriEnd,GrammaSymbols::TYPE_TERM)),
				dotString(".")
  {
	  this->sstart = this->gsyms.getAdd(extStart,GrammaSymbols::TYPE_VAR);
	  this->Father::addProduction(extStart,{oriStart});
  }


  int  LRGramma::getFirstSymbolAfterDot(const ItemType & i)
  {
 	 return getNthSymboleAfterDot(i,1);
  }
  int			LRGramma::getExtStart()const
  {
	  return sstart;
  }
  int LRGramma::getEnd()const
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
 LR1Gramma::LR1Gramma(const LRGramma& lrg):
		 LRGramma(lrg)
	{
	}

 LR1Gramma::LR1Gramma(LRGramma&& lrg):
		 LRGramma(lrg)
	{}
  LR1Gramma::LR1Gramma(const std::initializer_list<std::pair<int,std::string> > &list,
  		   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist,int oristart,int oriend,const std::string & strstart):
  				   LRGramma(list,prodlist,oristart,oriend,strstart)
	{

	}

  inline LR1Gramma::LR1Gramma(const Gramma& g, const std::string& oriStart,
  		const std::string& oriEnd, const std::string& extStart):
  				Father(g,oriStart,oriEnd,extStart)
  {
  }

  inline LR1Gramma::LR1Gramma(Gramma&& g, const std::string& oriStart,
  		const std::string& oriEnd, const std::string& extStart):
		Father(std::move(g),oriStart,oriEnd,extStart)
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
