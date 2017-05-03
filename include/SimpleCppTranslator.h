/*
 * @brief	一个cpp翻译器的实现
 * SimpleCppTranslator.h
 *
 *  Created on: 2017年4月28日
 *      Author: 13774
 *
 */

#ifndef INCLUDE_SIMPLECPPTRANSLATOR_H_
#define INCLUDE_SIMPLECPPTRANSLATOR_H_

#include <x2def.h>
#include <numeric>
#include <GrammarTranslateUtils.h>
#include <MutualMap.h>
#include <utility>
#include <tuple>
#include <StringUtils.h>

namespace x2{
	/**
	 * @brief	四元式的定义
	 */
	template <class OporType,class OpandType>
	class TranslatedQuadTuple:public std::tuple<OporType,OpandType,OpandType,OpandType>{
	public:
		typedef TranslatedQuadTuple This;
		typedef std::tuple<OporType,OpandType,OpandType,OpandType>	Father;
		typedef OporType OperatorType;
		typedef	OpandType OprandType;

		TranslatedQuadTuple()=default;
		TranslatedQuadTuple(const TranslatedQuadTuple &)=default;
		TranslatedQuadTuple(TranslatedQuadTuple &&)=default;
		TranslatedQuadTuple(Father &&);
		TranslatedQuadTuple(const Father &);


		OperatorType &	getOperator();
		const OperatorType & getOperator()const;

		OprandType	& getNthOprand(int i);
		const OprandType & getNthOprand(int i)const;


	};
	/**
	 * @brief	符号表中的每一项的内容
	 */
	class SymbolAttributes:public x2::HashMap<int,int>{
	public:
		typedef SymbolAttributes This;
		typedef x2::HashMap<int,int>	Father;
		SymbolAttributes()=default;
		SymbolAttributes(const SymbolAttributes &)=default;
		SymbolAttributes(SymbolAttributes &&)=default;
		SymbolAttributes(const Father & f);
		SymbolAttributes(Father &&f);
	};
	/**
	 * @brief	符号表
	 * @detail	必须为每个符号生成唯一的标识,使用std::string = name作为查找类型
	 * 			同时还提供一个全局的
	 */
	class SymbolTable:public std::vector<SymbolAttributes>{
	public:

		/**
		 * @brief	检查符号表中是否存在一个已经定义的符号
		 */
		bool isSymbolDefined(const std::string & sym)const;

	};
	/**
	 * @brief	符号表集合
	 * @detail	提供一个genTemp方法,为临时变量生成索引名称
	 * 			同时还承担一个stringPool的角色
	 */
	class SymbolTableList:public std::vector<SymbolTable>{
	public:
		typedef SymbolTableList This;
		typedef std::vector<SymbolTable> Father;
		using SizeType=Father::size_type;

		SymbolTableList();
		/**
		 * @brief	生成一个临时的符号名
		 * @detail	生成规则: 使用__temp_xx作为生成名,用下标xx作为尾部
		 * 			@delete{每次插入一个符号的时候,都检查其是否符号__temp_xx的情况,其中xx必须作为一个有效的数字
		 * 			对于检查来说,如果xx小于当前下标,则必须检查它与局部表是否冲突,如果冲突,则这个变量名是不被接受的}
		 * 			每次插入一个符号是,检查其名称是否符合__temp_xx类型,如果符合,直接提示错误.这样无疑会大大减少genTemp的设计复杂度
		 */
		std::string genTemp();
//		/**
//		 * @brief	收回一个符号名用于分配
//		 * @deprecated 没有意义,因为临时符号名可以无限大
//		 */
//		void		withdrawTemp(const std::string & s);

		/**
		 *
		 */
		SizeType			insertString(const std::string & s);
		SizeType			insertString(std::string && s);
		const std::string & getString(SizeType i)const;

	protected:
		/**
		 * @brief	检查符号是否和临时符号冲突
		 * @detail	如果符合临时符号的命名方式,则返回xx部分
		 * @return  如果不符合就返回-1,否则返回大于0的下标值
		 */
		static long long		checkSymbolTempIndex(const std::string & sym);
		static std::string		TEMP_PREFIX;

