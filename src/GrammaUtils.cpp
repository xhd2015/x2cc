/*
 * GrammaUtils.cpp
 *
 *  Created on: 2017年3月31日
 *      Author: 13774
 */

#include <GrammaUtils.h>
#include <utility> //for std::move
#include <new>
#include <iterator> //for inserter
#include <set>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <fstream>

namespace x2
{
	//========static members
  std::string	GrammaSymbols::UNDEFINED_STRING="UNDEFINED";
  std::string 	GrammaSymbols::EMPTY_STRING="EMPTY";
  int			 GrammaSymbols::UNDEFINED_INDEX=-2;

  x2::MutualMap<int,std::string> GrammaSymbols::TYPES_INFO(
		TYPE_UNDEFINED,"UNDEFINED",
	{
      {TYPE_EMPTY,"EMPTY"},
      {TYPE_TERM,"TERMINATOR"},
      {TYPE_VAR,"VARIABLE"},
  });
  std::unordered_map<int,std::string> LR1Gramma::ACTION_INFO_STRING={
		  {ACTION_ACCEPT,"ACCEPT"},
		  {ACTION_SHIFT_IN,"SHIFT"},
		  {ACTION_ERROR,"ERROR"},
		  {ACTION_REDUCE,"REDUCE"},
  };


  //=======functions



  //===========class GrammaSentence
  GrammaSentence::GrammaSentence(const SymsListType & slist):syms(slist)
  {

  }
  GrammaSentence::GrammaSentence(SymsListType && slist):
      syms(slist)
  {

  }
  GrammaSentence::GrammaSentence(std::initializer_list<int>&& list):
    syms(list)
  {

  }
  GrammaSentence::GrammaSentence(const std::initializer_list<int>& list):
      syms(list)
  {

  }
  GrammaSentence& GrammaSentence::operator=(const GrammaSentence& gs)
  {
    new (this) GrammaSentence(gs);
    return *this;
  }
  void GrammaSentence::replaceFirst(const GrammaSentence& gs,int empty)
  {
    this->syms.erase(this->syms.begin());
    if(gs.startsWith(empty) && this->syms.size()>0)return; //必须满足至少有一项的规则 //空产生式就不替换了
    auto in=std::inserter(this->syms,this->syms.begin());
    std::copy(std::begin(gs.syms),std::end(gs.syms),in);
  }
  std::vector<GrammaSentence> GrammaSentence::replaceFirst(const std::vector<GrammaSentence>& vgs,int empty)
    {
      std::vector<GrammaSentence> v;
      for(auto it:vgs)
	{
	  GrammaSentence replaced=this->duplicate();
	  replaced.replaceFirst(it,empty);
	  v.push_back(std::move(replaced));
	}
      return v;
    }
  GrammaSentence GrammaSentence::duplicate()const
  {
    GrammaSentence gs;
    std::copy(this->syms.begin(),this->syms.end(),std::back_inserter(gs.syms));
    return gs;
  }

  //===========class Gramma
  Gramma::Gramma(const GrammaSymbols & gsyms):gsyms(gsyms)
  {

  }
  Gramma::Gramma(GrammaSymbols&& gsyms):gsyms(gsyms)
  {

  }
  Gramma::Gramma(const std::initializer_list<std::pair<int,std::string> > &list):
      gsyms(list)
  {

  }
  Gramma::Gramma(const std::string &file)
  {
	 std::ifstream in(file);
	  new (this) Gramma(in);
	  in.close();
  }
  Gramma::Gramma(std::istream & in):
		  Gramma()
  {
	  std::string s;
	  std::set<int>	typeSure;
	  typeSure.insert(this->gsyms.getEmpty());
	  typeSure.insert(GrammaSymbols::UNDEFINED_INDEX);
	  while(!in.eof())
	  {
		  std::getline(in, s);
		  std::stringstream ss(s);
		  std::string var;
		  ss>>var;
		  int start=gsyms.getAdd(var,GrammaSymbols::TYPE_VAR);
		  typeSure.insert(start);
		  GrammaSentence gs;

		  ss>>var; //discard
		  if(ss.eof())continue;
		  bool	inNote=false;
		  while(!ss.eof())
		  {
			  ss>>var;
			 // std::cout << "var is \""<<var<<"\""<<std::endl;
			  if(var[0]=='#')
				  inNote=!inNote;
			  if(!inNote && var[0]!='#')
				  gs.push_back(gsyms.getAdd(var,GrammaSymbols::TYPE_TERM));
		  }
		  if(gs.syms.size()==0)//such as A-> or A->EMPTY
			  gs.syms.push_back(gsyms.getEmpty());
		  addProduction(start,gs);
	  }
	  for(auto &p:gsyms.symInfo) //correct type
		  if(typeSure.find(p.first)==typeSure.end())
			  p.second=GrammaSymbols::TYPE_TERM;

  }
  Gramma::Gramma(const std::initializer_list<std::pair<int,std::string> > &list,
	   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist):
	       gsyms(list)
  {
    auto it=prodlist.begin(),itend=prodlist.end();
    for(;it!=itend;it++)
      {
    	addProduction(it->first,it->second);
      }
  }
  Gramma::Gramma(const std::initializer_list<std::pair<std::string,std::initializer_list<std::string>>> &prodlist):
		  gsyms()
  {
	  for(auto &p:prodlist)
	  {
		  this->addProduction(p.first,p.second);
	  }
  }
  Gramma::Gramma(const std::vector<std::pair<std::string,std::vector<std::string>>> &prodlist)
  {
	  for(auto &p:prodlist)
	  {
		  this->addProduction(p.first,p.second);
	  }
  }
  void Gramma::addProduction(int i,const GrammaSentence &gs)
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
  void Gramma::addProduction(int i,GrammaSentence &&gs)
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

  void Gramma::addProduction(const std::string& head, const std::vector<std::string>& gs)
  {
	  //headi = this->gsyms.getAdd(head,VAR)
	  int ihead=this->gsyms.getAdd(head,GrammaSymbols::TYPE_VAR);

	  //for s in gs, si = this->gsyms.getAdd(s,TERM)
	  GrammaSentence grammars;
	  for(auto & s:gs)
	  {
		  grammars.push_back(this->gsyms.getAdd(s, GrammaSymbols::TYPE_TERM));
	  }
	  this->addProduction(ihead, std::move(grammars));

  }

  void Gramma::addProduction(const std::string& head,const std::initializer_list<std::string>& gs)
  {
	  //headi = this->gsyms.getAdd(head,VAR)
	  int ihead=this->gsyms.getAdd(head,GrammaSymbols::TYPE_VAR);

	  //for s in gs, si = this->gsyms.getAdd(s,TERM)
	  GrammaSentence grammars;
	  for(auto & s:gs)
	  {
		  grammars.push_back(this->gsyms.getAdd(s, GrammaSymbols::TYPE_TERM));
	  }
	  this->addProduction(ihead, std::move(grammars));
  }


