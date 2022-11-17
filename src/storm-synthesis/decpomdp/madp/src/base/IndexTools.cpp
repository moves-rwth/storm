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

#include "IndexTools.h"
#include "Scope.h"

using namespace std;

bool IndexTools::Increment(Index& index, size_t nrElems)
{
    index = (index+1) % nrElems;
    //carry_over:
    return(index == 0);
}

/**
 * takes 2 vectors of size vec_size:
 * indexVec gives for each vector index(i) an element index( indexVec[i] )
 *            e.g. < 2, 4, 0 >
 * nrElems gives for each  vector index(i) the number of elements
 *            e.g. < 3, 5, 2 >
 * Meaning that (in this example) the highest indexVec vector is
 *            < 2, 4, 1 >
 *
 * Staying with this example, Incrementing < 1, 3, 1> will yield < 1, 4, 0>
 *
 * returns a bool indicating carryover signal (true = on)
 */
bool IndexTools::Increment(vector<Index>& indexVec,
                           const vector<size_t>& nrElems )
{
    size_t vec_size = indexVec.size();
    if(nrElems.size() != indexVec.size()) 
        throw E("IndexTools::Increment - nrElems.size() != indexVec.size()");
    if(vec_size == 0)
    {
        return true;
        //return true is sufficient
        //throw E("IndexTools::Increment - vec_size == 0");
        
    }

    bool carry_over = true;
    Index i = vec_size - 1;
    while(carry_over /*&& i >= 0* - i is unsigned!*/ )
    {
        //start towards templatization:
        carry_over = Increment( indexVec[i] , nrElems[i] );
        //old code
        //indexVec[i] = (indexVec[i] + 1) % nrElems[i];
        //carry_over = (indexVec[i] == 0);
        if(i==0)
            break;//we just incremented the first element
        else
            i--;//proceed with previous elem (if carry_over, of course)
    }

    return(carry_over);

}

// ind -> joint

/**Calculate the joint index from individual indices i each taken from
 * sets with nrElems[i] elems (i.e., index i ranges from
 *      0...nrElems[i]-1
 *
 * Note: this only works if all joint indices occur, so you can use this
 * for joint actions, but not joint observation histories (this would 
 * assume there is a joint observation history index corresponding to
 * indiv. observation histories of different lengths).
 */ 
Index IndexTools::IndividualToJointIndices(const vector<Index>& indices,
        const vector<size_t>& nrElems) 
{
    size_t vec_size = nrElems.size();
    if(vec_size == 0)
        return 0;
    size_t* step_size=CalculateStepSize(nrElems);
    
    Index jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    delete [] step_size;

    return(jointI);

}
/**Calculate the joint index from individual indices i each taken from
 * sets with nrElems[i] elems (i.e., index i ranges from
 *      0...nrElems[i]-1
 *
 * Note: this only works if all joint indices occur, so you can use this
 * for joint actions, but not joint observation histories (this would 
 * assume there is a joint observation history index corresponding to
 * indiv. observation histories of different lengths).
 */ 
Index IndexTools::IndividualToJointIndices(const vector<Index>& indices,
        const vector<size_t>& nrElems, size_t n) 
{
    size_t vec_size = n; //let's assume compiler optimizes this away...(?)
    if(vec_size == 0)
        return 0;
    size_t* step_size=CalculateStepSize(nrElems, n);
    
    Index jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    delete [] step_size;

    return(jointI);

}
Index IndexTools::IndividualToJointIndicesArray(const Index* indices,
        const vector<size_t>& nrElems) 
{
    size_t vec_size = nrElems.size();
    size_t* step_size=CalculateStepSize(nrElems);

    Index jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    delete [] step_size;

    return(jointI);
}
Index IndexTools::IndividualToJointIndicesStepSize(const vector<Index>& indices,
        const vector<size_t>&  step_size) 
{
    size_t vec_size = indices.size();

    Index jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    return(jointI);
}

