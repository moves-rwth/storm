/* This file is part of the Multiagent Decision Process (MADP) Toolbox. 
 *
 * The majority of MADP is free software released under GNUP GPL v.3. However,
 * some of the included libraries are released under a different license. For 
 * more information, see the included COPYING file. For other information, 
 * please refer to the included README file.
 *
 * This file has been written and/or modified by the following people:
 *
 * Frans Oliehoek 
 * Matthijs Spaan 
 *
 * For contact information please see the included AUTHORS file.
 */


#include "Scope.h"
#include <algorithm>

using namespace std;

Scope::Scope(const string &s)
{
    stringstream ss;
    ss.str(s);
    ss >> *this;
}

ostream& operator<< (ostream& o, const Scope& s)
{
    const vector<Index>& v = s;
    return o << SoftPrintVector( v );
}

istream& operator>> (istream& i, Scope& s)
{
    char c = 0;
    i >> c;
    if(c != '<')
    {
        cerr << "Scope extraction error: expected '<', but got '"<<c<<"' !" << endl;
        i.setstate(ios_base::failbit);
    }
    while( i.good() )
    {
        Index Index;
        i >> Index;   
#if 0        
        cout << "parsed Index="<<Index;
        cout << ",\ti.good="<<i.good();
        cout << ",\ti.eof="<<i.eof();
        cout << ",\ti.fail="<<i.fail();
        cout << ",\ti.bad="<<i.bad()<<endl;
#endif        
        if(i.good() )
            s.Insert(Index);
    }
    //failed to parse an Index
    i.clear(); //reset status
    i >> c;
    if(c != '>')
    {
        cerr << "Scope extraction error: expected '>', but got '"<<c<<"' !" << endl;
        i.setstate(ios_base::failbit);
    }

    return i;
}

void Scope::Insert(const Scope& s)
{
    SDT::const_iterator it = s.begin();
    SDT::const_iterator last = s.end();
    while(it != last)
    {
        Index i = *it;
        //check if i not in this yet.
        if(! this->Contains(i) )
            this->Insert(i);
        it++;
    }
}

void Scope::Remove(const Scope& s)
{
    SDT::const_iterator it = s.begin();
    SDT::const_iterator last = s.end();    
    while(it != last)
    {
        Index i = *it; //the index to remove from 'this'
        SDT::iterator remove_it = this->GetIteratorForIndex(i);
        if( remove_it != this->end() )
            this->erase(remove_it);

        it++;
    }

}
bool Scope::Contains(Index i) const
{
    SDT::const_iterator found_it = find(this->begin(), this->end(), i);
    return( found_it != this->end() );// element is in *this* scope
}

bool Scope::IsSubSetOf(const Scope& s) const
{
    Scope interS = Intersection(*this, s);
    if(interS.size() == this->size() )
        //all elements in this were also contained by s
        return true;
    return false;
}

Scope Scope::Intersection(const Scope& a, const Scope& b)
{
    Scope result;
    SDT::const_iterator it = a.SDT::begin();
    SDT::const_iterator last = a.SDT::end();
    while(it != last)
    {
        Index i = *it;
        if(b.Contains(i))
            result.Insert(i);
        it++;
    }
    return result;
}

bool Scope::Equals(const Scope& s) const
{
    size_t siz= this->size();
    if(siz != s.size())
        return false;

    for(Index i=0; i < siz; i++)
        if( (*this)[i] != s[i] )
            return false;

    return true;

}

SDT::iterator Scope::GetIteratorForIndex(Index i) 
{
    SDT::iterator found_it = find(this->begin(), this->end(), i);
    return( found_it );
}


Index Scope::GetPositionForIndex(Index i) const
{
    SDT::const_iterator it = this->begin();
    Index pos = 0;
    while( *it != i )
    {
        pos++;
        it++;
        if(it == this->end())
        {
            stringstream ss;
            ss << "Scope::GetPositionForIndex(Index i=" << i <<") not found!";
            throw E(ss);
        }

    }
    // *it == i, so...
    return(pos);
}

Scope& Scope::Sort()
{
    sort(this->begin(),this->end());
    return *this;
}

// --------------------------------
// sorting based on:
// http://stackoverflow.com/questions/17074324/how-can-i-sort-two-vectors-in-the-same-way-with-criteria-that-uses-only-one-of
// rewriten for C++98 and simplified
template <typename T>
class CompareVec
{
public:
    typename std::vector<T> vec;
    CompareVec(typename std::vector<T> vec) : vec(vec)
    {}

    bool operator()(std::size_t a,std::size_t b)
    {  return vec[a]<vec[b];}
};

template <typename T>
std::vector<std::size_t> sort_permutation(
    const std::vector<T>& vec)
{
    std::vector<std::size_t> p(vec.size());
    for (int i=0;i<vec.size();i++)
        p[i]=i;

    CompareVec<T> compareVec(vec);
    std::sort(p.begin(), p.end(),compareVec);
    return p;
}

template <typename T>
std::vector<T> apply_permutation(
    const std::vector<T>& vec,
    const std::vector<std::size_t>& p)
{
    std::vector<T> sorted_vec(p.size());
    for (int i=0;i<p.size();i++)
        sorted_vec[i]=vec[p[i]];
    return sorted_vec;
}
// --------------------------------

void Scope::Sort(Scope& scope,ScopeInstance& scopeInstance)
{
    vector<std::size_t> permutation=sort_permutation(scope);
    scope        =apply_permutation(scope        ,permutation);
    scopeInstance=apply_permutation(scopeInstance,permutation);
}

Scope& Scope::SortAndUnique()
{
    Sort();
    SDT::iterator prev=begin();
    for (SDT::iterator it=++begin();it!=end();)
    {
        if (*it==*prev)
            erase(it);
        else
        {
            prev=it;
            ++it;
        }
    }
    return *this;
}

std::string Scope::SoftPrint() const
{
    stringstream ss; 
    ss << (*this);
    return(ss.str());
}

ScopeInstance Scope::Instantiate(const std::vector<Index>& values) const
{
    ScopeInstance instance;
    for (Scope::const_iterator it=begin();it!=end();it++)
        instance.Insert(values[*it]);
    return instance;
}