  bool Gramma::canSymbolEmpty(int i)
  {
//    std::cout << "symbol is " << toString(i) <<std::endl;
    if(gsyms.isSymbolEmpty(i)||gsyms.isSymbolUndefined(i))return true;
    else if(gsyms.isSymbolTerm(i))return false;
    else
      {
	std::vector<GrammaSentence> &list=getRightSentences(i);
	for(auto it=list.begin(),itend=list.end();it!=itend;it++)  if(!it->startsWith(i) && canSentenceEmpty(*it))return true;//有一个可空，则为空
	return false;
      }
  }
  bool Gramma::canSentenceEmpty(const GrammaSentence& s)
  {
//      std::cout << "sentence is "<<this->toString(s)<<std::endl;
      return canSentenceEmpty(s,s.syms.size());
  }
  bool Gramma::canSentenceEmpty(const GrammaSentence& s,int end)
  {
    return canSentenceEmpty(s,0,end);
  }
  bool Gramma::canSentenceEmpty(const GrammaSentence& s,int start,int end)
  {
    if(end<=start)return true;
    auto it=s.syms.begin() + start,itend=s.syms.begin() + end;
    while(it < itend)
	{
	  if(!canSymbolEmpty(*it))return false;
	  it++;
	}
    return true;
  }

  void Gramma::replaceFirstProduction(int i,const int j)
  {
    auto &si=this->prods[i],&sj=getRightSentences(j);
    //先复制，然后进行删除，再添加
    auto it=si.begin();
//    printf("%d,%d \n",i,j);
    while(it!=si.end())
      {

	if(it->startsWith(j))
	  {
//	    printf("%d starts with %d\n",i,j);
	    auto v=std::move(it->replaceFirst(sj,gsyms.getEmpty()));
//	    printf("empty %d\n",gsyms.findEmpty());
	    auto n=v.size();
//	    printf("n = %d ,size = %d\n",n,si.size());
	    it=si.erase(it);
//	    printf("after erase : %s ,size = %d\n",toString(*it).c_str(),si.size());
	    it=si.insert(it, v.begin(), v.end());
//	    printf("after insert size = %d\n",si.size());
//	    printf("index = %d \n",it - si.begin());
//	    printf("si 0=%s,1=%s,2=%s\n",toString(*it).c_str(),toString(*(it+1)).c_str(),toString(*(it+2)).c_str());
	    it += n;
	  }else{
	      it++;
	  }
      }
//    this->prods[i]=std::move(si); //low performance
//    it=getRightSentences(i).begin();//si has changed
//    printf("si 0=%s\n",toString(*it).c_str());
//    printf("si 0=%s,1=%s,2=%s\n",toString(*it).c_str(),toString(*(it+1)).c_str(),toString(*(it+2)).c_str());
  }


  void Gramma::reduceLeftRecursive()
  {
    //获取所有的键,次序固定
    auto vec=std::move(getProductionsHead());
    for(size_type i=0;i<vec.size();i++)
      {
      for(size_type j=0;j<i;j++)
	{
//	  printf("i=%d,j=%d,v[i]=%s,v[j]=%s\n",i,j,gsyms.getString(vec[i]).c_str(),gsyms.getString(vec[j]).c_str());
	  replaceFirstProduction(vec[i],vec[j]);
//	  printf("after replace :\n");
//	  std::cout << toString() << std::endl;
//	   while(getchar()!='\n');
	}
      reduceDirectLeftRecursive(vec[i]);
      }

  }
  void Gramma::eliminateSelfDeduction(int i)
  {
    auto &it=getRightSentences(i);
    for(size_type j=0;j!=it.size();j++)
      {
	if(it[j].getLength()==1 && it[j].startsWith(i))
	  {
	    it.erase(it.begin() + j);
	    j--;
	  }
      }
  }
  void Gramma::eliminateDuplication(int i)
  {
      for(size_type j=0;j<prods[i].size();j++)
      {
		  for(size_type k=j+1;k<prods[i].size();k++)
			{
			  bool eq=true;
			  if(prods[i][j].syms.size()!=prods[i][k].syms.size())continue;
			  for(int z=0;z<(int)prods[i][j].syms.size();z++)
				{
				  if(prods[i][j].syms[z] != prods[i][k].syms[z])
					{
					  eq=false;break;
					}
				}
			  if(eq)
				{
				  prods[i].erase(prods[i].begin() + k);
				  k--;
				}
			}
      }
  }
//  void Gramma::eliminateDuplication(std::vector<GrammaSentence>& data)
//  {
//
//   if(data.size() < 2) return;
//   bool existCommon=true;
//   std::vector<bool> check(data.size());
//   for(size_type it=0,itsz=data.size();it<itsz;it++)
//     check.push_back[it]=true;
//   int i=0;
//
//   while(existCommon)
//     {
//       existCommon=false;
//       std::vector<int> toBeReduced;
//       for(auto it=data.begin(),itend=data.end();it<itend;it++)
//          if(check[it - data.begin()] && it->getLength()<=i)toBeReduced.push_back(it - data.begin());
//
//       if(toBeReduced.size() > 0)
//         for(size_type rit=toBeReduced.size() - 1,ritend=0;rit > ritend;0) //toBeReduced[0] is reserved
//           {
//	     printf("erasing %d\n",toBeReduced[rit]);
//	     while(getchar()!='\n');
//	     data.erase(data.begin() + toBeReduced[rit]);
//           }
//
//       std::map<int,std::vector<int>> setMap;
//       setMap[gsyms.findEmpty()]=std::vector<int>();
//       int curSym;
//       for(size_type it=0,itsz=data.size();it<itsz;it++)
//	 {
//	   if(!check[it] || (toBeReduced.size()>0 && it==toBeReduced[0]))continue;
//	   curSym = data[it].syms[i];
//	   if(setMap.find(curSym)==setMap.end())
//	     setMap[curSym]=std::vector<int>();
//	   setMap[curSym].push_back(it);
//
//	   if(setMap[curSym].size() >= 2)
//	     {
//	       existCommon=true;
//	       it++;while(it<itsz)
//		 {
//		   if(data[it].syms[i]==curSym)setMap[curSym].push_back(it);
//		   it++;
//		 }
//	       for(int si=0,sisz=setMap[curSym].size();si<sisz;si++)
//		 {
//
//		 }
//	       break;
//	     }
//	 }
//
//
//
//     }
//
//
//  }
  /**
   * 将i归为两类：以s开头和不以s开头
   */
  void Gramma::reduceDirectLeftRecursive(int i)
  {
    auto &it=getRightSentences(i);
    std::vector<int> r;
    std::vector<int> nr;
    for(auto s=it.begin(),send=it.end();s!=send;s++)
      {
	if(s->startsWith(i))r.push_back(s - it.begin());
	else nr.push_back(s - it.begin());
      }
    if(r.size()>0)
      {
	//=========add EMPTY
	int newvar = gsyms.addVar(gsyms.getString(i)+ "_");
	prods[newvar] = std::vector<GrammaSentence>();
	std::vector<int> vec={gsyms.getEmpty()};
	prods[newvar].push_back(vec);

	//====add S_ to end of those aren't recursive
      for(int i:nr)
	{
	  if(it[i].getLength()==1 && it[i].startsWith(gsyms.getEmpty()))
	      it[i].syms[0]=newvar;
	  else
	    it[i].syms.push_back(newvar);
	}

      //======move those are recursive to new symbol
      for(auto itt=r.rbegin(),ittend=r.rend();itt!=ittend;itt++)
	{
	  it[*itt].syms.erase(it[*itt].syms.begin());
	  if(it[*itt].syms.size() > 1 || !it[*itt].startsWith(gsyms.getEmpty()))//不为空
		prods[newvar].push_back(std::move(it[*itt]));
	  it.erase(it.begin() + *itt);
	}
      }
  }