		long long			 		tempIndex;
//		std::vector<std::string>	stringPool;//stringPool, uiy使用indexedMap来实现
		x2::IndexedMap<std::string>	stringPool;
	};

	class SimpleCppTranslator : public DemoTranslator{
	public:
		typedef SimpleCppTranslator 					This;
		typedef DemoTranslator							Father;

		typedef std::tuple<SymbolAttributes,int,int>	SymStackElementType;
		typedef TranslatedQuadTuple<int,SymbolAttributes>	CodeType;
		using StreamWordType=Father::StreamWordType;
		using SizeType=std::size_t;

		enum ATTRI_TYPES{
			TYPE_DEFAULT=0,
			TYPE_TYPE=1,
			TYPE_INT,
			TYPE_STRING,
			TYPE_VOID,
			TYPE_CHAR,
			TYPE_EMPTY,/*indicate that this SymbolAttribute is not used*/
			TYPE_DEPEND, /*TYPE been set at next*/
		};
		enum REFT_TYPES{
			REF_SELF,
			REF_TABLE,
			REF_NAME
		};

		SimpleCppTranslator(const LR1Gramma & grammar,const LR1Gramma::InfoType & closuresInfo,
					const LR1Gramma::LRAnalyzeTableType & analyzeTable,int endingSym,int initState=0);
		SimpleCppTranslator(SimpleCppTranslator && simple)=default;
		SimpleCppTranslator(const SimpleCppTranslator & simple)=default;

	protected:
		virtual void onInit();
		virtual void onReduce(int i,int j);

		virtual void onNoGoto();

		virtual void onNoAction();

		virtual void onAccept();

		virtual void onShift();
		virtual void onInputNotTerminator();

		virtual void onEnd();

		std::string	dumpAttribute(const SymbolAttributes & attr)const;
		/**
		 * @param	no		行号
		 * @param	code	代表代码的四元组
		 */
		std::string dumpCode(std::size_t no)const;
		/**
		 * @brief	输出code的四元组形式
		 * @detail	输出code的四元组形式,同时标上行号
		 */
		std::string dumpCodes()const;

		//helper functions
		/**
		 * @detail get the Nth lexical pair offset from 0.
		 */
		const StreamWordType & getNthInput(std::size_t i)const;

		/**
		 * @detail	eval(hex|decimal|bin)
		 */
		int					evalNumber(const StreamWordType & word)const;

		int					getType(SymStackElementType& attri);

		SymbolAttributes &  getRefAttributes(SymbolAttributes & attr);
	protected:
//		using	Father::grammar;
		SymbolTableList										symTableList;
		std::stack<SymStackElementType>						symStack;
		std::vector<CodeType>								genedCodes;
		x2::HashMap<int,std::pair<int,int>>					symTableSwitchMap;
		std::stack<SizeType>								terminatorIndexStack;
		int curSymTableIndex;

//		/**
//		 * @brief	当前输入流的词法流计数器
//		 * @detail	每次遇到一个终结符,并且将其shift进入栈的时候,发生计数器的增加.
//		 */
//		int curInputWordIndex;
		/**
		 * @brief 输入流的顺序计数器
		 * @detail	它不是当前输入流的index计数,而是总计的翻译过的输入流的计数
		 */
		int inputStreamIndex;


		x2::IndexedMap<std::string>							codesMap;
		x2::IndexedMap<std::string>							attrsMap;
	public:
		const static std::string UNRECOGNIZED_DIRECTIVE;

	};

//==========functions definitions(inline)
	//===class TranslatedQuadTuple
	template<class OporType, class OpandType>
	inline TranslatedQuadTuple<OporType, OpandType>::TranslatedQuadTuple(
			Father&& tuple):
			Father(std::move(tuple))
	{
	}