Index IndexTools::IndividualToJointIndicesStepSize(const vector<Index>& indices,
        const size_t * step_size) 
{
    size_t vec_size = indices.size();

    Index jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    return(jointI);
}
Index IndexTools::IndividualToJointIndicesArrayStepSize(const Index* indices,
        const size_t* step_size, size_t vec_size) 
{
    Index jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    return(jointI);
}

Index IndexTools::IndividualToJointIndicesArrayStepSize(const Index* indices,
        const vector<size_t> &step_size, size_t vec_size) 
{
    Index jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];
    return(jointI);
}

//joint->ind

vector<Index> IndexTools::JointToIndividualIndices(Index jointI,
       const vector<size_t>& nrElems) 
{
    size_t vec_size = nrElems.size();
    vector<Index> result(vec_size);
    JointToIndividualIndices(jointI, nrElems, result);
    return result;
}
void IndexTools::JointToIndividualIndices(Index jointI,
       const vector<size_t>& nrElems, vector<Index>& result ) 
{
    size_t* step_size=0;
    size_t vec_size = nrElems.size();
    if(result.size() != vec_size)
        result.resize(vec_size);

    if(vec_size > 0)
    {
        step_size=CalculateStepSize(nrElems);

        Index remainder = jointI;
        for(Index i=0; i < vec_size ; i++)
        {
            Index aI = remainder / step_size[i];
            result[i]= aI;//store this indiv. index
            remainder = remainder %  step_size[i];
        }
        delete [] step_size;
    }
}

vector<Index> IndexTools::JointToIndividualIndicesStepSize(Index jointI, 
            const size_t * step_size, size_t vec_size )
{
    Index remainder = jointI;

    Index resultArr[vec_size];
    for(Index i=0; i < vec_size ; i++)
    {
        Index aI = remainder / step_size[i];
        resultArr[i] = aI;//store this indiv. index
        //remainder = remainder %  step_size[i];
        remainder -= step_size[i] * aI;
    }
    vector<Index> result(&resultArr[0], &resultArr[vec_size]);
    return(result);
}

vector<Index> IndexTools::JointToIndividualIndicesStepSize(
        Index jointI, 
        const vector<size_t> &step_size, 
        size_t vec_size  
    )
{
    Index remainder = jointI;

    Index resultArr[vec_size];
    for(Index i=0; i < vec_size ; i++)
    {
        Index aI = remainder / step_size[i];
        resultArr[i] = aI;//store this indiv. index
        //remainder = remainder %  step_size[i];
        remainder -= step_size[i] * aI;
    }
    vector<Index> result(&resultArr[0], &resultArr[vec_size]);
    return(result);
}
std::vector<Index> IndexTools::JointToIndividualIndicesStepSize(
        Index jointI, 
        const std::vector<size_t> &stepSize
        )
{
    return JointToIndividualIndicesStepSize(
            jointI, stepSize, stepSize.size()
        );
}

const Index* IndexTools::JointToIndividualIndicesArrayStepSize(Index jointI, 
            const size_t * step_size, size_t vec_size )
{
    Index remainder = jointI;

    Index* resultArr = new Index[vec_size];
    for(Index i=0; i < vec_size ; i++)
    {
        Index aI = remainder / step_size[i];
        resultArr[i] = aI;//store this indiv. index
        //remainder = remainder %  step_size[i];
        remainder -= step_size[i] * aI;
    }
    return(resultArr);
}
size_t * IndexTools::CalculateStepSize(const vector<size_t>& nrElems)
{
    size_t vec_size = nrElems.size();
    //increment indicates for each agent how many the joint index is
    //incremented to get the next individual action...
    size_t *step_size = new size_t[vec_size];
    if (vec_size == 0)
        return(step_size);

    //the step_size for the last agent is 1
    step_size[vec_size-1] = 1;
    if(vec_size != 1) //i.e. vec_size >  1
    {
        Index i = vec_size-2;
        while(1)
        {
            if(i>0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                i--;
            }
            else if(i==0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                break;
            }
        }
    }
    return(step_size);
}
std::vector<size_t> IndexTools::CalculateStepSizeVector(const std::vector<size_t>& nrElems)
{
    size_t vec_size = nrElems.size();
    //increment indicates for each agent how many the joint index is
    //incremented to get the next individual action...
    vector<size_t> step_size(vec_size);
    if (vec_size == 0)
        return(step_size);
    //the step_size for the last agent is 1
    step_size[vec_size-1] = 1;
    if(vec_size != 1)
    {
        Index i = vec_size-2;
        while(1)
        {
            if(i>0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                i--;
            }
            else if(i==0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                break;
            }
        }
    }
    return(step_size);
}