  void Gramma::reduceLeftFactor(int i,int j)
  {

  }
  int Gramma::reduce(std::vector<GrammaSentence> &data,const std::string & varname,int start,int end,std::vector<int>& subset)
  {
//    printf("start = %d, end = %d\n",start,end);
    if(data.size() < 2 || subset.size() < 2 || start >= end )return gsyms.getEmpty();
    //====sort subset to delete them in reversed  order
    std::sort(subset.begin(), subset.end(),[](int a,int b){return b-a;});

    //=====create new variable to hold those productions
    int newvar = gsyms.addVar(varname + "'");//A', A1',A2',A3'
    this->prods[newvar] = std::vector<GrammaSentence>();

    bool allempty=true;
    for(int subi : subset)
      {
	 std::vector<int> vec;//call each time
	if(end < (int)data[subi].getLength())
	  {
	    //copy
	     std::copy( data[subi].syms.begin() + end,data[subi].syms.end(),std::inserter(vec, vec.begin()) );
	     allempty=false;
	  }else{
	     //set empty
	      vec.push_back(gsyms.getEmpty());
	  }
	this->prods[newvar].push_back(std::move(vec));
      }
    if(allempty)//全部是空产生式，这个产生式就毫无意义，删除原来的符号，返回值为空即可
      {
	prods.erase(newvar);
	newvar = gsyms.getEmpty();
	for(std::vector<int>::size_type i=0;i<subset.size() -1 ; i++)data.erase(data.begin()+subset[i]);//仅仅保留一项即可
      }else{

      //=====create a new gs to replace those in subset
       GrammaSentence newgs;
       std::copy(data[subset[0]].syms.begin() + start,data[subset[0]].syms.begin() + end, std::inserter(newgs.syms,newgs.syms.begin()));
       newgs.syms.push_back(newvar);//[]A'  A'
       //=======delete all those in subset
       for(auto it : subset)
	 {
  // 	printf("earsing %d\n",it);
	  data.erase(data.begin() + it);
	 }
	 data.push_back(std::move(newgs));
      }

     return newvar;
  }
//  void reduce(std::vector<GrammaSentence> &data,const std::string & varname,int start,int end,std::vector<int> subset)
//  {
//    reduce(data,varname,start,end,static_cast<std::vector<int>&>(subset));
//  }
  /**
   *
   */
  void Gramma::reduceLeftFactor(std::vector<GrammaSentence> & data,const std::string & varname,int i)
  {
    //======check data size
    if(data.size()<2)return;

    bool existCommon=true;
    int namei=1;
    while(existCommon)
      {
	std::map<int,std::vector<int> > setMap;
	setMap[gsyms.getEmpty()]=std::vector<int>();
	existCommon=false;
	int sz=(int)data.size();
//	printf("before for,exist common? %d\n",existCommon);
//	 while(getchar()!='\n');
	for(int it=0;it!=sz;it++)
	  {

//	    printf("subset i = %d\n",it);
	    int curSym;
	    if( i < (int)data[it].syms.size() )
	      {
		curSym = data[it].syms[i];
		auto mit = setMap.find(curSym);
		if(mit!=setMap.end())mit->second.push_back(it);
		else		setMap[curSym] = std::vector<int>({it});//如果存在一个多分类，则直接对其进行消除，直到没有新的分类出现
	      }else{
		  curSym=gsyms.getEmpty();
		  setMap[curSym].push_back(it);
	      }
	    if(setMap[curSym].size() >= 2)
	      {
		existCommon=true;
		it++;
		int j=i,k=i;
		while(it != sz)
		  {
		    if( i < (int)data[it].syms.size() && data[it].syms[i] == curSym )setMap[curSym].push_back(it);
		    it++;
		  }
		for(;;k++)
		  for(auto itt:setMap[curSym])
		    {
		      if( k >= (int)data[itt].syms.size() ||
			  data[itt].syms[k]!=data[setMap[curSym][0]].syms[k])
			{
			  goto end;
			}
		    }
		end:
//		printf("j,k=%d,%d\n",j,k);
//		printf("subset is \n");
//		for(auto it:setMap[curSym])
//		  {
//		     printf("%d ",it);
//		  }
//		printf("\n");
		if(k > j)
		  {
			int newvar=reduce(data,varname + std::to_string(namei++),j,k,setMap[curSym]);
//			std::cout << "after reduce,this=\n"<<this->toString()<<std::endl;
//			while(getchar()!='\n');
			if(newvar != gsyms.getEmpty())
			  reduceLeftFactor(prods[newvar],gsyms.getString(newvar),0);

		  }
		break;
	      }
	  }
//	printf("after for,exist common? %d\n",existCommon);
      }
//    printf("return\n");
  }
  std::vector<int>	Gramma::getProductionsHead()const
    {
      std::vector<int> vec;
      for(auto it:prods)vec.push_back(it.first);
      return vec;
    }
 typename Gramma::SetsType Gramma::calcFirst()
  {
    bool hasnew=true;
    SetsType s;
    while(hasnew)
      {
	hasnew=false;
	for(auto &it:prods)
	  {
//	    std::cout << "current variable is "<<gsyms.getString(it.first)<<std::endl;
	    if(s.find(it.first)==s.end())s[it.first]=std::set<int>();
	     for(auto &stnc:it.second)
	       {
		 int parti=0;
//		 std::cout << "current sentence : " << toString(stnc) << std::endl;
		 while(parti < (int)stnc.getLength() && (gsyms.isSymbolVar(stnc.syms[parti]) || gsyms.isSymbolEmpty(stnc.syms[parti])))
		   {
//			 std::cout << "parti = " << parti << std::endl;
		     if(gsyms.isSymbolEmpty(stnc.syms[parti]))
		       {
				 if(s[it.first].find(stnc.syms[parti])==s[it.first].end())
				   {
//					 std::cout << "add 1" << std::endl;
					 hasnew=true;
					  s[it.first].insert(stnc.syms[parti]);
				   }
		       }
		     else if(s.find(stnc.syms[parti])!=s.end()) //当前符号已经存在
		       {
				 if(!std::includes(s[it.first].begin(),s[it.first].end(),
						  s[stnc.syms[parti]].begin(),s[stnc.syms[parti]].end()))
				   {
//					 std::cout << "add 2" << std::endl;
					 hasnew=true;
					 s[it.first].insert(s[stnc.syms[parti]].begin(), s[stnc.syms[parti]].end());
				   }
		       }

		     if(!canSentenceEmpty(stnc,parti+1))break; //no next

//		     std::cout << "this part can empty." << std::endl;
		     parti++;
		   }
		 if(parti == (int)stnc.getLength())
		   {
		     if(canSentenceEmpty(stnc) && s[it.first].find(gsyms.getEmpty())==s[it.first].end())
		       {
//		    	 std::cout << "add 3" << std::endl;
			 hasnew=true;
			 s[it.first].insert(gsyms.getEmpty());
		       }
		   }else{
		       if(gsyms.isSymbolVar(stnc.syms[parti]) && !std::includes(s[it.first].begin(),s[it.first].end(),
			   					  s[stnc.syms[parti]].begin(),s[stnc.syms[parti]].end()))
			     {
			       hasnew=true;
//			       std::cout << "add 5" << std::endl;
			       s[it.first].insert(s[stnc.syms[parti]].begin(), s[stnc.syms[parti]].end());
			     }
		       else if(gsyms.isSymbolTerm(stnc.syms[parti]) && s[it.first].find(stnc.syms[parti])==s[it.first].end())
				 {
//		    	   std::cout << "add 4" << std::endl;
				   hasnew=true;
				   s[it.first].insert(stnc.syms[parti]);
				 }
		   }
//		 std::cout << "current set is "<<std::endl;
//		 std::string str;
//		 for(auto &index:s[it.first])
//		   {
//		     str+=gsyms.getString(index) + ", ";
//		   }
//		 str+= std::string("\n");
//		 std::cout << str;
//		 while(getchar()!='\n');
	       }
	  }
//	std::cout << "current whole set is "<<std::endl;
//	std::string strfirstset;
//	std::for_each(s.begin(),s.end(),[&strfirstset,this](auto &it){
//	  strfirstset += this->gsyms.getString(it.first) + "{ \n";
//	  std::for_each(it.second.begin(),it.second.end(), [&strfirstset,this](int i){
//	    strfirstset += this->gsyms.getString(i) + ", ";
//	  });
//	  strfirstset += std::string("\n}\n");
//	});
//	std::cout << strfirstset << std::endl;
//	 while(getchar()!='\n');
      }
    return s;

  }
 std::set<int> Gramma::calcFirst(const GrammaSentence & gs,int start,int end,const SetsType& firstset)
 {
	 std::set<int> set;
	 int i=start;
	 while(i<end)
	 {
		 if(firstset.find(gs.syms[i])!=firstset.end())
		 {
			const std::set<int>& temp=firstset.at(gs.syms[i]);
			set.insert(temp.begin(),temp.end());
		 }
		if(!canSymbolEmpty(gs.syms[i]))break;
		i++;
	 }
	 if(i==end)
		 set.insert(gsyms.getEmpty());
	 else if(gsyms.isSymbolTerm(gs.syms[i]))
		  set.insert(gs.syms[i]);
	 if(set.empty())set.insert(gsyms.getEmpty());
	 return set;
 }
 typename Gramma::SetsType Gramma::calcFollow(const SetsType& firstset,int startSym,int endSym)
 {
   SetsType s;
   SetsType deps;// calculate the dependency of i, i's FOLLOW change --> deps[i]'s FOLLOW change

   //========初始化开始符号和结束符号
   s[startSym]=std::set<int>();
   s[startSym].insert(endSym);

   //========将FIRST集加入FOLLOW集中
   for(auto &it:prods)
     {
//	 std::cout << "current variable is " << gsyms.getString(it.first) << std::endl;
	 for(auto &stnc:it.second)
	   {
//	     std::cout << "current sentence is " << toString(stnc) << std::endl;
	     //======对每个句型中的 第parti个符号进行设置FOLLOW集
	     size_type parti=0;
	     for(;parti < stnc.getLength();parti++)
	       {
		 int curSym=stnc.syms[parti];
//		 std::cout << "current parti is " << parti << ",  curSym is " << toString(curSym) << std::endl;
		 if(!gsyms.isSymbolVar(curSym))continue;  //如果不是变量，就不进行下面的检查

		 //=======初始化FOLLOW集合和deps集合
		 if(s.find(curSym)==s.end())s[curSym]=std::set<int>();
		 if(deps.find(it.first)==deps.end())deps[it.first]=std::set<int>();

		 //===========从curSym之后的每个符号检查空句型，并将下一个变量或者终结符或者EMPTY的FIRST集加入
		 size_type partj=parti+1;
		 for(;partj < stnc.getLength();partj++)//对 [parti+1,) 的子句型检查，直到检查到第一个不为空的值;如果最后找到末尾
		   //可以为空，则填上依赖关系 curSym-->it.first
		   {

		     //FOLLOW(curSym) += FIRST(A..)
		     if(gsyms.isSymbolTerm(stnc.syms[partj])) //对于终结符，直接加入
		       {
//			 std::cout << "add terminator" << toString(stnc.syms[partj]) << std::endl;
			 s[curSym].insert(stnc.syms[partj]);
		       }
		     else if(!gsyms.isSymbolEmpty(stnc.syms[parti])){	//非空符号，是变量
//			 std::cout << "add FIRST(" << toString(stnc.syms[partj]) << ")"<< std::endl;
			 const std::set<int> &fset=firstset.at(stnc.syms[partj]);
			 s[curSym].insert(fset.begin(),fset.end());
		     }
		     if(!canSymbolEmpty(stnc.syms[partj]))// curSym A B C
		       {
			   break;
		       }
		   }
		   if(partj==stnc.getLength() && it.first!=curSym) //查到依赖关系,如果是B-->B这种，就不用保留
		     {
//		       std::cout << "find deps,  " <<toString(it.first) << "  contains "<< toString(curSym) << std::endl;
		       deps[it.first].insert(curSym);
		     }
	       }
//	     while(getchar()!='\n');
	   }
     }
   //=====除去FOLLOW集中的EMPTY
//   std::cout << "removing empty from FOLLOWs"<<std::endl;
//   while(getchar()!='\n');
   int empty=gsyms.getEmpty();
   for(auto &it:s)
     {
       it.second.erase(empty);
     }

   //======打印deps
//   std::cout << "map of deps" << std::endl;
//   for(auto it:deps)
//     {
//       std::cout << toString(it.first) << ": { ";
//       for(int i:it.second)
//	 {
//	   std::cout << toString(i) << ", ";
//	 }
//       std::cout << "}" << std::endl;
//     }

   //=======循环检测deps集合中的变化直到FOLLOW集直到没有新的集合变化
   std::map<int,bool> changed;//将所有的置为changed
   for(auto it:s)
     {
       changed[it.first]=true;
     }
   bool hasnew=true;
   while(hasnew)
     {
       hasnew=false;
//       std::cout << "map of changed" << std::endl;
       for(auto &it:changed)
	 {
//	   std::cout << toString(it.first) << " == " << it.second << std::endl;
	   if(it.second==true)//changed
	     {
	       hasnew=true;
	       for(int itt:deps[it.first])//all deps will be changed
		 {
		   if(!std::includes(s[itt].begin(), s[itt].end(), s[it.first].begin(), s[it.first].end()))
		     {
			 s[itt].insert(s[it.first].begin(), s[it.first].end());
			 changed[itt]=true;
		     }
		 }
	       it.second=false;//that has been done.
	     }
	 }
//       while(getchar()!='\n');


     }
   return s;
 }