	template<class OporType, class OpandType>
	inline TranslatedQuadTuple<OporType, OpandType>::TranslatedQuadTuple(
			const Father& tuple):
			Father(tuple)
	{
	}
	//===class SymbolAttributes

	inline SymbolAttributes::SymbolAttributes(const Father& f):
			Father(f)
	{
	}

	inline SymbolAttributes::SymbolAttributes(Father&& f):
			Father(std::move(f))
	{
	}


	//====class SymbolTable
	inline SymbolTableList::SymbolTableList():
			tempIndex(1) /*0 is reserved*/
	{
	}
	inline std::string SymbolTableList::genTemp()
	{
		return TEMP_PREFIX + std::to_string(this->tempIndex++);
	}
	inline typename SymbolTableList::SizeType SymbolTableList::insertString(const std::string& s)
	{
		return (SizeType)this->stringPool.getAdd(s);
	}

	inline typename SymbolTableList::SizeType SymbolTableList::insertString(std::string&& s)
	{
		return (SizeType)this->stringPool.getAdd(std::move(s));
	}
	inline const std::string& SymbolTableList::getString(SizeType i) const
	{
		return this->stringPool.get((int)i);
	}


	inline long long SymbolTableList::checkSymbolTempIndex(
			const std::string& sym)
	{
		if(sym.size() >= TEMP_PREFIX.size() && sym.compare(0,TEMP_PREFIX.size(), TEMP_PREFIX)==0)
		{
			//check if the end are all digits
			auto i=TEMP_PREFIX.size();
			long long val=0;
			for(;i<sym.size() && std::isdigit(sym[i]);i++) val = val*10 + (sym[i]-'0');
			if(i < sym.size())return -1;
			return val;

		}
		return -1;
	}
	//======class SimpleCppTranslator
	inline SimpleCppTranslator::SimpleCppTranslator(const LR1Gramma& grammar,
			const LR1Gramma::InfoType& closuresInfo,
			const LR1Gramma::LRAnalyzeTableType& analyzeTable, int endingSym,
			int initState):
					Father(grammar,closuresInfo,analyzeTable,endingSym,initState),
					curSymTableIndex(0),
//					curInputWordIndex(0),
					inputStreamIndex(-1),
					codesMap(UNRECOGNIZED_DIRECTIVE)
	{
//		this->genedCodes.push_back(CodeType({codesMap.getAdd("NOP"),SymbolAttributes(),SymbolAttributes(),SymbolAttributes()}));//an empty code
		this->attrsMap.add("type");
		this->attrsMap.add("val");
		this->attrsMap.add("refType");//records type,
						//reference to a symbol in the table, by name (refType=1),table index & symbol index are provided by type & val
						//if type is string, then it should be in the table(means that it is an anonymous symbol) (refType=1)
						//or reference to an immediate value by the attribute itself, if type is int/char,then val is immediate val(refType=0)
						// if refType is not present,then default to refType=0
						//and there is the third refType, reference to a variable name, not directly to a specific index in the table
						//(by refType=REF_NAME), its val is a string index in the stringPool.
		//Rules:
		//	if refType=REF_TABLE:
//				then  type & val must be present.
		//	if	type=TYPE_TYPE
		//		then val must be present
		//	if  type=TYPE_STRING
		//		val must be present, pointing to the real value
		this->attrsMap.add("name");
	}
	inline void SimpleCppTranslator::onInit()
	{
		new (&this->symStack) decltype(this->symStack)();
//		std::cout << this->symTableList.size() << std::endl;
		this->symTableList.push_back(SymbolTable());
		this->curSymTableIndex = (int)this->symTableList.size() - 1;

		//=== clear terminatorIndexStack
		new (&this->terminatorIndexStack) decltype(this->terminatorIndexStack);

//		this->curInputWordIndex=0;
		this->inputStreamIndex++;
	}

