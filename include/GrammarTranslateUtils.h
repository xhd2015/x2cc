/*
 * GrammarTranslateUtils.h
 *
 *  Created on: 2017年4月18日
 *      Author: 13774
 */

#ifndef INCLUDE_GRAMMARTRANSLATEUTILS_H_
#define INCLUDE_GRAMMARTRANSLATEUTILS_H_

#include <FAUtils.h>
#include <GrammaUtils.h>
#include <stack>
#include <LexicalParser.h>
#include <x2def.h>
#include <string>

namespace x2
{


/////////////====DEPRECATED
////////////=====之所以不把语义动作和翻译过程分开实现,是因为翻译动作和语义是紧密相关的
/////////////=====语义会使用到许多翻译所使用的数据
///**
// * 语义动作(也就是语义规则)
// * 使用者应当根据自己的需求来实现onReduce方法
// *
// */
//class SemanticAction{
//public:
//	/**
//	 * 在产生一个归约式的时候调用.
//	 *  \param i
//	 *  \param j
//	 *  <i,j>指定使用的产生式,因为它只是个索引,所以你必须知道它所使用的文法实体
//	 */
//	virtual void onReduce(int i,int j)=0;
//	/**
//	 * 在GOTO[state,A]没有对应表项的时候调用
//	 */
//	virtual void onNoGotoError()=0;
//	/**
//	 * 在Action[state,a]没有对应的转移的时候调用
//	 */
//	virtual void onNoActionError()=0;
//};
///**
// * @brief 根据提议，应当为某些可有可无的适配器提供一个默认实现
// */
//class DefaultSemanticAction:public SemanticAction{
//public:
//		virtual ~DefaultSemanticAction()=default;
//
//};

/*! \brief A Grammar Translation Demonstrate to Clarify Some Concepts
 *
 * Given a grammar:
 * 	A -> T id = E ;
 * 	A -> id = E ;
 * 	E -> ( E )
 * 	E -> id
 * 	E -> intval
 * 	E -> charval
 * 	E -> E + E
 * 	E ->　E * E
 * 	C -> A
 * 	T -> int
 * 	T -> char
 * 	T -> string
 *
 *  An Input:
 *		int a=0;
 *		char c='t';
 *		a=c;
 *		a=d;
 *		int a=1;
 *
 *	The Translator should point out that:
 *		1. a=c is impossible because their types are not compitable
 *		2. a=d is impossible because d is not defined
 *		3. int a=1 is impossible because a is already defined
 *		4. it should generate a proper code for the right parts
 *
 *	自动机构造算法:
 *		首先将CorruptTable转化为标准的分析表AnalyzeTale类型
 *		初始化DFA,EMPTY取相同值, failed取UNDEFINED值(-2),  开始状态为I0, 结束状态为为空
 *		遍历GOTO表添加变量类型的转移
 *
 *	ADVANCE:
 *		翻译器是通用的,因此只定义所有翻译器公用的部分.
 *		包括文法引用,分析表引用
 *		语义动作是一个单独的类,指明了某个归约式产生的时候进行某种动作
 *		翻译器是不可重入的,所以它必须与某个输入流结合起来
 *
 *	Why:
 *		到了翻译这一步,基本上已经没有通用的东西了,必须针对具体的语义进行翻译动作
 *
 */
class DemoTranslator{
public:
	using ACTION_TYPE=LR1Gramma::ACTION_TYPE;
	typedef std::pair<std::string,int>	StreamWordType;

	/**
	 * @param grammar	待翻译输入流中使用的文法
	 * @param closuresInfo	规范集项目族和GOTO表,处理方法参见LR1Gramma类的toString(info)
	 * @param analyzeTable	规范的LR分析表
	 * @param endingSym	指定结束符号 $
	 * @param initState 指定这个分析表的起始状态,默认情况下所有的分析表起始状态为0
	 */
	DemoTranslator(const LR1Gramma & grammar,const LR1Gramma::InfoType & closuresInfo,
			const LR1Gramma::LRAnalyzeTableType & analyzeTable,int endingSym,int initState=0);
	DemoTranslator(const DemoTranslator & t)=default;
	DemoTranslator(DemoTranslator && t)=default;
	virtual ~DemoTranslator()=default;

	/**
	 *  @brief 翻译输入流
	 *  @detail 翻译过程中栈会被设置
	 *
	 */
	virtual void translate(StreamConvertor<StreamWordType>& input);
protected:

	/**
	 * @brief 初始化时调用
	 */
	virtual void onInit();
	/**
	 * @brief called when a reduce is produced
	 */
	virtual void onReduce(int i,int j);

	virtual void onNoGoto();

	virtual void onNoAction();

	virtual void onAccept();

	virtual void onShift();

	virtual void onInputNotTerminator();

	/**
	 * @brief	结束时调用
	 * @detail	建议的动作是清空状态栈
	 */
	virtual void onEnd();

