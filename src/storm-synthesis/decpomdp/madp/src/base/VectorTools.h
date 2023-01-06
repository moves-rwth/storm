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

/* Only include this header file once. */
#ifndef _VECTORTOOLS_H_
#define _VECTORTOOLS_H_ 1

/* the include directives */
#include "Globals.h"
#include <algorithm>

namespace VectorTools{

    template <typename T>
    bool Equal( const std::vector<T>& vec1,  const std::vector<T>& vec2 )
    {
        size_t s1 = vec1.size();
        if( s1 != vec2.size())
            return false;

        return( std::equal(vec1.begin(), vec1.end(), vec2.begin() ) );
/*
        typename std::vector< T >::const_iterator it1, it2;
        it1 = vec1.begin();
        it2 = vec2.begin();
        while(it1 != vec1.end() )
        {
            if( (*it1) != (*it2) )
            it1++;
            it2++;
        }
        */
        
    }

    template <typename T>
    T InnerProduct( const std::vector<T>& vec1,  const std::vector<T>& vec2 )
    {
        size_t s1 = vec1.size();
        if( s1 != vec2.size())
            throw E("VectorTools::InnerProduct - vec sizes not equal");

        typename std::vector< T >::const_iterator it1, it2;
        T inprod=0;
        it1 = vec1.begin();
        it2 = vec2.begin();
        while(it1 != vec1.end() )
        {
            inprod += (*it1) * (*it2) ;
            it1++;
            it2++;
        }
        return(inprod);
    }    
    
    ///Compute the product of the vector's elements
    /**
     * clearly the product operator* should be defined for type T
     */        
    template <typename T>
    T VectorProduct( const std::vector<T>& vec )
    {
        if(vec.size() == 0)
            throw E("IndexTools::VectorProduct - vector product of vec of size 0 is undefined!");

        T product = *(vec.begin());//the first element
        typename std::vector< T >::const_iterator it;
        it = vec.begin() + 1;//second element
        while(it != vec.end() )
        {
            product = product * (*it);
            it++;
        }
        return(product); 
    }

    template <typename T>
    T MaxNorm(  const std::vector<T>& vec )
    {
        T norm = 0.0 ;
        for (Index i = 0 ; i < vec.size() ; i++)
            norm = std::max (fabs (vec [i]), norm) ;
        return norm;
    }

}

#endif /* !_VECTORTOOLS_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