 std::string Gramma::toString(const SetsType& set)const
 {
	 std::string s;
	   std::for_each(set.begin(),set.end(),[&s,this](const Gramma::SetsType::value_type &it){
	     s+= this->gsyms.getString(it.first) + "{\n\t\t";
	     std::for_each(it.second.begin(),it.second.end(), [&s,this](int i){
	       s += this->gsyms.getString(i) + ", ";
	     });
	     s += std::string("\n}\n");
	   });
	   return s;
 }
 std::string Gramma::toString(const std::set<int> & set)const
 {
	 std::string s;
	 std::for_each(set.begin(),set.end(),[this,&s](int i){
		 s+=this->gsyms.getString(i)+" ";
	 });
	 return s;
 }

 //============class LRGramma
 LRGramma::LRGramma(const std::initializer_list<std::pair<int,std::string> > &list,
	   const std::initializer_list<std::pair<int,GrammaSentence> > &prodlist,int oristart,int oriend,const std::string & strstart):
		send(oriend),dotString(".")
 {
	 gsyms = list;
	auto it=prodlist.begin(),itend=prodlist.end();
	for(;it!=itend;it++)
	  {
		addProduction(it->first,it->second);
	  }

	 sstart = gsyms.addVar(strstart);
	 GrammaSentence tempgs={oristart};
	 addProduction(sstart,std::move(tempgs));
 }
 LRGramma::LRGramma(const GrammaSymbols & gs,const ProductionsType & prods,int oristart,int oriend,const std::string & strstart):
		send(oriend),dotString(".")
 {
	 this->gsyms=gs;
	 this->prods=prods;

	 sstart = gsyms.addVar(strstart);
	 GrammaSentence tempgs={oristart};
	 addProduction(sstart,std::move(tempgs));
 }
 LRGramma::LRGramma(const GrammaSymbols & gs,ProductionsType && prods,int oristart,int oriend,const std::string & strstart):
		send(oriend),dotString(".")
 {
	 this->gsyms=gs;
	 this->prods=prods;

	 sstart = gsyms.addVar(strstart);
	 GrammaSentence tempgs={oristart};
	 addProduction(sstart,std::move(tempgs));
 }
 LRGramma::LRGramma( GrammaSymbols && gs,const ProductionsType & prods,int oristart,int oriend,const std::string & strstart):
		send(oriend),dotString(".")
 {
	 this->gsyms=gs;
	 this->prods=prods;

	 sstart = gsyms.addVar(strstart);
	 GrammaSentence tempgs={oristart};
	 addProduction(sstart,std::move(tempgs));
 }
 LRGramma::LRGramma(GrammaSymbols && gs,ProductionsType && prods,int oristart,int oriend,const std::string & strstart):
		send(oriend), dotString(".")
 {
	 this->gsyms=gs;
	 this->prods=prods;

	 sstart = gsyms.addVar(strstart);
	 GrammaSentence tempgs={oristart};
	 addProduction(sstart,std::move(tempgs));
 }
 std::tuple<LRGramma::ClosuresVector,std::set<int>,std::map<std::pair<int,int>,int>> LRGramma::getAllClosures()
 {
	 ClosuresVector itemsets;//项目族
//	 std::map<ItemType,int>		C0;//ItemType --> {ItemType}
	 std::map<std::pair<int,int>,int> C2;
	 std::set<int>		iC; //the concanical closure

	 ItemType tempitem=std::make_tuple(getGStart(),0,0);
	 ClosureType temp;
	 temp.insert(tempitem);
//	 itemsets.push_back(temp); //S' -> .S
//	 int C0_tempitem = itemsets.size() - 1;
//	 C0[tempitem] = C0_tempitem;
	 itemsets.push_back(getClosure(temp));
	 int iC_C0_tempitem = itemsets.size() - 1;
	 iC.insert(iC_C0_tempitem);

//	 std::cout << "init, [0] are" << std::endl;
//	 std::cout << toString(itemsets[0]) << std::endl;
//	 std::cout << toString(itemsets[1]) << std::endl;

//	 while(getchar()!='\n');
	 bool hasnew=true;
	 while(hasnew)
	 {
		 hasnew=false;
		 for(int iic:iC)//iC中所有已经求出的CLOSUER
		 {
			 ClosureType &curClo = itemsets[iic];
//			 std::cout << "current closure  at " << iic  << " is "<< std::endl;
//			 std::cout << toString(curClo) << std::endl;

//			 while(getchar()!='\n');
			 for(auto &eachItem : curClo)//对curClo中的.的每个后继符号x，将GOTO(curClo,x)在不属于C1的情况下加入C1中
			 {
//				 std::cout << "current production is " << toString(eachItem) << std::endl;
				 int x=getFirstSymbolAfterDot(eachItem);
//				 std::cout << "current expect SYMBOL is " << gsyms.getString(x) << std::endl;
				 if(x!=gsyms.getEmpty() && x!=getGEnd())//不是空符号和结束符
				 {
					 auto iic_x_key=std::pair<int,int>(iic,x);
					 auto itgoto=C2.find(iic_x_key);
					 if(itgoto==C2.end())//如果 GOTO(curClo,x)尚未存在,则求其CLOSUER
					 {
//						 std::cout << "curClo is " << std::endl << toString(curClo) << std::endl;
//						 std::cout << "goto not present "<<std::endl;

						 //===========求得GOTO集
						 ClosureType iic_x_goto=getGoto(curClo,x);
						 //===如果iic_x_goto与当前求出的某个集合相等，就不需要添加

						 //=========判断求得的GOTO集合是否已经存在与序列中，如果不存在就加入，否则仅仅记录GOTO的映射关系
						 auto itemit=std::find(itemsets.begin(),itemsets.end(),iic_x_goto);
						 int iic_x_key_index;
						 if(itemit==itemsets.end())
						 {
//							 std::cout << "after goto in vector" << std::endl;
							 itemsets.push_back(iic_x_goto); //curClo has changed, because itemsets are changing, so any memory reference is not good
							 iic_x_key_index = (int)itemsets.size() - 1;
							 iC.insert(iic_x_key_index);  //加入集合之中,此时集合已经改变，需要重新提起遍历过程
//							 std::cout << "GOTO(curClo," << gsyms.getString(x) << ") = " << std::endl
//									 << toString(itemsets[iic_x_key_index]) << std::endl;
							 hasnew=true;
							 break; //everything has changed
						 }else{
//							 std::cout << "found duplication" << std::endl;
//							 while(getchar()!='\n');
							 iic_x_key_index = itemit - itemsets.begin();
						 }
						 C2[iic_x_key] = iic_x_key_index; //记录GOTO(curClo,x)
					 }


				 }
			 }
			 if(hasnew)break;
		 }
	 }
	 //使用增强版的goto：
	 // 能够同itemsets，C0，C1,C2进行协作
	 return std::make_tuple(itemsets,iC,C2);
 }
 /**
  * if not set, then set
  *   at the end,get
  */
 int 	LRGramma::getClosureInVector(const ItemType& i,ClosuresVector &itemsets, std::map<ItemType,int>& C0)
 {
	 auto it=C0.find(i);
	 std::cout << "get closure of " << toString(i) << std::endl;
	 if(it==C0.end()) //not found
	 {
		 ClosureType C=std::move(getClosure(i));
		 itemsets.push_back(C);
		 std::cout << "new a closure at " << itemsets.size() - 1 << " is " << std::endl;
		 std::cout << toString(itemsets[itemsets.size() - 1]) << std::endl;
		 return (C0[i] = (int)itemsets.size() - 1);
	 }else{
		 std::cout << "get existing closure at " << it->second << " is " << std::endl;
		 std::cout << toString(itemsets[it->second]) << std::endl;
//		 while(getchar()!='\n');
		 return it->second;
	 }
 }
 typename LRGramma::ClosureType LRGramma::getClosure(const ItemType& i)
 {
	 ClosureType C;
	 getClosure(i,C);
	 return C;
 }
 void	LRGramma::getClosure(const ItemType& i,ClosureType& C)
 {
	 C.insert(i);
	 std::map<int,bool> added;
	 for(auto &it:prods)
	 {
		 added[it.first]=false;
	 }
	 bool hasnew=true;
	 while(hasnew)
	 {
		 hasnew=false;
		 for(auto & cit : C)
		 {
			 int v = getFirstSymbolAfterDot(cit);
			 if(gsyms.isSymbolVar(v) && !added[v])
			 {
				 size_type sz=prods[v].size();
				 for(size_type i=0;i<sz;i++)
				 {
					 ItemType tempItem(v,i,0);
					 if(C.find(tempItem)==C.end())
					 {
						 C.insert(std::move(tempItem));
						 hasnew=true;
					 }
				 }
				 added[v]=true;
				 if(hasnew)break; //C has been modified
			 }
		 }
	 }
 }

