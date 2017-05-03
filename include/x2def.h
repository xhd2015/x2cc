/*
 * x2def.h
 *
 *  Created on: 2017年4月28日
 *      Author: 13774
 */

#ifndef INCLUDE_X2DEF_H_
#define INCLUDE_X2DEF_H_
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace x2{
	template <class K,class V>
	using HashMap=std::unordered_map<K,V>;

	template <class K,class V>
	using HashMultiMap=std::unordered_multimap<K,V>;

	template <class K>
	using HashSet=std::unordered_set<K>;

	template <class K>
	using HashMultiSet=std::unordered_multiset<K>;

}



#endif /* INCLUDE_X2DEF_H_ */