	inline void SimpleCppTranslator::onReduce(int i, int j)
	{
		this->Father::onReduce(i,j);
//		std::cout << "top of terminator index is "<<this->terminatorIndexStack.top() <<
//				", size is "<<this->terminatorIndexStack.size() << std::endl;
		if(this->grammar.gsyms.get("TYPE")==i)
		{
			if(j==0)//TYPE -> int
			{
				this->symStack.push({
					SymbolAttributes({
						{this->attrsMap.get("type"),TYPE_TYPE},
						{this->attrsMap.get("val"),TYPE_INT}
					}),
					(int)this->symTableList[curSymTableIndex].size(),
					(int)this->genedCodes.size()

				});
				this->terminatorIndexStack.pop();
			}
		}else if(this->grammar.gsyms.get("EXPRESSION")==i){
			if(j==7)//EXPRESSION -> number
			{
				this->symStack.push({
					SymbolAttributes({
						{this->attrsMap.get("refType"),REF_SELF},
						{this->attrsMap.get("type"),TYPE_INT},
						{this->attrsMap.get("val"),this->evalNumber(
								this->currentInput->getStreamWord(this->terminatorIndexStack.top())
						)}}), //由立即数推到出文法符号,必然是在某一步归约过程中实现的
						(int)this->symTableList[curSymTableIndex].size(),
						(int)this->genedCodes.size()
					});
				this->terminatorIndexStack.pop();
			}else if(j==8){//EXPRESSION -> stringval
				SymbolTable & curTable=this->symTableList[curSymTableIndex];
				curTable.push_back(SymbolAttributes({
					{this->attrsMap.get("val"),this->symTableList.insertString(
							this->currentInput->getStreamWord(this->terminatorIndexStack.top()).first)},
					{this->attrsMap.get("type"),TYPE_STRING}
				}));
				this->symStack.push({
					SymbolAttributes({
						{this->attrsMap.get("refType"), REF_TABLE},
						{this->attrsMap.get("type"),this->curSymTableIndex},
						{this->attrsMap.get("val"),(int)curTable.size() - 1},
					}),
					(int)this->symTableList[curSymTableIndex].size(),
					(int)this->genedCodes.size()
				});

				this->terminatorIndexStack.pop();

			}else if(j==11){//EXPRESSION -> id
//				std::cout << "EXPRESSION -> id, id is "<<this->currentInput->getStreamWord(this->terminatorIndexStack.top()).first<<std::endl;
				this->symStack.push({
					SymbolAttributes({
						{this->attrsMap.get("refType"),REF_NAME},
						{this->attrsMap.get("type"),TYPE_DEPEND},
						{this->attrsMap.get("val"),this->symTableList.insertString(
							this->currentInput->getStreamWord(this->terminatorIndexStack.top()).first)}
					}),
					(int)this->symTableList[curSymTableIndex].size(),
					(int)this->genedCodes.size()
				});
				//id的type取决于 1:声明  声明不存在 2:所要赋值的类型
				//type=TYPE_DEPEND的时候, 需要在下一个能够确定的时候将其确定

				this->terminatorIndexStack.pop();
			}else if(j==0){//EXPRESSION -> CALL_STATEMENT
				//nothing to do
			}else if(j==2){//EXPRESSION -> EXPRESSION - EXPRESSION
				this->terminatorIndexStack.pop();
				SymStackElementType s2=this->symStack.top();this->symStack.pop();
				SymStackElementType s1=this->symStack.top();this->symStack.pop();
				int type1=this->getType(s1);
				int type2=this->getType(s2);
				if(type1==TYPE_DEPEND)
				{
					if(type2!=TYPE_DEPEND)
					{
						type1=type2;
					}
				}else if(type2==TYPE_DEPEND){
					if(type1!=TYPE_DEPEND)
						type2=type1;
				}
				if(type1!=type2){
					std::cout << "Error : conficting types." << std::endl;
					this->terminate();
				}else{
					this->symStack.push(SymStackElementType({
						SymbolAttributes({
								{this->attrsMap.get("refType"),REF_TABLE},
								{this->attrsMap.get("type"),this->curSymTableIndex},
								{this->attrsMap.get("val"),(int)this->symTableList[curSymTableIndex].size()}
						}),
						(int)this->symTableList[curSymTableIndex].size(),
						(int)this->genedCodes.size()
					}));

					this->genedCodes.push_back(CodeType({
						codesMap.getAdd("-"),
						std::move(std::get<0>(s1)),
						std::move(std::get<0>(s2)),
						SymbolAttributes({
									{this->attrsMap.get("refType"),REF_TABLE},
									{this->attrsMap.get("type"),this->curSymTableIndex},
									{this->attrsMap.get("val"),(int)this->symTableList[curSymTableIndex].size()}
						})
					}));


					this->symTableList[curSymTableIndex].push_back(SymbolAttributes({
						{this->attrsMap.get("name"),  this->symTableList.insertString(this->symTableList.genTemp()) },//临时符号
						{this->attrsMap.get("type"), type1}
					}));
				}
			}

		}else if(this->grammar.gsyms.get("VAR_DECLARATION")==i){
			if(j==1)//VAR_DECLARATION -> TYPE id = EXPRESSION
					//EXPRESSION很有可能已经合并了某些项,所以寻址方式不能依靠offset
					//只能依靠栈中的位置来辨别,当shift的时候,同时维持一个相对于wordStream中的下标的栈H
					//在reduce的时候,每移除一个终结符,就在这个栈中移除一个下标.
				//为了实现这个目标.必须
				//	1.在pop的过程中检查变量而的类型,仅当类型是terminator时,在H中pop
				//	2.在shift的时候,H中push当前的下标
				//有没有另外的实现方法?
				//	因为表达式终结符的个数是可见的,所以H可以在子类中实现
			{
				SymStackElementType s2=this->symStack.top();this->symStack.pop();
				SymStackElementType s1=this->symStack.top();this->symStack.pop();
				this->terminatorIndexStack.pop();
				int idindex=this->terminatorIndexStack.top();this->terminatorIndexStack.pop();
//				std::cout << "index of id is "<< idindex << " and string is "
//						<< this->currentInput->getStreamWord(idindex).first<<std::endl;
				int type1=this->getType(s1);
				int type2=this->getType(s2);
				if(type2==TYPE_DEPEND)
				{
					this->getRefAttributes(std::get<0>(s2))[this->attrsMap.get("type")]=type1; //更改ref的type
					type2=type1;
				}
				if(type2==type1)
				{
					this->symStack.push({
						SymbolAttributes({
							{this->attrsMap.get("type"),TYPE_VOID}
						}),
						(int)this->symTableList[curSymTableIndex].size(),
						(int)this->genedCodes.size()
					});
					SymbolTable & curTable=this->symTableList[curSymTableIndex];
					curTable.push_back(SymbolAttributes({
						{this->attrsMap.get("name"),this->symTableList.insertString(this->currentInput->getStreamWord(idindex).first)},
						{this->attrsMap.get("type"),std::get<0>(s2).at(this->attrsMap.get("type"))}
					}));
					this->genedCodes.push_back(CodeType({
						codesMap.getAdd("="),
						SymbolAttributes({
							{this->attrsMap.get("refType"),REF_TABLE},
							{this->attrsMap.get("type"),this->curSymTableIndex},
							{this->attrsMap.get("val"), (int)curTable.size() - 1}}),
						std::move(std::get<0>(s2)),
						SymbolAttributes({{this->attrsMap.get("type"), TYPE_EMPTY}})
					}));
				}else{
					std::cout << "Error : conficting types." << std::endl;
					this->terminate();
				}
			}
		}else if(this->grammar.gsyms.get("CALL_STATEMENT")==i){
			if(j==0)//CALL_STATEMENT -> id ( EXPRESSION , EXPRESSION )
			{
				SymStackElementType s2=this->symStack.top();this->symStack.pop();
				SymStackElementType s1=this->symStack.top();this->symStack.pop();
				this->terminatorIndexStack.pop();
				this->terminatorIndexStack.pop();
				this->terminatorIndexStack.pop();
				/**
				 * 此处产生重复,假设 printf出现多次,则每一次都会插入,这是不正确的
				 * UPDATE 1:已经修改成IndexedMap
				 */
				int idindex=this->symTableList.insertString(this->currentInput->getStreamWord(
								this->terminatorIndexStack.top()).first);this->terminatorIndexStack.pop();
				int tempid=this->symTableList.insertString(this->symTableList.genTemp());
				int tempidIndex=(int)this->symTableList[curSymTableIndex].size();

				this->symStack.push(SymStackElementType({
						SymbolAttributes({
							{this->attrsMap.get("refType"),REF_TABLE},
							{this->attrsMap.get("type"),this->curSymTableIndex},
							{this->attrsMap.get("val"),tempidIndex},
						}),
						(int)this->symTableList[curSymTableIndex].size(),
						(int)this->genedCodes.size()
				}));
				this->symTableList[curSymTableIndex].push_back(SymbolAttributes({
					{this->attrsMap.get("type"),TYPE_DEPEND},
					{this->attrsMap.get("name"),tempid}
				}));
				this->genedCodes.push_back(CodeType({
					this->codesMap.getAdd("param"),
					std::move(std::get<0>(s2)),
					SymbolAttributes({ {this->attrsMap.get("type"), TYPE_EMPTY} }),
					SymbolAttributes({ {this->attrsMap.get("type"), TYPE_EMPTY} })
				}));
				this->genedCodes.push_back(CodeType({
					this->codesMap.getAdd("param"),
					std::move(std::get<0>(s1)),
					SymbolAttributes({ {this->attrsMap.get("type"), TYPE_EMPTY} }),
					SymbolAttributes({ {this->attrsMap.get("type"), TYPE_EMPTY} })
				}));

				this->genedCodes.push_back(CodeType({
					this->codesMap.getAdd("call"),
					SymbolAttributes({ {this->attrsMap.get("refType"), REF_NAME}, {this->attrsMap.get("val"),idindex} }),
					SymbolAttributes({ {this->attrsMap.get("type"), TYPE_EMPTY} }),
					SymbolAttributes({ {this->attrsMap.get("refType"), REF_TABLE}, {this->attrsMap.get("type"),this->curSymTableIndex}, {this->attrsMap.get("val"),tempidIndex} })
				}));

//				this->terminate();

			}
		}else if(this->grammar.gsyms.get("FUN_DEFINITION")==i){
			if(j==0)//FUN_DEFINITION -> TYPE id ( TYPE id ) { VAR_DECLARATION ; ASSIGNMENT ; RETURN_STATEMENT ; }
			{
				SymStackElementType s5=this->symStack.top();this->symStack.pop();
				SymStackElementType s4=this->symStack.top();this->symStack.pop();
				SymStackElementType s3=this->symStack.top();this->symStack.pop();
				SymStackElementType s2=this->symStack.top();this->symStack.pop();
				SymStackElementType s1=this->symStack.top();this->symStack.pop();

				//从s3.symStart到s2.symStart,分离出一个符号表 T1
				int t1=(int)this->symTableList.size(); //s3.symStart   s2.symStart, base1=s3.symStart
				int t2=t1+1; // s2.symStart  currentSymStart,   base2=s2.symStart

				this->symTableList.push_back(SymbolTable());
				this->symTableList.push_back(SymbolTable());

				//从s2.symStart到当前分离出第二个符号表T2


				//T1的Father设为T2,T2的Father设为当前符号表


				//在switch映射表中压入 s3.codeStart, push T1
				//				s2.codeStart pop
				//			压入  s2.codeStart push T2
				//			压入 当前代码curCodeStart,pop

				//在当前表中添加id.name的定义,TYPE=FUNC RTYPE=s1.type

				//遍历下标范围内的所有refType=REF_TABLE类型, 然后更改type=Ti(i=1,2),val不必更改
				//Table类型不如使用 <i,j>表示一个连续的范围, i,j(end) 指向总的符号表,这种设计避免了无用的复制或者移动
				//一个数组 tuple<int,int,int>		[0]  father,  [1],[2]=i,j (i,j只是标明查找的下标范围,不包括j)

				//===在最后进行存储分配阶段,
			}
		}else if(this->grammar.gsyms.get("RETURN_STATEMENT")==i){
			if(j==0)//RETURN_STATEMENT -> return EXPRESSION
			{
				this->genedCodes.push_back(CodeType({
					this->codesMap.getAdd("return"),
					std::get<0>(this->symStack.top()),
					SymbolAttributes({ {this->attrsMap.get("type"), TYPE_EMPTY} }),
					SymbolAttributes({ {this->attrsMap.get("type"), TYPE_EMPTY} })
				}));
			}
		}else if(this->grammar.gsyms.get("ASSIGNMENT")==i){
			if(j==0)//ASSIGNMENT -> id = EXPRESSION
			{
				SymStackElementType s1=this->symStack.top();this->symStack.pop();
				this->terminatorIndexStack.pop();
				int idnameindex=this->symTableList.insertString(this->currentInput->getStreamWord(this->terminatorIndexStack.top()).first);this->terminatorIndexStack.pop();

				int type1=this->getType(s1);
				this->symStack.push({
					SymbolAttributes({
						{this->attrsMap.get("refType"),REF_NAME},
						{this->attrsMap.get("type"),type1},
						{this->attrsMap.get("val"),idnameindex}
					}),
					(int)this->symTableList[curSymTableIndex].size(),
					(int)this->genedCodes.size()
				});

				this->genedCodes.push_back(CodeType({
					codesMap.getAdd("="),
					std::move(std::get<0>(s1)),
					SymbolAttributes({{this->attrsMap.get("type"), TYPE_EMPTY}}),
					SymbolAttributes({
						{this->attrsMap.get("refType"),REF_NAME},
						{this->attrsMap.get("type"),type1},
						{this->attrsMap.get("val"),idnameindex}
					})
				}));
			}
		}

	}