 typename LRGramma::ClosureType LRGramma::getClosure(const ClosureType & C)
 {
	 ClosureType sumC;
	 for(auto &cit:C)
	 {
		 getClosure(cit,sumC);
	 }
	 return sumC;
 }
 typename LRGramma::ClosureType LRGramma::getGotoInVector(int iclo,int x, ClosuresVector& itemsets, std::map<ItemType,int>	&	C0)
 {
	 ClosureType GOTO;
	 bool hasnew=true;
	 std::cout << "getting GOTO for ( " << std::endl;
	 std::cout << toString(itemsets[iclo]) << std::endl << " ," << gsyms.getString(x) << ")" << std::endl;
	 while(hasnew)
	 {
		 hasnew=false;
		 ClosureType curset=itemsets[iclo];
		 for(auto &item:curset)
		 {
			 std::cout << "current item is " << toString(item) << std::endl;
			 if(getFirstSymbolAfterDot(item)!=x)continue;
			 auto I=std::make_tuple(std::get<0>(item),
					 std::get<1>(item),
					 std::get<2>(item)+1);
			 ClosureType iClosure = itemsets[getClosureInVector(I,itemsets,C0)];
			 std::cout << "finding a closure for GOTO is "<< toString(iClosure) << std::endl;
			 if(!std::includes(GOTO.begin(), GOTO.end(),
					 iClosure.begin(),iClosure.end()))
			 {
				 std::cout << "not containing,insert." << std::endl;
				 GOTO.insert(iClosure.begin(),iClosure.end());
				 hasnew=true;
			 }else{
				 std::cout << "already containing" << std::endl;
			 }
			 while(getchar()!='\n');
		 }
		 std::cout << "loop once, hasnew = " << hasnew << std::endl;
		 while(getchar()!='\n');
	 }
	 return GOTO;

 }
 typename LRGramma::ClosureType LRGramma::getGoto(const ClosureType & items,int x)
 {
//	 std::cout << "wait,size is" << items.size() << std::endl;
//	 while(getchar()!='\n');

//	 std::cout << "getting goto of " << toString(items) << std::endl;
//	 while(getchar()!='\n');
	 ClosureType GOTO;
	 bool hasnew=true;
//	 std::map<int,bool> added;
//	 for(auto &it:prods)
//		 added[it.first]=false;
	 while(hasnew)
	 {
		 hasnew=false;
		 for(auto &item : items)
		 {
			 if(getFirstSymbolAfterDot(item)!=x)continue;
			 auto I=std::make_tuple(std::get<0>(item),
					 std::get<1>(item),
					 std::get<2>(item)+1);
			 ClosureType iClosure = getClosure(I);
			 if(!std::includes(GOTO.begin(), GOTO.end(),
					 iClosure.begin(),iClosure.end()))
			 {
				 GOTO.insert(iClosure.begin(),iClosure.end());
				 hasnew=true;
			 }
		 }
	 }
	 return GOTO;
 }
 int  LRGramma::getNthSymboleAfterDot(const ItemType & i,unsigned int j)
 {
	 if(j == 0)
		 return gsyms.getEmpty();
	 const GrammaSentence & gs=prods[std::get<0>(i)][std::get<1>(i)];
	 int dotIndex=std::get<2>(i);
	 int symIndex = dotIndex + j -1;
	 if(symIndex< (int)gs.getLength())
		 return gs.syms[symIndex];
	 else if(std::get<0>(i) == getGStart() && j==gs.getLength()) //start的末尾是结束符号
		 return getGEnd();
	 else
		  return gsyms.getEmpty();
 }
 std::string LRGramma::toString(const ItemType& item)const
 {
	 std::string s;
	 const GrammaSentence &gs=prods.at(std::get<0>(item))[std::get<1>(item)];
	 s+=gsyms.getString(std::get<0>(item)) + " -> ";
	 size_type doti=(size_type)std::get<2>(item);
	 size_type i=0,sz=gs.getLength();
	 for(;i<doti;i++)s+=gsyms.getString(gs.syms[i]) + " ";
	 s+= dotString + " ";
	 for(;i<sz;i++)s+=gsyms.getString(gs.syms[i]) + " ";
	 return s;
 }
 std::string LRGramma::toString(const ClosureType& closure)const
 {
	 std::string s;
//	 std::cout << "entering" << std::endl;
	 std::for_each(closure.begin(),closure.end(),[this,&s](const ItemType&  i)
			 {
		 	 	 s += this->toString(i) + "\n";
			 }
			 );
//	 std::cout << "return " << std::endl;
	 return s;
 }
 std::string LRGramma::toString(const  std::tuple<ClosuresVector,std::set<int>,std::map<std::pair<int,int>,int>> & tups)const
 {
	 auto &vec=std::get<0>(tups);
	 auto &iC=std::get<1>(tups);
	 auto C2=std::get<2>(tups);
	 std::string s;
	 s+= std::string("CLOSURES:\n");
	 for(auto i:iC)
	 {
		 s += std::to_string(i)+": { \n" + toString(vec[i]) + "}\n\n";
	 }
	 s+=std::string("GOTO:\n");
	 for(auto &p:C2)
	 {
		 s += std::string("<") + std::to_string(p.first.first) + ", " + gsyms.getString(p.first.second) + "> =  "+
				 std::to_string(p.second) + "\n";
	 }
	 return s;
 }

