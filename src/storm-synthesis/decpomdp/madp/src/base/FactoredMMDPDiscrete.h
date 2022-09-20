/* This file is part of the Multiagent Decision Process (MADP) Toolbox. 
 *
 * The majority of MADP is free software released under GNUP GPL v.3. However,
 * some of the included libraries are released under a different license. For 
 * more information, see the included COPYING file. For other information, 
 * please refer to the included README file.
 *
 * This file has been written and/or modified by the following people:
 *
 * Philipp Robbel 
 *
 * For contact information please see the included AUTHORS file.
 */

/* Only include this header file once. */
#ifndef _FACTOREDMMDPDISCRETE_H_
#define _FACTOREDMMDPDISCRETE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "FactoredDecPOMDPDiscrete.h"

class FactoredMMDPDiscrete : public FactoredDecPOMDPDiscrete
{
public:
    /**\brief Default constructor.
     * Constructor that sets the name, description, and problem file,
     * and subsequently loads this problem file. */ 
FactoredMMDPDiscrete(
    std::string name="received unspec. by FactoredMMDPDiscrete", 
    std::string descr="received unspec. by FactoredMMDPDiscrete", 
    std::string pf="received unspec. by FactoredMMDPDiscrete")
        : FactoredDecPOMDPDiscrete(name, descr, pf)
    {
        //SetFlatObservationModel(new ObservationModelIdentityMapping());
        
        //MADPComponentDiscreteObservations::SetInitialized(true) can
        //overflow _m_nrJointObservations (indicated by _m_jointIndicesValid).
        //(likely)

        //Problem occurs if single observation variable's stepsize
        //calculations already overflow (happens in 2DBN::InitializeIIs()).
        //(unlikely)
    }

    virtual std::string SoftPrint() const;

    /// Returns a pointer to a copy of this class.
    virtual FactoredMMDPDiscrete* Clone() const
    { return new FactoredMMDPDiscrete(*this); }

    void CacheFlatModels(bool sparse);

    void Initialize2DBN();

    // Called in MultiAgentDecisionProcessDiscreteFactoredStates::Initialize2DBN CPT initialization but not utilized in FactoredMMDPDiscrete.
    // Intentionally breaks inheritance chain (no fully-observable subclass
    // can re-implement this and hope to get this executed)
    double ComputeObservationProb(
        Index o,
        Index oVal,
        const std::vector< Index>& As,
        const std::vector< Index>& Ys,
        const std::vector< Index>& Os ) const
    {
        // could first check whether GetYSoI_O(o) is full state space scope
        return oVal == IndexTools::IndividualToJointIndices(Ys, GetNrValuesPerFactor()) ? 1.0 : 0.0;
    }
  
    // Intentionally breaks inheritance chain
    void SetScopes() //reimplemented from base class
    {SetYScopes(); SetOScopes();}
    void SetOScopes();
        
protected:  

private:
    /**\brief Check whether models appear valid probability 
     * distributions.  
     *
     * Different from the version in the parent class, observations 
     * are not exhaustively checked here.*/
    bool SanityCheckObservations() const
    {return true;}

    /// Construct all the observations for fully-observable case.
    void ConstructObservations();
    void Initialize2DBNObservations();

};

#endif
