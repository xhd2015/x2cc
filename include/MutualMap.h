/*
 * MutualMap.h
 *
 *  Created on: 2017年4月19日
 *      Author: 13774
 */

#ifndef INCLUDE_MACROS_MUTUALMAP_H_
#define INCLUDE_MACROS_MUTUALMAP_H_

#include <map>
#include <utility>
#include <StringUtils.h>
#include <initializer_list>

namespace x2
{
//如果T1=T2时,则无法通过方法名区分调用的方法
/**
 *
 * 任意时刻保证：
 * 		如果a,b 在m1中，则b,a必在m2中；如果a,b在m2中，则b,a必在m1中
 * 		如果a,b不在m1中，则b,a不在m2中；如果a,b不在m2中，则b,a不在m1中
 */
template <class T1,class T2>
class MutualMap
{
public:
	typedef MutualMap<T1,T2> This;
	typedef T1	Type1;
	typedef T2  Type2;
	typedef std::map<Type1,Type2> MapType1;
	typedef std::map<Type2,Type1> MapType2;
public:
	MutualMap()=default;
	MutualMap(const T1& failedT1,const T2& failedT2);
	MutualMap(const T1& failedT1,const T2& failedT2,std::initializer_list<std::pair<T1,T2>> list);
	MutualMap(const T1& failedT1,const T2& failedT2,std::initializer_list<std::pair<T2,T1>> list);
	MutualMap(MutualMap<T1,T2> && mm)=default;
	MutualMap(const MutualMap<T1,T2> & mm)=default;
	~MutualMap()=default;
	/**
	 * 两者都作为键，不可能直接更改其中任何一方的值
	 * 如果没有查找到，还应当 有一个默认的返回值，即提供失败的值
	 */
	const T1&	get(const T2& t2)const;
	const T2&   get(const T1& t1)const;
	/**
	 * get,if non-exist, add it
	 */
	const T1&	getAdd(const T2& t2,const T1& t1);
	const T2&	getAdd(const T1& t1,const T2& t2);
	/**
	 * if any one is present,return failed,else return succeed
	 */
	bool add(const T1& t1,const T2& t2);
	bool add(T1 && t1,T2 && t2);
	bool add(const T2& t2,const T1& t1);
	bool add(T2 && t2,T1 && t1);


	T2 remove(const T1& t1);
	T1 remove(const T2& t2);

	/**
	 * add, if t1 pair already exist,then t1 is kept,change <t1,_> to <t1,t2>
	 * return if replaced
	 */
	bool addReplace(const T1 & t1,const T2& t2);
	bool addReplace(const T2 & t2,const T1 & t1);

	std::string toString()const;


protected:
	MapType1 m1;
	MapType2 m2;
	Type1 failedT1;
	Type2 failedT2;
};
/**
 * visit all through T,not by index
 */
template <class T>
class IndexedMap : protected MutualMap<int,T>{
public:
	typedef MutualMap<int,T> Father;
	typedef IndexedMap		This;
	using Father::toString;