 //=========class LR1Gramma
 typename LR1Gramma::ClosureType	LR1Gramma::getClosure(const ItemType& i)
 {
	 Gramma::SetsType calcedFirst = this->Gramma::calcFirst();
//	 std::cout << "after get FIRST" << std::endl;
//	 std::cout << Gramma::toString(calcedFirst) << std::endl;

	return getClosure(i,calcedFirst);
 }
 typename LR1Gramma::ClosureType	LR1Gramma::getClosure(const ItemType& i,const Gramma::SetsType &firstset)
 {
	 ClosureType C;
	 getClosure(i,C,firstset);
	 return C;
 }
 void	LR1Gramma::getClosure(const ItemType& i,ClosureType& C,const Gramma::SetsType &firstset)
 {
	 //====初始化集合只有一个元素 i中的产生式
	 C.insert(i);

	 std::set<std::pair<int,int>> added;//记录 S->B.AC 中 A的所有产生式是否已经加入,默认情况下是false
	 std::queue<ItemType>	waits;
	 std::set<ItemType>		processed;//for time complexity to O(1)(hashset) or O(logn)(treeset)
	 waits.push(i);

	 while(!waits.empty())
	 {
		 const auto &cit=waits.front();
		 waits.pop();
		 if(processed.find(cit)!=processed.end())continue;

		 //========add (v,i,0,FIRST(sym cit[3])) 获取句型sym,cit[3]的FIRST集
		 GrammaSentence& gs=prods[std::get<0>(cit)][std::get<1>(cit)];

		 //=====在已有的firstset的基础上计算first(beta)，并且如果beta能够为空，将a加入其中
		 std::set<int> fsets=Gramma::calcFirst(gs,std::get<2>(cit)+1,(int)gs.getLength() ,firstset);
		 auto itempty=fsets.find(gsyms.getEmpty());
		 if(itempty!=fsets.end())//移除empty
		 {
			 fsets.erase(itempty);
			 fsets.insert(std::get<3>(cit));
		 }
		 //=====当前表达式 A->C.BD 中的v=B
		 int v = getFirstSymbolAfterDot(cit);
		 if(gsyms.isSymbolVar(v))
		 {
			 size_type sz=prods[v].size();
			 //===将v->..的所有表达式加入SET中
			 for(int fsym:fsets)
			 {
				 if(added.find({v,fsym})!=added.end())continue;
				 for(size_type i=0;i<sz;i++)
				 {
					 ItemType tempItem(v,i,0,fsym);
					 //if(C.find(tempItem)==C.end())//prevent repeat ; NOTE set is no repeat
					 C.insert(tempItem);
					 waits.push(tempItem);
				 }
				 added.insert({v, fsym});
			 }
		 }
		 processed.insert(cit);
	 }
 }
 typename LR1Gramma::ClosureType	LR1Gramma::getClosure(const ClosureType & C,const Gramma::SetsType &firstset)
 {
	 ClosureType sumC;
	 for(auto &cit:C)
	 {
		 getClosure(cit,sumC,firstset);
	 }
	 return sumC;
 }
 typename LR1Gramma::ClosureType	LR1Gramma::getGoto(const ClosureType& clo,int x,const Gramma::SetsType &firstset)
 {
	 ClosureType GOTO;
	 bool hasnew=true;
	 while(hasnew)
	 {
		 hasnew=false;
		 for(auto &item : clo)
		 {
			 if(getFirstSymbolAfterDot(item)!=x || (std::get<0>(item)==getGStart() && x==getGEnd()))continue;
			 ItemType I=item;
			 std::get<2>(I) += 1; //point to next
			 ClosureType iClosure = getClosure(I,firstset);
			 if(!std::includes(GOTO.begin(), GOTO.end(),
					 iClosure.begin(),iClosure.end()))
			 {
				 GOTO.insert(iClosure.begin(),iClosure.end());
				 hasnew=true;
			 }
		 }
	 }
	 return GOTO;
 }
 typename LR1Gramma::InfoType LR1Gramma::getAllClosures()
 {
	 LR1Gramma::ClosuresVector itemsets;//记录所有产生的Closure
	 std::map<ClosureType,int>		itemsetsRecord;//辅助记录,为了更快的查找时间
	std::map<std::pair<int,int>,int> C2;//记录GOTO关系
	std::set<int>		iC;//记录所有的规范Closure

	 Gramma::SetsType calcedFirst = this->Gramma::calcFirst();

	 ItemType tempitem=std::make_tuple(getGStart(),0,0,getGEnd());//由拓广文法的唯一产生式得到CLOSURE 0
	 ClosureType temp;
	 temp.insert(tempitem);
	 itemsets.push_back(getClosure(temp,calcedFirst));
	 itemsetsRecord[itemsets[0]]=0;

	 int iC_C0_tempitem = itemsets.size() - 1;
	 iC.insert(iC_C0_tempitem);

	 std::queue<int> waits;
	 std::set<int>   processed;
	 waits.push(iC_C0_tempitem);
	 while(!waits.empty())
	 {
		 int iic=waits.front();
		 waits.pop();
		 if(processed.find(iic)!=processed.end())continue;

		 //取出当前的Closure
		 ClosureType &curClo = itemsets[iic];

		 //对当前Closure的每个后继符号.x
		 //如果GOTO(curClo,x)不属于
		 std::map<ClosureType,int> tempitems;
		 int						curIndex=0;
		 int base=(int)itemsets.size();
		 for(auto &eachItem : curClo)
		 {
			 int x=getFirstSymbolAfterDot(eachItem);
			 //==如果不是空符号则求其Closure,并检查这个Closure是否已经存在于现有的Closure中，如果不存在，就将其加入
			 if(x!=gsyms.getEmpty() && C2.find({iic,x})==C2.end())
			 {
				 //===avoid S'->S.,$ to generate EMPTY
				 if(std::get<0>(eachItem)==getGStart() && x==getGEnd())continue;

				 ClosureType iic_x_goto=getGoto(curClo,x,calcedFirst);//获取Closure
				 auto itemit=itemsetsRecord.find(iic_x_goto);//检查其是否已经存在
				 int iic_x_key_index;
				 if(itemit==itemsetsRecord.end())
				 {
					 if( (itemit=tempitems.find(iic_x_goto))==tempitems.end())
					 {
						 iic_x_key_index = base + curIndex;
						 itemsetsRecord[iic_x_goto]=iic_x_key_index;
						 tempitems[iic_x_goto]=curIndex++;
					 }else{
						 iic_x_key_index = base + itemit->second;
					 }
					 iC.insert(iic_x_key_index);
					 waits.push(iic_x_key_index);
				 }else{
					 iic_x_key_index = itemit->second;
				 }
				 C2[{iic,x}] = iic_x_key_index; //记录GOTO(curClo,x)
			 }
		 }
		 //itemsets changed but delayed until here
		 itemsets.resize(base + curIndex);//resize to contain more

		 for(auto &p:tempitems)
		 {
			 itemsets[base + p.second]=std::move(p.first);
		 }
		 processed.insert(iic);
	 }
	 return std::make_tuple(itemsets,iC,C2);
 }
 /**
  * constructing algorithm:
  * 构造项目集规范族和GOTO
  * 对项目集规范族中的每一项Ii
  * 	对Ii中的每一项 [A->C.DE,b],如果D不是变量
  * 		如果D是终结符号，则ACTION[i,D]=移入GOTO(i,D)的值
  * 		如果D是空（表明一定不是S'->S.),将ACTION[i,a]设置为规约A->C.DE
  * 		如果D是结束符号,意味着b也是结束符号，设置ACTION[i,$]=接受
  *  //如果上面的循环出现重复，就意味着冲突，则这个文法不是LR(1)的
  *  //还应当返回冲突信息 CorruptType,这是ACTION类型的扩展集，它可以在一个转移上支持多个状态
  *
  * 对于没有设置的项，默认为ERROR
  * 初始状态是I0
  *
  *
  */
typename LR1Gramma::CorruptTableType 	LR1Gramma::constructAnalyzeTable(const InfoType& info)
 {
	 	 const auto &vec=std::get<0>(info);//all closures
		 const auto &iC=std::get<1>(info);//concanical closures
		 const auto &C2=std::get<2>(info);//goto
		LR1Gramma::CorruptTableType corruptAction;


		for(int i:iC)
		{
//			std::cout << "current closure is "<<i<<std::endl;
			for(auto & item: vec[i])
			{
//				std::cout << "current item is "<<toString(item)<<std::endl;

				 int nextsym=getFirstSymbolAfterDot(item);
				 if(nextsym==6)
				 {
					 std::cout << std::endl;
				 }
				 if(gsyms.isSymbolVar(nextsym))continue;
				 else if(gsyms.isSymbolTerm(nextsym))
				 {
					 std::pair<int,int> key={i,nextsym};
					 if(corruptAction.find(key)==corruptAction.end())
						 corruptAction[key]=std::set<std::tuple<int,int,int>>();
					 if(nextsym!=getGEnd())
						 corruptAction[key].insert({C2.at({i,nextsym}),-1,ACTION_SHIFT_IN});
					 else
						 corruptAction[key].insert({-1,-1,ACTION_ACCEPT});
				 }
				 else if(gsyms.isSymbolEmpty(nextsym))
				 {
					 std::pair<int,int> key={i,std::get<3>(item)};
					 if(corruptAction.find(key)==corruptAction.end())
						 corruptAction[key]=std::set<std::tuple<int,int,int>>();
					 corruptAction[key].insert({std::get<0>(item),std::get<1>(item),ACTION_REDUCE});
				 }
			}
		}
		return corruptAction;
 }

