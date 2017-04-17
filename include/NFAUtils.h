/*
 * NFAUtils.h
 *
 *  Created on: 2017年4月13日
 *      Author: 13774
 */

#ifndef INCLUDE_NFAUTILS_H_
#define INCLUDE_NFAUTILS_H_

#include <set>
#include <map>
#include <string>

namespace {

//struct NFAState:std::pair<int,std::set<int>>{
//
//};

class NFAStates{
public:
	enum{
		IN_EMPTY/*空输入*/,
		IN_ANY_CONTENT/*任何输入,e.g. .  */,
		IN_ANY_NUMBER/* any number of an input, e.g. * */,
		IN_ONE_OR_ZERO/* ? */,
		IN_ONE_MORE/* + */,
	};
	int			addState(const std::string& name);
	std::string getString(int i)const;
};

template <class T> /*the elements want to apply '=' on*/
class NFA{
public:
	void addTransition(const T &in,int at,int out);
	void addTransition(const T &in,const std::string& at,const std::string& out);//from name to name
	bool isStateEnd(int state)const;
	const std::set<int> & getNexts(int i,const T& in);


	NFAStates statesMan; //state manager
	std::map<int,std::set<int>> transitions; // from NFAStates;
	std::map<int,T>   names;//a space for inputs
	int curState;
};


}



#endif /* INCLUDE_NFAUTILS_H_ */
