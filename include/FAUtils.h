/*
 * NFAUtils.h
 *
 *  Created on: 2017年4月13日
 *      Author: 13774
 */

#ifndef INCLUDE_FAUTILS_H_
#define INCLUDE_FAUTILS_H_

#include <set>
#include <map>
#include <string>
#include <MutualMap.h>
#include <vector>
#include <StringUtils.h>
#include <functional>
#include <initializer_list>

namespace x2
{
/**
 * symbols 中至少含有一项：empty
 * states和symbols都有未定义
 *
 * int和相应的string是等价的
 *
 * T	输入符号类型
 * 输入符号类型必须转成int型
 */
template<class T>
class FiniteAutomataManager
{ //只是一个管理器，管理状态，管理符号表
public:
	/**
	 * when not found,return what?
	 */
	FiniteAutomataManager(const  T& emptyT,const T& failedT);
	FiniteAutomataManager(const  T& emptyT,const T& failedT,std::initializer_list<T> symList,
			std::initializer_list<std::string> stateList
	);
	FiniteAutomataManager(FiniteAutomataManager<T>&& fa)=default;
	void addSymbol(const T& t,int num);
	void addStates(int state,const std::string & strstate);
	int queryState(const std::string & state)const;
	int querySymbol(const T& t)const;
	const std::string& queryState(int state)const;
	const T& 		querySymbol(int sym)const;
	std::string toString()const;


protected:
	x2::MutualMap<int, T> symbols;
	x2::MutualMap<int, std::string> states;
	static int EMPTY_INDEX, UNDEFINED_INDEX;
	static std::string EMPTY_STRING, UNDEFINED_STRING;
};

//针对int类型的特化
/**
 * 暂时什么都不提供
 */
template <>
class FiniteAutomataManager<int>
{

};

/**
 * helper for FAs
 *
 * T input stream type
 *
 */
template <class T>
 class OutputStreamProcessor
 {
 public:
	OutputStreamProcessor()=default;
	virtual ~OutputStreamProcessor()=default;
	virtual void process(int curState,const T& in)=0;
 };
/**
 * 一般的词法分析器要求输出 <值，类型>两种
 * 有些值，比如int，类型是ID，ID-int就可以作为一种语法引导符号
 */
template <class T,class V>
class LexicalOutputStreamProcessor:public OutputStreamProcessor<T>{
public:
	typedef std::vector<V> OutputStreamType;
public:
	LexicalOutputStreamProcessor();
	virtual ~LexicalOutputStreamProcessor();
	OutputStreamType& getCachedStream();
	const OutputStreamType& getCachedStream()const;
	virtual void process(int curState,const T& in)=0;

protected:
	 OutputStreamType	cachedStream;
};


/**
 * 输入流既可以是 vector，也可以是string；
 */
template<class T>
class FiniteAutomata
{
public:
	typedef std::vector<T> InputStreamType;
	typedef FiniteAutomata<T>	This;
	typedef std::function<void(int,const T&)> ProcessFunType;
public:
	FiniteAutomata(const  T& emptyT,const T& failedT,int startState,const std::vector<int>& endingStates);
	FiniteAutomata(FiniteAutomataManager<T> && faman,int startState,const std::vector<int>& endingStates);
	FiniteAutomata(FiniteAutomataManager<T> && faman,const std::string& startState,const std::vector<std::string>& endingStates);
	virtual ~FiniteAutomata()=default;