 /**
  *  j=0  return EMPTY
  *  j=1 return first
  *  j=2 return second
  */
 int		  LR1Gramma::getNthSymboleAfterDot(const ItemType & i,unsigned int j)
 {
	 if(j == 0)
		 return gsyms.getEmpty();
	 const GrammaSentence & gs=prods[std::get<0>(i)][std::get<1>(i)];
	 int dotIndex=std::get<2>(i);
	 int symIndex = dotIndex + j -1;
	 if(symIndex< (int)gs.getLength())
		 return gs.syms[symIndex];
	 else if(std::get<0>(i) == getGStart() && j==gs.getLength()) //start的末尾是结束符号
		 return getGEnd();
	 else
		  return gsyms.getEmpty();
 }
 std::string LR1Gramma::toString(const ItemType& i)const
 {
	 std::string s="[";
	 LRGramma::ItemType triple=std::make_tuple(std::get<0>(i),std::get<1>(i),std::get<2>(i));
	 s += std::move(LRGramma::toString(triple)) + ", " + gsyms.getString(std::get<3>(i)) + "]";
	 return s;
 }
 std::string LR1Gramma::toString(const ClosureType& closure)const
 {
	 std::string s;
	 std::for_each(closure.begin(),closure.end(),[this,&s](const ItemType&  i)
			 {
				 s += this->toString(i) + "\n";
			 }
			 );
	 return s;
 }
 std::string LR1Gramma::toString(const InfoType& info)const
 {
	 auto &vec=std::get<0>(info);
	 auto &iC=std::get<1>(info);
	 auto &C2=std::get<2>(info);
	 std::string s;
	 s+= std::string("CLOSURES:\n");
	 for(auto i:iC)
	 {
		 s += std::to_string(i)+": { \n" + toString(vec[i]) + "}\n\n";
	 }
	 s+=std::string("GOTO:\n");
	 for(auto &p:C2)
	 {
		 s += std::string("<") + std::to_string(p.first.first) + ", " + gsyms.getString(p.first.second) + "> =  "+
				 std::to_string(p.second) + "\n";
	 }
	 return s;
 }
 std::string LR1Gramma::toString(const AnaylzeTableType& tableInfo)const
 {
	 std::string s("Analyze Table:\n");
	 for(auto &p:tableInfo)
	 {
		 s+="action["+std::to_string(p.first.first)+", "+gsyms.getString(p.first.second)+"] = ";
		 int actionType=std::get<2>(p.second);
		 switch(actionType)
		 {
		 	 case ACTION_SHIFT_IN:
		 		s+=This::ACTION_INFO_STRING[actionType] + " " + std::to_string(std::get<0>(p.second));
		 		break;
		 	 case ACTION_REDUCE:
		 		s+=This::ACTION_INFO_STRING[actionType] + " " + Gramma::toString(std::get<0>(p.second),std::get<1>(p.second));
		 		break;
		 	 case ACTION_ACCEPT:
		 		 s+=This::ACTION_INFO_STRING[actionType];
		 		 break;
		 	 case ACTION_ERROR:
		 	 default://ACTION_ERROR
		 		 s+=This::ACTION_INFO_STRING[ACTION_ERROR];
		 		 break;
		 }
		 s+="\n";
	 }
	 return s;
 }
 std::string LR1Gramma::toString(const CorruptTableType& tableInfo)const
 {
	 std::string s("[Corrupt]Analyze Table:\n");
	 for(auto &p:tableInfo)
	 {
		 s+="action["+std::to_string(p.first.first)+", "+gsyms.getString(p.first.second)+"] = ";
		 for(auto &tup : p.second)
		 {
			 int actionType=std::get<2>(tup);
			 switch(actionType)
			 {
				 case ACTION_SHIFT_IN:
					s+=This::ACTION_INFO_STRING[actionType] + " " + std::to_string(std::get<0>(tup));
					break;
				 case ACTION_REDUCE:
					s+=This::ACTION_INFO_STRING[actionType] + " " + Gramma::toString(std::get<0>(tup),std::get<1>(tup));
					break;
				 case ACTION_ACCEPT:
					 s+=This::ACTION_INFO_STRING[actionType];
					 break;
				 case ACTION_ERROR:
				 default://ACTION_ERROR
					 s+=This::ACTION_INFO_STRING[ACTION_ERROR];
					 break;
			 }
			 s+=", ";
		 }
		 s+="\n";
	 }
	 return s;
 }



} /* namespace x2 */