size_t * IndexTools::CalculateStepSize(const vector<size_t>& nrElems, size_t n)
{
    size_t vec_size = n;
    //increment indicates for each agent how many the joint index is
    //incremented to get the next individual action...
    size_t *step_size = new size_t[vec_size];
    if (vec_size == 0)
        return(step_size);
    //the step_size for the last agent is 1
    step_size[vec_size-1] = 1;
    if(vec_size != 1)
    {
        Index i = vec_size-2;
        while(1)
        {
            if(i>0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                i--;
            }
            else if(i==0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                break;
            }
        }
    }
    return(step_size);
}

size_t IndexTools::CalculateNumberOfSequences(size_t o, size_t seqLength)
{
    // sequences have length seqLength, 
    // if the number of options(the branching factor) each time-step is o
    // then we get:
    //      sum_t=0...seqLength o^t == (o^(seqLength+1) - 1) / (o - 1)
    // or
    //      sum_t=0...h-1 o^t == (o^h - 1) / (o - 1)
    return( (size_t) 
        (
            ( pow((double)o,(double)(seqLength + 1)) - 1 )
            /   
            ( o - 1 )
        )
    );
}


// LIndex versions



bool IndexTools::Increment(LIndex& index, LIndex nrElems )
{
    index = (index+1) % nrElems;
    //carry_over:
    return(index == 0);
}

bool IndexTools::Increment(vector<LIndex>& indexVec, const vector<LIndex>& nrElems )
{
    if(nrElems.size() != indexVec.size()) 
        throw E("IndexTools::Increment - nrElems.size() != indexVec.size()");
    if(indexVec.size() == 0)
    {
        return true;
        //return true is sufficient
        //throw E("IndexTools::Increment - vec_size == 0");
        
    }

    bool carry_over = true;
    Index i = indexVec.size() - 1;
    while(carry_over /*&& i >= 0* - i is unsigned!*/ )
    {
        //start towards templatization:
        carry_over = Increment( indexVec[i] , nrElems[i] );
        //old code
        //indexVec[i] = (indexVec[i] + 1) % nrElems[i];
        //carry_over = (indexVec[i] == 0);
        if(i==0)
            break;//we just incremented the first element
        else
            i--;//proceed with previous elem (if carry_over, of course)
    }

    return(carry_over);

}
LIndex IndexTools::IndividualToJointIndices(const vector<LIndex>& indices,
        const vector<LIndex>& nrElems) 
{
    LIndex* step_size=CalculateStepSize(nrElems);
    size_t vec_size = nrElems.size();
    
    LIndex jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    delete [] step_size;

    return(jointI);

}
LIndex IndexTools::IndividualToJointIndicesArray(LIndex* indices,
        const vector<LIndex>& nrElems) 
{
    size_t vec_size = nrElems.size();
    LIndex* step_size=CalculateStepSize(nrElems);

    LIndex jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    delete [] step_size;

    return(jointI);
}
LIndex IndexTools::IndividualToJointIndicesStepSize(const vector<LIndex>& indices,
        const vector<LIndex>&  step_size) 
{
    size_t vec_size = indices.size();

    LIndex jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    return(jointI);
}