	inline void SimpleCppTranslator::onNoGoto()
	{
	}

	inline void SimpleCppTranslator::onNoAction()
	{
	}

	inline void SimpleCppTranslator::onAccept()
	{
		this->Father::onAccept();
	}

	inline void SimpleCppTranslator::onShift()
	{
//		std::cout << "shift terminator index " << this->currentInput->getOffset() << std::endl;
		this->terminatorIndexStack.push(this->currentInput->getOffset());
	}

	inline void SimpleCppTranslator::onInputNotTerminator()
	{
	}

	inline void SimpleCppTranslator::onEnd()
	{
		this->Father::onEnd();

		std::cout << "Generated Codes"<<std::endl<<this->dumpCodes() << std::endl;
	}
	/**
	 * @detail 处理流程: 查找refType,如果没有找到或者找到是REF_SELF,显示val的值
	 * 					如果是REF_CUR_TABLE, 则显示val的值对应的项的name的下的string值
	 * 					如果是REF_NAME, 使用VAL的值在字符串池中获取一个字符串即可.
	 */
	inline std::string SimpleCppTranslator::dumpAttribute(
			const SymbolAttributes& attr) const
	{
		std::string s;
		int refTypeIndex=this->attrsMap.get("refType");
		int valIndex=this->attrsMap.get("val");
		int	typeIndex=this->attrsMap.get("type");

		auto itval=attr.find(valIndex);
		auto itref=attr.find(refTypeIndex);
		auto ittype=attr.find(typeIndex);
		if(ittype!=attr.end() && ittype->second==TYPE_EMPTY)
		{
			s="_";
		}else if(itref==attr.end() || itref->second==REF_SELF){
			if(itval==attr.end())
				s="MISSING_VAL";
			else if(ittype==attr.end() || ittype->second==TYPE_INT)
			{
				s=std::to_string(itval->second);
			}else if(ittype->second == TYPE_CHAR){
				s=x2::toString((char)itval->second);
			}
		}else if(itref->second == REF_NAME){
			if(itval==attr.end())
				s="MISSING_REF_NAME";
			else
				s=this->symTableList.getString(itval->second);
		}else{//REF_CUR_TABLE,找到变量名即可
			if(itval==attr.end() || ittype == attr.end())
				s="MISSING_REF_IN_TABLE";
			else{
				const SymbolAttributes& refAttr=this->symTableList.at(ittype->second).at(itval->second);
				int nameIndex=this->attrsMap.get("name");
				auto itRealTypeIndex=refAttr.find(typeIndex);
				if(itRealTypeIndex != refAttr.end()  && itRealTypeIndex->second == TYPE_STRING)
				{
					auto itRealValueIndex=refAttr.find(valIndex);
					if(itRealValueIndex != refAttr.end())
						s=std::string("\"")+this->symTableList.getString(itRealValueIndex->second)+"\"";
					else
						s="MISSING_STRING_VAL";
				}else{
					auto itNameStringIndex=refAttr.find(nameIndex);
					if(itNameStringIndex == refAttr.end())
						s="UNNAMED_REF";
					else
						s=this->symTableList.getString(itNameStringIndex->second);
				}
			}
		}
//		std::cout << "attr is "<< s << std::endl;
		return s;
	}
	inline std::string SimpleCppTranslator::dumpCode(std::size_t no) const
	{
		const CodeType & code=this->genedCodes.at(no);
		std::string s=std::to_string(no) + ": (" +
				this->codesMap.get(std::get<0>(code)) + ", "+this->dumpAttribute(std::get<1>(code))+", "+
				this->dumpAttribute(std::get<2>(code)) + ", "+this->dumpAttribute(std::get<3>(code))
				+")\n";
//		std::cout << "code is "<<s <<std::endl;
		return s;
	}
	inline std::string SimpleCppTranslator::dumpCodes() const
	{
		std::string s;
		for(SizeType i=0;i<this->genedCodes.size();i++)
		{
//			std::cout << "Line is "<< i <<std::endl;
			s+=this->dumpCode(i);
		}
		return s;
	}

