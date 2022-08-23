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
#ifndef _MULTIAGENTDECISIONPROCESSDISCRETEINTERFACE_H_
#define _MULTIAGENTDECISIONPROCESSDISCRETEINTERFACE_H_ 1

/* the include directives */
#include "Globals.h"
#include "MultiAgentDecisionProcessInterface.h"

class Action;
class Observation;
class JointObservation;
class State;
class JointAction;
class TransitionModelMapping;
class TransitionModelMappingSparse;
class ObservationModelMapping;
class ObservationModelMappingSparse;
class EventObservationModelMapping;
class EventObservationModelMappingSparse;

class TransitionModelDiscrete; 
class ObservationModelDiscrete;

class TGet;
class OGet;
class StateDistribution;

class Scope;

/**\brief MultiAgentDecisionProcessDiscreteInterface is an abstract base
 * class that defines publicly accessible member functions that a
 * discrete multiagent decision process must implement.
 *
 * This interface is currently implemented by MultiAgentDecisionProcessDiscrete
 * and MultiAgentDecisionProcessDiscreteFactoredStates. 
 *
 * The functions this interface defines relate to actions, observations,
 * transition and observation probabilities. *
 *
 **/
class MultiAgentDecisionProcessDiscreteInterface : 
    virtual public MultiAgentDecisionProcessInterface
{
    private:    

    protected:    

    public:

        ///Destructor.
        virtual ~MultiAgentDecisionProcessDiscreteInterface() {};

        // 'get' functions:     

        ///Return the number of states.
        virtual size_t GetNrStates() const = 0;
        ///Returns a pointer to state i.
        virtual const State* GetState(Index i) const = 0;
        virtual std::string SoftPrintState(Index sI) const = 0;

        /////Return the initial state distribution.
        virtual double GetInitialStateProbability(Index sI) const = 0;
        /// Returns the complete initial state distribution.
        virtual StateDistribution* GetISD() const = 0;


        
        ///Return the number of actions vector    
        virtual const std::vector<size_t>& GetNrActions() const = 0;
        ///Return the number of actions of agent agentI    
        virtual size_t GetNrActions(Index AgentI) const =0 ;
        ///Return the number of joint actions.
        virtual size_t GetNrJointActions() const = 0;
        ///Get the number of joint actions the agents in agScope can form
        virtual size_t GetNrJointActions(const Scope& agScope) const = 0;

        ///Return a ref to the a-th action of agent agentI.
        virtual const Action* GetAction(Index agentI, Index a) const = 0;
        ///Return a ref to the i-th joint action.
        virtual const JointAction* GetJointAction(Index i) const = 0;
        /**\brief Returns the joint action index that corresponds to the array
         * of specified individual action indices.*/
        virtual Index IndividualToJointActionIndices(const Index* AI_ar)
            const = 0;
        /**\brief Returns the joint action index that corresponds to the vector
         * of specified individual action indices.*/
        virtual Index IndividualToJointActionIndices(const std::vector<Index>& 
                indivActionIndices) const = 0;
        /**\brief Returns a vector of indices to indiv. action indicies corr.
         * to joint action index jaI.*/
        virtual const std::vector<Index>& JointToIndividualActionIndices(
                Index jaI) const = 0;
        
        ///indiv->joint for a restricted set (Scope) of agents        
        virtual Index IndividualToJointActionIndices(
            const std::vector<Index>& ja_e, const Scope& agSC) const = 0;
        ///joint->indiv for a restricted set (Scope) of agents        
        virtual std::vector<Index> JointToIndividualActionIndices(
            Index ja_e, const Scope& agSC) const = 0;
        /**\brief Converts a global joint action index jaI to a restricted
         * joint action index ja_e, for agents scope agSc_e
         * Returns a vector of indices to indiv. action indicies corr.
         * to joint action index jaI.*/
        virtual Index JointToRestrictedJointActionIndex(
                Index jaI, const Scope& agSc_e ) const = 0;

        ///Return the number of observations vector.    
        virtual const std::vector<size_t>& GetNrObservations() const = 0;
        ///Return the number of observations of agent agentI    
        virtual size_t GetNrObservations(Index AgentI) const = 0;
        ///Return the number of joint observations.
        virtual size_t GetNrJointObservations() const = 0; 

        ///Return a ref to the a-th observation of agent agentI.
        virtual const Observation* GetObservation(Index agentI, Index a) 
            const = 0; 
        ///Return a ref to the i-th joint observation.
        virtual const JointObservation* GetJointObservation(Index i) const = 0;

        /**\brief Returns the joint observation index that corresponds to the
         * vector of specified individual observation indices.*/
        virtual Index IndividualToJointObservationIndices(
                const std::vector<Index>& 
                indivObservationIndices) const = 0;
        /**\brief Returns a vector of indices to indiv. observation indicies
         * corr. to joint observation index joI.*/
        virtual const std::vector<Index>& 
            JointToIndividualObservationIndices(Index joI) const = 0;

        ///indiv->joint for a restricted set (Scope) of agents        
        virtual Index IndividualToJointObservationIndices(
            const std::vector<Index>& jo_e, const Scope& agSC) const
        { throw E("MultiAgentDecisionProcessDiscreteInterface function not implemented (by TOI model?) "); };
           // = 0;
        ///joint->indiv for a restricted set (Scope) of agents        
        virtual std::vector<Index> JointToIndividualObservationIndices(
            Index jo_e, const Scope& agSC) const 
        { throw E("MultiAgentDecisionProcessDiscreteInterface function not implemented (by TOI model?) "); };
            //= 0;
        /**\brief Converts a global joint observation index joI to a restricted
         * joint observation index jo_e, for agents scope agSc_e
         * Returns a vector of indices to indiv. observation indicies corr.
         * to joint observation index joI.*/
        virtual Index JointToRestrictedJointObservationIndex(
                Index joI, const Scope& agSc_e ) const
        { throw E("MultiAgentDecisionProcessDiscreteInterface function not implemented (by TOI model?) "); };
        //    = 0;

        ///Return the probability of successor state sucSI: P(sucSI|sI,jaI).
        virtual double GetTransitionProbability(Index sI, Index jaI, Index 
                sucSI) const = 0;
        virtual TGet* GetTGet() const = 0;

        ///Return the probability of joint observation joI: P(joI|jaI,sucSI).
        virtual double GetObservationProbability(Index jaI, Index sucSI, 
                Index joI) const = 0;
        virtual double GetObservationProbability(Index sI, Index jaI, Index sucSI, 
                Index joI) const
        {return GetObservationProbability(jaI, sucSI, joI); }
        virtual OGet* GetOGet() const = 0;
        

        /**\brief Returns a pointer to the underlying transition model.
         *
         * If speed is required (for instance when looping through all states)
         * the pointer can be requested by an algorithm. It can than obtain
         * a pointer to the actual implementation type by runtime type 
         * identification. (i.e., using typeid and dynamic_cast).
         */
        virtual const TransitionModelDiscrete* GetTransitionModelDiscretePtr() 
            const = 0;

        /**\brief Returns a pointer to the underlying observation model.
         *
         * If speed is required (for instance when looping through all states)
         * the pointer can be requested by an algorithm. It can than obtain
         * a pointer to the actual implementation type by runtime type 
         * identification. (i.e., using typeid and dynamic_cast).
         */
        virtual const ObservationModelDiscrete* GetObservationModelDiscretePtr()
            const = 0;

        /**\brief Whether observation models are P(o|s,a) or P(o|s,a,s').
         * This is here since model-independent structures(such as joint beliefs)
         * need this information. It should be overriden by derived classes.
         * */
        virtual bool GetEventObservability() const
        {return(false);}

        //sample functions:

        /// Sample a successor state - needed by simulations.
        virtual Index SampleSuccessorState(Index sI, Index jaI) const = 0;
        /// Sample an observation - needed for simulations.
        virtual Index SampleJointObservation(Index jaI, Index sucI) const =0;
        virtual Index SampleJointObservation(Index sI, Index jaI, Index sucI) const
        { return SampleJointObservation(jaI, sucI); }
        /// Sample a state according to the initial state PDF.
        virtual Index SampleInitialState() const = 0;

        /// Returns a pointer to a copy of this class.
        virtual MultiAgentDecisionProcessDiscreteInterface* Clone() const = 0;

        /**\brief Prints some information on the 
         * MultiAgentDecisionProcessDiscreteInterface.
         * */ 
        virtual std::string SoftPrint() const = 0;

};

#endif /* !_MULTIAGENTDECISIONPROCESSINTERFACE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