LIndex IndexTools::IndividualToJointIndicesStepSize(const vector<LIndex>& indices,
        const LIndex * step_size) 
{
    size_t vec_size = indices.size();

    LIndex jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    return(jointI);
}
LIndex IndexTools::IndividualToJointIndicesArrayStepSize(LIndex* indices,
        const LIndex* step_size, size_t vec_size) 
{
    LIndex jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];

    return(jointI);
}
LIndex IndexTools::IndividualToJointIndicesArrayStepSize(LIndex* indices,
        const vector<LIndex> &step_size, size_t vec_size) 
{
    LIndex jointI = 0;
    for(Index i=0; i < vec_size ; i++)            
        jointI += indices[i] * step_size[i];
    return(jointI);
}
vector<LIndex> IndexTools::JointToIndividualIndices(LIndex jointI,
       const vector<LIndex>& nrElems) 
{
    LIndex* step_size=0;
    Index vec_size = nrElems.size();
    vector<LIndex> result(vec_size);    
    if(vec_size > 0)
    {
        step_size=CalculateStepSize(nrElems);
        LIndex remainder = jointI;
        for(Index i=0; i < vec_size ; i++)
        {
            LIndex aI = remainder / step_size[i];
            result[i]= aI;//store this indiv. index
            remainder = remainder %  step_size[i];
        }
        delete [] step_size;
    }
    return(result);
}
vector<LIndex> IndexTools::JointToIndividualIndicesStepSize(LIndex jointI, 
            const LIndex * step_size, size_t vec_size )
{
    LIndex remainder = jointI;

    LIndex resultArr[vec_size];
    for(Index i=0; i < vec_size ; i++)
    {
        LIndex aI = remainder / step_size[i];
        resultArr[i] = aI;//store this indiv. index
        //remainder = remainder %  step_size[i];
        remainder -= step_size[i] * aI;
    }
    vector<LIndex> result(&resultArr[0], &resultArr[vec_size]);
    return(result);
}
vector<LIndex> IndexTools::JointToIndividualIndicesStepSize(LIndex jointI, 
            const vector<LIndex> &step_size, size_t vec_size )
{
    LIndex remainder = jointI;

    LIndex resultArr[vec_size];
    for(Index i=0; i < vec_size ; i++)
    {
        LIndex aI = remainder / step_size[i];
        resultArr[i] = aI;//store this indiv. index
        //remainder = remainder %  step_size[i];
        remainder -= step_size[i] * aI;
    }
    vector<LIndex> result(&resultArr[0], &resultArr[vec_size]);
    return(result);
}

std::vector<LIndex> IndexTools::JointToIndividualIndicesStepSize(LIndex jointI, 
            const std::vector<LIndex> &stepSize) 
{
    return JointToIndividualIndicesStepSize (jointI, stepSize, 
            stepSize.size());
}
LIndex * IndexTools::CalculateStepSize(const vector<LIndex>& nrElems)
{
    size_t vec_size = nrElems.size();
    //increment indicates for each agent how many the joint index is
    //incremented to get the next individual action...
    LIndex * step_size = new LIndex[vec_size];
    //the step_size for the last agent is 1
    step_size[vec_size-1] = 1;
    if(vec_size != 1)
    {
        Index i = vec_size-2;
        while(1)
        {
            if(i>0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                i--;
            }
            else if(i==0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                break;
            }
        }
    }
    return(step_size);
}
vector<LIndex> IndexTools::CalculateStepSizeVector(const vector<LIndex>& nrElems)
{
    size_t vec_size = nrElems.size();
    //increment indicates for each agent how many the joint index is
    //incremented to get the next individual action...
    vector<LIndex> step_size(vec_size);
    //the step_size for the last agent is 1
    step_size[vec_size-1] = 1;
    if(vec_size != 1)
    {
        Index i = vec_size-2;
        while(1)
        {
            if(i>0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                i--;
            }
            else if(i==0)
            {
                step_size[i] = nrElems[i+1] * step_size[i+1];
                break;
            }
        }
    }
    return(step_size);
}

Index 
IndexTools::ActionAndObservation_to_ActionObservationIndex(Index aI,
        Index oI, size_t nrA, size_t nrO) 
{ 
    return ( aI * nrO + oI ); 
}  
Index
IndexTools::ActionObservation_to_ActionIndex(Index aoI, size_t nrA, size_t nrO) 
{
    return ( (size_t) (aoI / nrO) ); 
}
Index 
IndexTools::ActionObservation_to_ObservationIndex(Index aoI, size_t nrA, size_t nrO)
{
    return ( (size_t) (aoI % nrO) );
}