	inline const typename SimpleCppTranslator::StreamWordType& SimpleCppTranslator::getNthInput(
			std::size_t i) const
	{
		//EXPRESSION -> number
		//EXPRESSION -> stringval
		return this->currentInput->getStreamWord(i);
	}
	inline int SimpleCppTranslator::evalNumber(const StreamWordType& word) const
	{
		int base;
		int val=0;
		switch(word.second)
		{
		case LexicalParser::TYPE_NUMBER_DECIMAL:
			base=10;break;
		case LexicalParser::TYPE_NUMBER_BIN:
			base=2;break;
		case LexicalParser::TYPE_NUMBER_HEX:
			base=16;break;
		default:
			return 0;
		}
		for(char ch:word.first)
		{
			if(ch>='0' && ch<='9') val = base*val + (ch-'0');
			else if(ch>='a' && ch<='f') val=base*val + (ch-'a');
			else if(ch>='A' && ch<='F') val=base*val + (ch-'A');
		}
		return val;
	}


	inline int SimpleCppTranslator::getType(SymStackElementType& attri)
	{
		int typeIndex=this->attrsMap.get("type");
		int valIndex = this->attrsMap.get("val");

		auto & refAttr=this->getRefAttributes(std::get<0>(attri));

		int thistype=refAttr.at(typeIndex);
		if(thistype!=TYPE_TYPE)return thistype;
		else return refAttr.at(valIndex);
	}
	inline SymbolAttributes& SimpleCppTranslator::getRefAttributes(
			SymbolAttributes& attr)
	{
		int refTypeIndex=this->attrsMap.get("refType");

		auto itref=attr.find(refTypeIndex);
		if(itref!=attr.end() && itref->second==REF_TABLE)
		{
			int typeIndex=this->attrsMap.get("type");
			int valIndex = this->attrsMap.get("val");
			return this->symTableList[attr[typeIndex]][attr[valIndex]];
		}
		return attr;
	}





}


#endif /* INCLUDE_SIMPLECPPTRANSLATOR_H_ */