	IndexedMap();
	IndexedMap(const IndexedMap<T>& map)=default;
	IndexedMap(IndexedMap<T>&& map)=default;
	IndexedMap(const T& failedT);
	IndexedMap(const T& failedT,std::initializer_list<T> list);
	IndexedMap(const T& failedT,const std::vector<T>& list);
	int get(const T& t)const;
	const T& get(int i)const;
	int getAdd(const T& t);
	/**
	 * always succeed
	 */
	void add(const T& t);
	void remove(const T& t);

protected:
	int max;
	static int UNDEFINED_INDEX;

};
/**
 *  the special type of int is not defined.
 */
template <> class IndexedMap<int>{

};

//===static members
template <class T>
	int	IndexedMap<T>::UNDEFINED_INDEX=-2;



//========templates
template<class T1, class T2>
inline MutualMap<T1, T2>::MutualMap(const T1& failedT1,const T2& failedT2):
	failedT1(failedT1),failedT2(failedT2)
{

}

template<class T1, class T2>
inline MutualMap<T1, T2>::MutualMap(const T1& failedT1,const T2& failedT2,
		std::initializer_list<std::pair<T1, T2> > list):
		MutualMap(failedT1,failedT2)
{
	for(auto &p:list)
	{
		this->add(p.first,p.second);
	}
}
template<class T1, class T2>
inline MutualMap<T1, T2>::MutualMap(const T1& failedT1,const T2& failedT2,
		std::initializer_list<std::pair<T2, T1> > list):
		MutualMap(failedT1,failedT1)
{
	for(auto &p:list)
	{
		this->add(p.first,p.second);
	}
}


template<class T1, class T2>
inline const T1& MutualMap<T1, T2>::get(const T2& t2)const
{
	const auto &it=m2.find(t2);
	if(it==m2.end())return failedT1;
	return it->second;
}

template<class T1, class T2>
inline const T2& MutualMap<T1, T2>::get(const T1& t1)const
{
	const auto &it=m1.find(t1);
	if(it==m1.end())return failedT2;
	return it->second;
}

template<class T1, class T2>
inline const T1& MutualMap<T1, T2>::getAdd(const T2& t2,const T1& t1)
{
	const auto &it=m2.find(t2);
	if(it==m2.end()){this->add(t2, t1);return t1;}
	return it->second;
}

template<class T1, class T2>
inline const T2& MutualMap<T1, T2>::getAdd(const T1& t1,const T2& t2)
{
	const auto &it=m1.find(t1);
	if(it==m1.end()){this->add(t1,t2);return t2;}
	return it->second;
}

template<class T1, class T2>
inline bool MutualMap<T1, T2>::add(const T1& t1, const T2& t2)
{
	auto it1=m1.find(t1);
	if(it1!=m1.end())return false;
	m1[t1]=t2;
	m2[t2]=t1;
	return true;
}

template<class T1, class T2>
inline bool MutualMap<T1, T2>::add(T1&& t1, T2&& t2)
{
	const T1& ct1=t1;
	const T2& ct2=t2;
	const auto &it1=m1.find(ct1);
	if(it1!=m1.end())return false;
	m1[ct1]=ct2;//copy it
	m2[std::move(t2)]=std::move(t1);//move it
	return true;
}
template<class T1, class T2>
inline bool MutualMap<T1, T2>::add(const T2& t2, const T1& t1)
{
	return add(t1,t2);
}

template<class T1, class T2>
inline bool MutualMap<T1, T2>::add(T2&& t2, T1&& t1)
{
	return add(t1,t2);
}


template<class T1, class T2>
inline T2 MutualMap<T1, T2>::remove(const T1& t1)
{
	auto it1=m1.find(t1);
	if(it1==m1.end())return failedT2;
	auto it2=m2.find(it1->second);
	m1.erase(it1);
	m2.erase(it2);

	return std::move(it1->second);
}

template<class T1, class T2>
inline T1 MutualMap<T1, T2>::remove(const T2& t2)
{
	auto it2=m2.find(t2);
	if(it2==m2.end())return failedT1;
	auto it1=m1.find(it2->second);
	m1.erase(it1);
	m2.erase(it2);

	return std::move(it2->second);
}

template<class T1, class T2>
inline bool MutualMap<T1, T2>::addReplace(const T1& t1, const T2& t2)
{
	typename std::map<T1,T2>::iterator it1=m1.find(t1);
	if(it1==m1.end()){add(t1,t2);return false;}
	typename std::map<T2,T1>::iterator it2=m2.find(it1->second);
	m2.erase(it2);
	it1->second = t2;
	m2[it1->second] = it1->first;
	return true;
}


template<class T1, class T2>
inline bool MutualMap<T1, T2>::addReplace(const T2& t2, const T1& t1)
{
	auto &it2=m2.find(t2);
	if(it2==m2.end()){add(t2,t1);return false;}
	auto &it1=m1.find(it2->second);
	m1.erase(it1);
	it2->second = t1;
	m1[it2->second] = it2->first;
	return true;
}
template<class T1, class T2>
inline std::string MutualMap<T1, T2>::toString()const
{
	return std::string("\tmap1:")+x2::toString(m1) + "\n\tmap2:"+x2::toString(m2)+"\n";
}

//=======class IndexedMap<T>
template<class T>
inline IndexedMap<T>::IndexedMap():max(0)
{
}

template<class T>
inline IndexedMap<T>::IndexedMap(const T& failedT):
Father(UNDEFINED_INDEX,failedT),max(0)
{
}
template<class T>
inline IndexedMap<T>::IndexedMap(const T& failedT,
		std::initializer_list<T> list):
		Father(UNDEFINED_INDEX,failedT),max(0)
{
	for(auto & t:list)
		this->add(t);
}

template<class T>
inline IndexedMap<T>::IndexedMap(const T& failedT,
		const std::vector<T>& list):
		Father(UNDEFINED_INDEX,failedT),max(0)
{
	for(auto & t:list)
		this->add(t);
}

template<class T>
inline int IndexedMap<T>::get(const T& t)const
{
	return this->Father::get(t);
}
template<class T>
inline const T& IndexedMap<T>::get(int i) const
{
	return this->Father::get(i);
}

template<class T>
inline int IndexedMap<T>::getAdd(const T& t)
{
	int i=this->Father::getAdd(t, max);
	if(i==max)max++;
	return i;
}

template<class T>
inline void IndexedMap<T>::add(const T& t)
{
	auto it2=this->m2.find(t);
	if(it2!=this->m2.end())return;
	this->Father::add(this->max++, t);
}

template<class T>
inline void IndexedMap<T>::remove(const T& t)
{
	auto it2=this->m2.find(t);
	if(it2==this->m2.end())return;
	if(it2->second==this->max-1)this->max--;
	auto it1=this->m1.find(it2->second);
	this->m1.erase(it1);
	this->m2.erase(it2);
}


} /* namespace x2 */


#endif /* INCLUDE_MACROS_MUTUALMAP_H_ */
