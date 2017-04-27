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

namespace x2
{

/**
 * 语法制导的翻译 SSD
 *    产生式需要与另一个规则相互连接起来，构成SSD的语义规则
 *
 *
 */
class SSDTranslator
{
public:
	SSDTranslator();
	~SSDTranslator();
};
/**
 * 语义动作(也就是语义规则)
 * 使用者应当根据自己的需求来实现onReduce方法
 *
 */
class SemanticAction{
public:
	/**
	 * 在产生一个归约式的时候调用.
	 *  \param i
	 *  \param j
	 *  <i,j>指定使用的产生式,因为它只是个索引,所以你必须知道它所使用的文法实体
	 */
	virtual void onReduce(int i,int j)=0;
	/**
	 * 在GOTO[state,A]没有对应表项的时候调用
	 */
	virtual void onNoGotoError()=0;
	/**
	 * 在Action[state,a]没有对应的转移的时候调用
	 */
	virtual void onNoActionError()=0;
};
/**
 * @brief 根据提议，应当为某些可有可无的适配器提供一个默认实现
 */
class DefaultSemanticAction:public SemanticAction{
public:
		virtual ~DefaultSemanticAction()=default;

		virtual void onReduce(int i,int j)
		{
			std::cout << "Reducing <"<<i<<","<<j<<">"<<std::endl;
		}
		virtual void onNoGotoError()
		{
			std::cout << "ERROR : no goto." << std::endl;
		}
		virtual void onNoActionError()
		{
			std::cout << "ERROR : no action." << std::endl;
		}
};

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
 *
 */
class DemoTranslator{
public:
	using ACTION_TYPE=LR1Gramma::ACTION_TYPE;
	typedef std::tuple<int,int,int,int>	QuadType;
	typedef std::vector<QuadType>	TranslationType;

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

	/**
	 *  在
	 */
	virtual TranslationType translate(StreamConvertor& input,SemanticAction &action)const;


protected:
	const LR1Gramma & grammar;
	const LR1Gramma::InfoType & closuresInfo;
	const Gramma::LRAnalyzeTableType & analyzeTable;
	int endingSym;
	int initState;


};

//=========class : DemoTranslator
inline DemoTranslator::DemoTranslator(const LR1Gramma & grammar,const LR1Gramma::InfoType & closuresInfo,
		const LR1Gramma::LRAnalyzeTableType & analyzeTable,int endingSym,int initState):
				grammar(grammar),closuresInfo(closuresInfo),analyzeTable(analyzeTable),endingSym(endingSym),initState(initState)
{
}
/**
 *
 */
inline typename DemoTranslator::TranslationType DemoTranslator::translate(
		StreamConvertor& input,SemanticAction &action) const
{
	const LR1Gramma::GotoInfoType & GOTO=std::get<2>(closuresInfo);
	std::stack<int>	ss;
	ss.push(endingSym);
	ss.push(initState);
	while(!input.eof())
	{
		std::pair<int,int> gokey={ss.top(),input.peek()};
//		std::cout << "key is "<<gokey.first << ","<<grammar.gsyms.getString(gokey.second) <<std::endl;
		int type=grammar.gsyms.getSymbolType(gokey.second);
//		std::cout << "input type is "<<grammar.gsyms.TYPES_INFO.getT1(type)<<std::endl;
		if(type==GrammaSymbols::TYPE_TERM)
		{
			auto actionit=analyzeTable.find(gokey);
			if(actionit!=analyzeTable.end())
			{
				int actiontype=std::get<2>(actionit->second);
				if(actiontype==ACTION_TYPE::ACTION_SHIFT_IN)
				{
//					std::cout << "\t\tSHIFT " << grammar.gsyms.getString(gokey.second) << " going "<<std::get<0>(actionit->second)<<std::endl;
					ss.push(gokey.second);
					ss.push(std::get<0>(actionit->second));
					input.goForward();
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
					}else action.onNoGotoError();
					std::cout << "\t\treducing "<<grammar.Gramma::toString(i, j)<<std::endl;
					action.onReduce(i,j);
				}else if(actiontype==ACTION_TYPE::ACTION_ACCEPT){
					std::cout << "ACCEPT" << std::endl;
					break;
				}
			}else{
				action.onNoActionError();
				while(getchar()!='\n');
			}
		}else if(type==GrammaSymbols::TYPE_VAR){
			auto goit=GOTO.find(gokey);
			if(goit!=GOTO.end())
			{
//				std::cout << "GOTO " << goit->second <<std::endl;
				ss.push(goit->second);
			}else{
				action.onNoGotoError();
			}
		}
	}
	return TranslationType();
}



} /* namespace x2 */

#endif /* INCLUDE_GRAMMARTRANSLATEUTILS_H_ */