	const T& getCurrentState()const;
	int queryState(const std::string & state)const;
	int querySymbol(const T& t)const;
	const std::string& queryState(int state)const;
	const T& 		querySymbol(int sym)const;
	/**
	 * rest all state-related information, restart at startState
	 */
	void reset();
	virtual bool addTransition(int qin, int in, int qout)=0; //imply  if succeed
	/**
	 *
	 */
	virtual void getMatch(const InputStreamType& instream,OutputStreamProcessor<T>& outstream)=0;
	virtual void getMatch(const InputStreamType& instream,ProcessFunType process)=0;
	std::string toString()const;
protected:
	FiniteAutomataManager<T> FAman;
	int startState;
	std::set<int> endingStates;
	int curState;
};


/**
 * FA that can generate output stream.
 */
template<class T>
class DeterminasticFA: public FiniteAutomata<T>
{
public:
	typedef DeterminasticFA<T> This;
	typedef FiniteAutomata<T>  Father;
	typedef std::vector<T> InputStreamType;
	typedef std::function<void(int,const T&)> ProcessFunType;
public:
	DeterminasticFA(FiniteAutomataManager<T> && faman,int startState,const std::vector<int>& endingStates);
	DeterminasticFA(FiniteAutomataManager<T> && faman,const std::string& startState, const std::vector<std::string>& endingStates);
	/**
	 * add,if existed,return failed
	 */
	virtual ~DeterminasticFA()=default;
	/**
	 * if non-exist,add it,else return failed.
	 */
	bool addTransition(int qin, int in, int qout);
	bool addTransition(const std::string & qin, const T & in, const std::string & qout);
	void addReplace(int qin, int in, int qout);
	void addReplace(const std::string & qin, const T & in, const std::string & qout);
	/**
	 * goto next state on input in,if failed return failed,else return succeed.
	 */
	bool  next(const T& in);
	bool next(int in);
	/**
	 * if current state is accepted
	 */
	bool atEnd()const;
	virtual void getMatch(const InputStreamType& instream,OutputStreamProcessor<T>& outstream);
	virtual void getMatch(const InputStreamType& instream,ProcessFunType process);
	std::string toString()const;

protected:
	std::map<std::pair<int, int>, int> transitions;

};
template<class T>
class NondeterminasticFA: public FiniteAutomata<T>
{

protected:
	std::map<std::pair<int, T>, std::set<int>> transitions;

};

//==========template functions
//===static members
template<class T>
	int FiniteAutomataManager<T>::EMPTY_INDEX = -1;
template<class T>
	int FiniteAutomataManager<T>::UNDEFINED_INDEX = -2;
template<class T>
	std::string FiniteAutomataManager<T>::EMPTY_STRING = "EMPTY";
template<class T>
	std::string FiniteAutomataManager<T>::UNDEFINED_STRING = "UNDEFINED";

//====class FiniteAutomataManager<T>
template<class T>
FiniteAutomataManager<T>::FiniteAutomataManager(const  T& emptyT,const T& failedT) :
		symbols(UNDEFINED_INDEX, failedT),states(UNDEFINED_INDEX,UNDEFINED_STRING)
{
	symbols.add(EMPTY_INDEX, emptyT);
}
template<class T>
inline FiniteAutomataManager<T>::FiniteAutomataManager(const T& emptyT,
		const T& failedT, std::initializer_list<T> symList,
		std::initializer_list<std::string> stateList):
		FiniteAutomataManager(emptyT,failedT)
{
	int i=0;
	for(auto ch:symList)
	{
		addSymbol(ch, i++);
	}
	i=0;;
	for(auto &str:stateList)
	{
		addStates(i++, str);
	}
}

template<class T>
inline void x2::FiniteAutomataManager<T>::addSymbol(const T& t, int num)
{
	this->symbols.add(num, t);
}

template<class T>
inline void x2::FiniteAutomataManager<T>::addStates(int state,const std::string& strstate)
{
	this->states.add(state, strstate);
}
template<class T>
inline int x2::FiniteAutomataManager<T>::queryState(
		const std::string& state) const
{
	return this->states.get(state);
}

template<class T>
inline int x2::FiniteAutomataManager<T>::querySymbol(const T& t) const
{
	return this->symbols.get(t);
}

template<class T>
inline const std::string& x2::FiniteAutomataManager<T>::queryState(
		int state) const
{
	return this->states.get(state);
}
template<class T>
inline const T& x2::FiniteAutomataManager<T>::querySymbol(int sym) const
{
	return this->symbols.get(sym);
}

template<class T>
inline std::string x2::FiniteAutomataManager<T>::toString()const
{
	std::string s("Symbol MutualMap:{\n");
	s+=this->symbols.toString()+"}\n";
	s+="States MutualMap:{\n"+this->states.toString() + "}\n";
	return s;
}

//=====class LexicalOutputStreamProcessor
template<class T, class V>
inline LexicalOutputStreamProcessor<T, V>::LexicalOutputStreamProcessor()
{
}

template<class T, class V>
inline LexicalOutputStreamProcessor<T, V>::~LexicalOutputStreamProcessor()
{
}

template<class T, class V>
inline typename LexicalOutputStreamProcessor<T,V>::OutputStreamType& LexicalOutputStreamProcessor<T, V>::getCachedStream()
{
	return cachedStream;
}
template<class T, class V>
inline const typename LexicalOutputStreamProcessor<T,V>::OutputStreamType& LexicalOutputStreamProcessor<T, V>::getCachedStream() const
{
	return cachedStream;
}

//====class : FiniteAutomata
template<class T>
inline FiniteAutomata<T>::FiniteAutomata(const T& emptyT, const T& failedT,
		int startState, const std::vector<int>& endingStates):
		FAman(emptyT,failedT),startState(startState),curState(startState)
{
	for(auto i:endingStates)
	{
		this->endingStates.insert(i);
	}
}
template<class T>
inline FiniteAutomata<T>::FiniteAutomata(FiniteAutomataManager<T> && faman,
		const std::string& startState,
		const std::vector<std::string>& endingStates):
		FAman(std::move(faman)),startState(FAman.queryState(startState)),curState(this->startState)
{
	for(auto &str:endingStates)
	{
		this->endingStates.insert(FAman.queryState(str));
	}
}

template<class T>
inline FiniteAutomata<T>::FiniteAutomata(FiniteAutomataManager<T> && faman,
		int startState, const std::vector<int>& endingStates):
		FAman(std::move(faman)),startState(startState),curState(startState)
{
	for(auto i:endingStates)
	{
		this->endingStates.insert(i);
	}
}


template<class T>
inline const T& FiniteAutomata<T>::getCurrentState() const
{
	return this->queryState(this->curState);
}


template<class T>
inline int x2::FiniteAutomata<T>::queryState(const std::string& state)const
{
	return this->FAman.queryState(state);
}

template<class T>
inline int x2::FiniteAutomata<T>::querySymbol(const T& t)const
{
	return this->FAman.querySymbol(t);
}
template<class T>
inline const std::string& FiniteAutomata<T>::queryState(int state)const
{
	return this->FAman.queryState(state);
}

template<class T>
inline const T& FiniteAutomata<T>::querySymbol(int sym)const
{
	return this->FAman.querySymbol(sym);
}
template<class T>
inline void FiniteAutomata<T>::reset()
{
	this->curState = this->startState;
}
template<class T>
inline std::string x2::FiniteAutomata<T>::toString() const
{
	std::string s;
	s+=std::string("Current State : [") + std::to_string(curState) + "] " + this->queryState(curState) + "\n";
	s+=std::string("Starting State : [") + std::to_string(startState) + "] " + this->queryState(startState) + "\n";
	s+=std::string("Ending States : [");
	std::string temp;
	for(auto &i:this->endingStates)
	{
		s+=std::to_string(i) + ", ";
		temp += this->queryState(i) + ", ";
	}
	s+="] " + std::move(temp) + "\n";
	s+=std::string("Symbols & States Manager : {\n")+ this->FAman.toString() + "}";
	return s;
}


//====class : DeterminasticFA<T>
template<class T>
inline DeterminasticFA<T>::DeterminasticFA(FiniteAutomataManager<T> && faman,
		int startState, const std::vector<int>& endingStates):
		Father(std::move(faman),startState,endingStates)
{
}
template<class T>
inline DeterminasticFA<T>::DeterminasticFA(FiniteAutomataManager<T> && faman,
		const std::string& startState, const std::vector<std::string>& endingStates):
		Father(std::move(faman),startState,endingStates)
{
}


template<class T>
bool DeterminasticFA<T>::addTransition(int qin, int in, int qout)
{
	std::pair<int, int> key(qin, in);
	auto it = transitions.find(key);
	if (it != transitions.end())
		return false;
	transitions[key] = qout;
	return true;
}
template<class T>
inline bool DeterminasticFA<T>::addTransition(const std::string& qin,
		const T& in, const std::string& qout)
{
	return addTransition(this->queryState(qin),this->querySymbol(in),this->queryState(qout));
}

template<class T>
void DeterminasticFA<T>::addReplace(int qin, int in, int qout)
{
	std::pair<int, int> key(qin, in);
	auto it = transitions.find(key);
	if (it != transitions.end())
		it->second = qout;
	else
		transitions[key] = qout;
}
template<class T>
inline void DeterminasticFA<T>::addReplace(const std::string& qin,
		const T& in, const std::string& qout)
{
	addReplace(this->queryState(qin),this->querySymbol(in),this->queryState(qout));
}

template<class T>
inline bool DeterminasticFA<T>::next(const T& in)
{
	return this->next(this->querySymbol(in));
}

template<class T>
inline bool DeterminasticFA<T>::next(int in)
{
	std::pair<int,int> key(this->curState,in);
	auto it = transitions.find(key);
	if(it == transitions.end())return false;
	this->curState=it->second;
	return true;
}

template<class T>
inline bool DeterminasticFA<T>::atEnd() const
{
	return this->endingStates.find(this->curState)!=this->endingStates.end();
}


template <class T>
inline void DeterminasticFA<T>::getMatch(const InputStreamType& instream,OutputStreamProcessor<T>& outstream)
{
	for(const T& t:instream)
	{
		outstream.process(this->curState, t);
		if(!this->next(t))return;
	}
}

template <class T>
inline void DeterminasticFA<T>::getMatch(const InputStreamType& instream,ProcessFunType process)
{
	for(const T& t:instream)
	{
		process(this->curState, t);
		if(!this->next(t))return;
	}
}
template <class T>
inline std::string DeterminasticFA<T>::toString()const
{
	std::string s=this->Father::toString();
	s+="\nTransitions : {\n";
	for(auto &p:this->transitions)
	{
		s+=std::string("\t[") + this->queryState(p.first.first) + "," + x2::toString(this->querySymbol(p.first.second))+"] = "+this->queryState(p.second)+"\n";
	}
	s+="}";
	return s;
}

}



#endif /* INCLUDE_FAUTILS_H_ */
