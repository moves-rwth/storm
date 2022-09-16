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
#ifndef _MADPCOMPONENTDISCRETEACTIONS_H_
#define _MADPCOMPONENTDISCRETEACTIONS_H_ 1


/* the include directives */
#include <iostream>
#include <sstream>
#include "Globals.h"
#include "ActionDiscrete.h"
#include "JointActionDiscrete.h"
#include "IndexTools.h"
#include "EOverflow.h"

#include <map>


namespace {
    typedef std::vector<ActionDiscrete> ActionDVec;
}

/** \brief MADPComponentDiscreteActions contains functionality for
 * discrete action spaces.
 *
 *  It implements a part of the
 *  MultiAgentDecisionProcessDiscreteInterface. */
class MADPComponentDiscreteActions
{
    private:
    
        bool _m_initialized;    
        bool _m_cachedAllJointActions;
        bool _m_jointIndicesValid;
        size_t _m_nrJointActions;

        /// The stepsize array - used for indiv->joint index calculation.
        size_t * _m_actionStepSize;

        ///The vector storing pointers to joint actions.
        /** To use this, ConstructJointActions() should be called */
        std::vector<JointActionDiscrete*> _m_jointActionVec;
    
        /// When not all joint actions have been created, here we cache
        /// the individual indices created by
        /// JointToIndividualActionIndices()
        std::map<Index, std::vector<Index> *> *_m_jointActionIndices;

        /// Recursively creates the joint actions.
        size_t ConstructJointActionsRecursively( Index curAgentI, 
            JointActionDiscrete& ja, Index jaI);

        std::string SoftPrintActionSets() const;
        std::string SoftPrintJointActionSet() const;

        /// The number of actions for each agent.
        std::vector<size_t> _m_nrActions;
        /// The vectors of actions (vectors of ActionDiscrete) for each agent.
        std::vector<std::vector<ActionDiscrete> > _m_actionVecs;    

    protected:
    public:

        /// Default constructor.
        MADPComponentDiscreteActions();

        /// Copy constructor.
        MADPComponentDiscreteActions(const MADPComponentDiscreteActions& a);

        /// Destructor.
        virtual ~MADPComponentDiscreteActions();

        //data manipulation (set) functions:

        /// Sets _m_initialized to b.
        bool SetInitialized(bool b);

        /// Sets the number of actions for agent AI.
        void SetNrActions(Index AI, size_t nrA);

        /// Add a new action with name "name" to the actions of agent AI.
        void AddAction(Index AI, const std::string &name,
                       const std::string &description="");

        /// Recursively constructs all the joint actions.
        size_t ConstructJointActions();

        //get (data) functions:    

        /// Return the number of actions vector.
        const std::vector<size_t>& GetNrActions() const
        {return _m_nrActions;}

        /// Return the number of actions of agent agentI.    
        size_t GetNrActions(Index AgentI) const;

        /// Return the number of joiny actions.
        size_t GetNrJointActions() const;
        ///Get the number of joint actions the agents in agScope can form
        size_t GetNrJointActions(const Scope& agScope) const;

        /// Find out if there is an overflow in the joint indices variable.
        bool JointIndicesValid() const
            {return _m_jointIndicesValid;}

        /// Returns the action index of the agent I's action s.
        Index GetActionIndexByName(const std::string &s, Index agentI) const;

        /// Returns the name of a particular action a of agent i.
        std::string GetActionName(Index a, Index i) const {
            return(_m_actionVecs.at(i).at(a).GetName()); }

        /// Returns the name of a particular joint action a.
        std::string GetJointActionName(Index a) const {
            return(_m_jointActionVec.at(a)->SoftPrint()); } 

        /// Return a ref to the a-th action of agent agentI.
        const Action* GetAction(Index agentI, Index a) const;

        /// Return a ref to the a-th action of agent agentI.
        const ActionDiscrete* GetActionDiscrete(Index agentI, Index a) const;

        /// Return a ref to the i-th joint action.
        const JointAction* GetJointAction(Index i) const;

        /// Return a ref to the i-th joint action (a JointActionDiscrete).
        const JointActionDiscrete* GetJointActionDiscrete(Index i) const;

        /** \brief Returns the joint action index that corresponds to
         * the vector of specified individual action indices.*/
        Index IndividualToJointActionIndices(const std::vector<Index>& 
                indivActionIndices) const;

        /** \brief Returns the joint action index that corresponds to
         * the array of specified individual action indices.*/
        Index IndividualToJointActionIndices(const Index* IndexArray) const
            {return IndexTools::IndividualToJointIndicesArrayStepSize(
                 IndexArray, _m_actionStepSize, _m_nrActions.size());}

        /** \brief Returns a vector of indices to indiv. action
         * indicies corr. to joint action index jaI.*/
        const std::vector<Index>& JointToIndividualActionIndices(Index jaI)const
        {
            if(!_m_jointIndicesValid)
            {
                throw(EOverflow("MADPComponentDiscreteActions::JointToIndividualActionIndices() joint indices are not available, overflow detected"));
            }
            if(_m_cachedAllJointActions)
                return GetJointActionDiscrete(jaI)->
                    GetIndividualActionDiscretesIndices();
            else if(_m_jointActionIndices->find(jaI)!=
                    _m_jointActionIndices->end())
                return(*_m_jointActionIndices->find(jaI)->second);
            else // create new 
            {
                std::vector<Index> *indices=new std::vector<Index>();
                *indices=IndexTools::JointToIndividualIndices(jaI,
                                                              GetNrActions());
                _m_jointActionIndices->insert(make_pair(jaI,indices));
                return(*indices);
            }
        }
        
        Index IndividualToJointActionIndices(
                const std::vector<Index>& ja_e, const Scope& agSC) const;
        std::vector<Index> JointToIndividualActionIndices(
                Index ja_e, const Scope& agSC) const;
        Index JointToRestrictedJointActionIndex(
                Index jaI, const Scope& agSc_e ) const;

        std::string SoftPrint() const;
        void Print() const
        {std::cout << MADPComponentDiscreteActions::SoftPrint();}
};


#endif /* !_MADPCOMPONENTDISCRETEACTIONS_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