	void terminate();


protected:
	const LR1Gramma & grammar;
	const LR1Gramma::InfoType & closuresInfo;
	const Gramma::LRAnalyzeTableType & analyzeTable;
	int endingSym;
	int initState;
	const StreamConvertor<StreamWordType> * currentInput;
	std::stack<int>			ss;
	bool					terminated;
};

//=========class : DemoTranslator
inline DemoTranslator::DemoTranslator(const LR1Gramma & grammar,const LR1Gramma::InfoType & closuresInfo,
		const LR1Gramma::LRAnalyzeTableType & analyzeTable,int endingSym,int initState):
				grammar(grammar),closuresInfo(closuresInfo),
				analyzeTable(analyzeTable),
				endingSym(endingSym),
				initState(initState),
				currentInput(NULL),
				terminated(false)
{
}
inline void DemoTranslator::translate(
		StreamConvertor<StreamWordType>& input)
{
	//==set GOTO
	const LR1Gramma::GotoInfoType & GOTO=std::get<2>(closuresInfo);
	//==set currentInput
	this->currentInput=&input;

	//== call onInit()
	this->onInit();

	//== push initSym & initState
	ss.push(endingSym);
	ss.push(initState);

	while(!input.eof() && !terminated)
	{
		//== get current state & input
		std::pair<int,int> gokey={ss.top(),input.peek()};
//		std::cout << "key is "<<gokey.first << ","<<grammar.gsyms.getString(gokey.second) <<std::endl;
		int type=grammar.gsyms.getSymbolType(gokey.second);
//		std::cout << "input type is "<<grammar.gsyms.TYPES_INFO.getT1(type)<<std::endl;
		if(type==GrammaSymbols::TYPE_TERM)
		{
			auto actionit=analyzeTable.find(gokey);
			if(actionit!=analyzeTable.end())
			{
				//== get ActionType
				int actiontype=std::get<2>(actionit->second);
				//== do SHIFT
				if(actiontype==ACTION_TYPE::ACTION_SHIFT_IN)
				{
//					std::cout << "\t\tSHIFT " << grammar.gsyms.getString(gokey.second) << " going "<<std::get<0>(actionit->second)<<std::endl;
					ss.push(gokey.second);
					ss.push(std::get<0>(actionit->second));

					//== call onShift
					this->onShift();

					input.goForward();

				//== do REDUCE
				}else if(actiontype==ACTION_TYPE::ACTION_REDUCE){
					int i=std::get<0>(actionit->second),j=std::get<1>(actionit->second);
					int len=2*grammar.getRightSentences(i).at(j).getLength();
//					std::cout << "length is "<<len<<std::endl;
					while(len--)
					{
						ss.pop();
					}
					std::pair<int,int> tempkey={ss.top(),i};
					ss.push(i);
//					std::cout << "after removed,top is <"<<tempkey.first<<","<<grammar.gsyms.getString(tempkey.second)<<">"<<std::endl;
					auto tempit=GOTO.find(tempkey);
					if(tempit!=GOTO.end())
					{
						ss.push(tempit->second);
					}else this->onNoGoto();
//					std::cout << "\t\treducing "<<grammar.Gramma::toString(i, j)<<std::endl;

					//=== call onReduce
					this->onReduce(i,j);
				}else if(actiontype==ACTION_TYPE::ACTION_ACCEPT){

					//=== call onAccept,plus one default action: exit the translation process
					this->onAccept();
					break;
				}
			}else{

				//=== call onNoAction
				this->onNoAction();
			}
		}else{

			//=== call onInputNotTerminator
			this->onInputNotTerminator();
		}
//		else if(type==GrammaSymbols::TYPE_VAR){
//			auto goit=GOTO.find(gokey);
//			if(goit!=GOTO.end())
//			{
//				ss.push(goit->second);
//			}else{
//				this->onNoGoto();
//			}
//		}
	}

	//=== call onEnd
	this->onEnd();
}
inline void DemoTranslator::onInit()
{
	std::cout << "Init " << std::endl;
}
inline void DemoTranslator::onReduce(int i, int j)
{
	std::cout << "Reducing "<<this->grammar.Gramma::toString(i, j) << std::endl;
}

inline void DemoTranslator::onNoGoto()
{
	std::cout << "ERROR : no goto." << std::endl;
}

inline void DemoTranslator::onNoAction()
{
	std::cout << "ERROR : no action." << std::endl;
}
inline void DemoTranslator::onAccept()
{
	std::cout << "ACCEPT" << std::endl;
}

inline void DemoTranslator::onShift()
{
	std::cout << "SHIFT IN"<<std::endl;
}

inline void DemoTranslator::onInputNotTerminator()
{
	std::cout << "input is not terminator"<<std::endl;
}

inline void DemoTranslator::onEnd()
{
	std::cout << "Ended"<<std::endl;
	while(!ss.empty())ss.pop();
}
inline void DemoTranslator::terminate()
{
	this->terminated=true;
}



} /* namespace x2 */

#endif /* INCLUDE_GRAMMARTRANSLATEUTILS_H_ */
