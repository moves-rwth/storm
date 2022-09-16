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
#ifndef _TWOSTAGEDYNAMICBAYESIANNETWORK_H_
#define _TWOSTAGEDYNAMICBAYESIANNETWORK_H_ 1

/* the include directives */
#include "Globals.h"
#include "MultiAgentDecisionProcessDiscreteFactoredStatesInterface.h"
#include "CPDDiscreteInterface.h"

/** \brief TwoStageDynamicBayesianNetwork (2DBN) is a class that represents 
 * the transition and observation model for a factored MADP
 *
 * (i.e., for something that implements the 
 * MultiAgentDecisionProcessDiscreteFactoredStatesInterface. E.g., a 
 * MultiAgentDecisionProcessDiscreteFactoredStates)
 *
 * Components\n
 * ----------
 * A TwoStageDynamicBayesianNetwork is composed of the 
 * \li previous-stage (PS) state factors (SF) - x
 * \li next-stage (NS) state factors - y
 * \li actions variables - a
 * \li observation variables - o
 *
 * Connections\n
 * -----------
 * There are causal connections of the following forms
 * \li x -> y   influence of a PS SF on a NS SF
 * \li a -> y   influence of an action on a NS SF
 * \li a -> o   influence of an action on an observation
 * \li y -> y   influence of a NS SF (with a lower index) on a NS SF (with a 
 *              higher index)
 * \li y -> o   influence of a NS SF on an observation
 * \li o -> o   influence of a observation ((of an agent) with a lower index)
 *              on an observation ((of an agent) with a higher index).
 * 
 * Scopes of influence\n
 * -------------------\n
 * As such each y has a Scope of influence (SoI): SoI(y) = subset{x,a,y}
 * And similar for each o: SoI(o) = subset{a,y,o}
 *
 * Influence Instantiations (II)\n
 * -----------------------------\n
 * We refer to 1 instantiation of all variables in SoI(y) as an
 * 'influence instantiation'  (II). This class provides some convenience 
 * functions for converting a joint II index (denoted iiI), from vectors
 * describing an instantiation, and vice versa.
 *
 * We discriminate Yii's and Oii's for resp. NS SFs and Observations.
 *
 * CPFs\n
 * ----\n
 * For each y and o, the 2DBN maintains a conditional probability distr. (CPD).
 * These functions maintain the actual probabilities
 * \li CPD(y) = Pr(y | SoI(y) )
 * \li CPD(o) = Pr(o | SoI(o) )
 *
 * Computing joint probabilities\n
 * -----------------------------\n
 * Joint probabilities can now be computed as illustrated here:
 * \li Pr( y1, y2 | SoI(y1), SoI(y2) ) = CPD(y1,y2) = CPD(y1) * CPD(y2) = 
 *      Pr(y1 | SoI(y1)) * Pr(y2 | SoI(y2))
 * \li similar for observations and combinations of o and y.
 *
 * In the future we may want to use compact CPDs such as decision trees, ADDs
 * or rules based descriptions. Then we can also consider concretizing
 * the product  CPD(y1,y2) = CPD(y1) * CPD(y2) (e.g., overloading the operator* 
 * for CPDs etc...)
 *
 * 
 *
 **/
class TwoStageDynamicBayesianNetwork 
{
    private:    
        /**\brief pointer to a MultiAgentDecisionProcessFactoredStatesInterface
         *
         * A TwoStageDynamicBayesianNetwork is not a standalone thing. It is 
         * used to represent a MADP with a factored states space.
         */
        MultiAgentDecisionProcessDiscreteFactoredStatesInterface* _m_madp;

    //representing the nodes of the network
        ///the number of state factors
        size_t _m_nrY;
        ///the number of observation factors (i.e., the number of agents)
        size_t _m_nrO;

    //representing the connections
        bool _m_SoIStorageInitialized;
        /// For each NS SF we maintain the PS SFs, X, in its SoI
        std::vector< Scope > _m_XSoI_Y;
        /// For each NS SF we maintain the actions, A, in its SoI
        std::vector< Scope > _m_ASoI_Y;
        /// For each NS SF we maintain the NS SFs, Y, in its SoI
        std::vector< Scope > _m_YSoI_Y;

        /// For each O we maintain the PS SFs, X, in its SoI
        std::vector< Scope > _m_XSoI_O;
        /// For each O we maintain the actions, A, in its SoI
        std::vector< Scope > _m_ASoI_O;
        /// For each O we maintain the NS SFs, Y, in its SoI
        std::vector< Scope > _m_YSoI_O;
        /// For each O we maintain the observations, O, in its SoI
        std::vector< Scope > _m_OSoI_O;
       
        ///Boolean that is set when all ii info is initialized
        bool _m_ii_initialized; 
        ///For each NS SF Y we maintain the number of SF values in XSoI(Y)
        /**I.e., For each SF in XSoI(Y) we maintain the number of values it
         * has.
         * This can be used to convert 'local' X indices to joint ones, etc.
         */
        std::vector< std::vector<size_t> > _m_nrVals_XSoI_Y;
        ///For each NS SF Y we maintain the number of actions in ASoI(Y)
        std::vector< std::vector<size_t> > _m_nrVals_ASoI_Y;
        ///For each NS SF Y we maintain the number of SF values in YSoI(Y)
        std::vector< std::vector<size_t> > _m_nrVals_YSoI_Y;
        ///For each NS SF Y we maintain the number of SF values in SoI(Y)
        /**This is the concatination of [nrValsXSoI, nrValsASoI, nrValsYSoI]
         * used by IndividualToJointYiiIndices, and JointToIndividualYiiIndices
         */
        std::vector< std::vector<size_t> > _m_nrVals_SoI_Y;


        ///For each Y we maintain the number of instantiations of XSoI(Y)
        std::vector<size_t> _m_Xii_size_Y;
        ///For each Y we maintain the number of instantiations of ASoI(Y)
        std::vector<size_t> _m_Aii_size_Y;
        ///For each Y we maintain the number of instantiations of YSoI(Y)
        std::vector<size_t> _m_Yii_size_Y;
        ///For each y we maintain the number of II's
        std::vector<size_t> _m_ii_size_Y;

        ///For each O we maintain the number of SF values in XSoI(O)
        std::vector< std::vector<size_t> > _m_nrVals_XSoI_O;
        ///For each O we maintain the number of actions in ASoI(O)
        std::vector< std::vector<size_t> > _m_nrVals_ASoI_O;
        ///For each O we maintain the number of SF values in YSoI(O)
        std::vector< std::vector<size_t> > _m_nrVals_YSoI_O;
        ///For each O we maintain the number of observations in OSoI(O)
        std::vector< std::vector<size_t> > _m_nrVals_OSoI_O;
        ///For each NS SF O we maintain the number of SF values in SoI(O)
        /**This is the concatination of [nrValsASoI, nrValsYSoI, nrValsOSoI]
         * used by IndividualToJointOiiIndices, and JointToIndividualOiiIndices
         */
        std::vector< std::vector<size_t> > _m_nrVals_SoI_O;

        ///For each O we maintain the number of instantiations of XSoI(O)
        std::vector<size_t> _m_Xii_size_O;
        ///For each O we maintain the number of instantiations of ASoI(O)
        std::vector<size_t> _m_Aii_size_O;
        ///For each O we maintain the number of instantiations of YSoI(O)
        std::vector<size_t> _m_Yii_size_O;
        ///For each O we maintain the number of instantiations of OSoI(O)
        std::vector<size_t> _m_Oii_size_O;
        ///For each y we maintain the number of II's
        std::vector<size_t> _m_ii_size_O;

        //representing the CPDs
        /// For each next-stage state variable y we maintain a CPD
        std::vector< CPDDiscreteInterface * > _m_Y_CPDs;
        /// For each observation we maintain a CPD
        std::vector< CPDDiscreteInterface * > _m_O_CPDs;

        ///Computes the 'closure' of NS variables Y and O.
        void ComputeWithinNextStageClosure(Scope& Y, Scope& O) const;

        /// Temporary storage used in IndividualToJointYiiIndices.
        std::vector<Index> *_m_IndividualToJointYiiIndices_catVector;
        /// Cache the step size for speed.
        std::vector<size_t*> _m_nrVals_SoI_Y_stepsize;
        /// Temporary storage used in IndividualToJointOiiIndices.
        std::vector<Index> *_m_IndividualToJointOiiIndices_catVector;
        /// Cache the step size for speed.
        std::vector<size_t*> _m_nrVals_SoI_O_stepsize;
        /// Temporary storage used in SampleY.
        std::vector<Index> *_m_SampleY;
        std::vector<std::vector<Index>* > _m_X_restr_perY;
        std::vector<std::vector<Index>* > _m_A_restr_perY;
        std::vector<std::vector<Index>* > _m_Y_restr_perY;

        /// Temporary storage used in SampleO.
        std::vector<Index> *_m_SampleO;
        std::vector<std::vector<Index>* > _m_X_restr_perO;
        std::vector<std::vector<Index>* > _m_A_restr_perO;
        std::vector<std::vector<Index>* > _m_Y_restr_perO;
        std::vector<std::vector<Index>* > _m_O_restr_perO;
        /// Temporary storage used in SampleO.
        std::vector<size_t> *_m_SampleNrO;

    protected:
    
    public:
        // Constructor, destructor and copy assignment.
        /// (default) Constructor
        TwoStageDynamicBayesianNetwork(
                MultiAgentDecisionProcessDiscreteFactoredStatesInterface& madp);
        /// Copy constructor.
        //TwoStageDynamicBayesianNetwork(const TwoStageDynamicBayesianNetwork& a);
        /// Destructor.
        ~TwoStageDynamicBayesianNetwork();
        /// Copy assignment operator
        //TwoStageDynamicBayesianNetwork& operator= (const TwoStageDynamicBayesianNetwork& o);

        //operators:
        
    //SoI functions
        ///Sets the SoI vectors to appropriate size and containing empty scopes.
        /**this function should be called after adding all state factors, 
         * but before trying to add the connections (by setting the SoI's)
         */
        void InitializeStorage();

        ///Sets the SoI of a NS SF y
        void SetSoI_Y( Index y, 
                const Scope& XSoI, 
                const Scope& ASoI, 
                const Scope& YSoI);

        ///Sets the SoI of an observation o
        void SetSoI_O( Index o, 
                const Scope& ASoI, 
                const Scope& YSoI, 
                const Scope& OSoI);

        ///Sets the SoI of an observation o
        void SetSoI_O( Index o,
                const Scope& XSoI, 
                const Scope& ASoI, 
                const Scope& YSoI, 
                const Scope& OSoI);

        const Scope& GetXSoI_Y(Index y) const
        { return _m_XSoI_Y[y]; }
        const Scope& GetASoI_Y(Index y) const
        { return _m_ASoI_Y[y]; }
        const Scope& GetYSoI_Y(Index y) const
        { return _m_YSoI_Y[y]; }

        const Scope& GetXSoI_O(Index o) const
        { return _m_XSoI_O[o]; }        
        const Scope& GetASoI_O(Index o) const
        { return _m_ASoI_O[o]; }
        const Scope& GetYSoI_O(Index o) const
        { return _m_YSoI_O[o]; }
        const Scope& GetOSoI_O(Index o) const
        { return _m_OSoI_O[o]; }

    //influence instantiation (II) functions
        
        ///Computes some administrative things necessary for II functions
        /**This should be called after all SoI's have been defined.
         * When not using any II functions, this may be skipped?! (I think)
         */
        void InitializeIIs();
        
        ///returns the number of values for all state factors in XSoI(y)
        const std::vector<size_t>& GetNrVals_XSoI_Y(Index yI) const;
        ///returns the number of values for all actions in XSoI(y)
        const std::vector<size_t>& GetNrVals_ASoI_Y(Index yI) const;
        ///returns the number of values for all state factors in YSoI(y)
        const std::vector<size_t>& GetNrVals_YSoI_Y(Index yI) const;

        ///returns the nr. instantiations of XSoI(y)
        size_t GetXiiSize_Y(Index yI) const;
        ///returns the nr instantiations of ASoI(y)
        size_t GetAiiSize_Y(Index yI) const;
        ///returns the nr instantiations of YSoI(y)
        size_t GetYiiSize_Y(Index yI) const;
        ///returns the total number of IIs
        size_t GetiiSize_Y(Index yI) const;

        ///returns the number of values for all state factors in XSoI(o)
        const std::vector<size_t>& GetNrVals_XSoI_O(Index oI) const;
        ///returns the number of values for all actions in ASoI(o)
        const std::vector<size_t>& GetNrVals_ASoI_O(Index oI) const;
        ///returns the number of values for all state factors in YSoI(o)
        const std::vector<size_t>& GetNrVals_YSoI_O(Index oI) const;
        ///returns the number of values for all state factors in OSoI(o)
        const std::vector<size_t>& GetNrVals_OSoI_O(Index oI) const;

        ///returns the nr instantiations of XSoI(o)
        size_t GetXiiSize_O(Index oI) const;
        ///returns the nr instantiations of ASoI(o)
        size_t GetAiiSize_O(Index oI) const;
        ///returns the nr instantiations of YSoI(o)
        size_t GetYiiSize_O(Index oI) const;
        ///returns the nr instantiations of OSoI(o)
        size_t GetOiiSize_O(Index oI) const;
        ///returns the total number of IIs
        size_t GetiiSize_O(Index oI) const;


        
        ///Computes Xs, As, Ys from the joint iiI denoting a II of SoI(y).
        /**This returns restricted std::vectors (std::vectors restricted to the SoI(y))
         */
        void JointToIndividualYiiIndices(Index y, Index iiI, 
                std::vector<Index>& X_rest, 
                std::vector<Index>& A_rest, 
                std::vector<Index>& Y_rest) const;
        ///Computes the joint II index for restricted std::vectors Xs,As,Ys.
        /**std::vectors are restricted to only include values in SoI(y)
         */
        Index IndividualToJointYiiIndices(Index y, 
                const std::vector<Index>& X_rest, 
                const std::vector<Index>& A_rest, 
                const std::vector<Index>& Y_rest) const;
        ///Computes Xs, As, Ys from the joint iiI denoting a II of SoI(o).
        /**This returns restricted std::vectors (std::vectors restricted to the SoI(o))
         */
        void JointToIndividualOiiIndices(Index o, Index iiI, 
                std::vector<Index>& A_rest, 
                std::vector<Index>& Y_rest, 
                std::vector<Index>& O_rest) const;
        void JointToIndividualOiiIndices(Index o, Index iiI, 
                std::vector<Index>& X_rest, 
                std::vector<Index>& A_rest, 
                std::vector<Index>& Y_rest, 
                std::vector<Index>& O_rest) const;
        ///Computes the joint II index for restricted std::vectors As,Ys,Os
        /**std::vectors are restricted to only include values in SoI(o)
         */
        Index IndividualToJointOiiIndices(Index o, 
                const std::vector<Index>& A_rest, 
                const std::vector<Index>& Y_rest, 
                const std::vector<Index>& O_rest) const;
        Index IndividualToJointOiiIndices(Index o, 
                const std::vector<Index>& X_rest,
                const std::vector<Index>& A_rest, 
                const std::vector<Index>& Y_rest, 
                const std::vector<Index>& O_rest) const;
    //functions regarding CPDs
        ///Set the CPDDiscreteInterface for Y
        void SetCPD_Y(Index yI, CPDDiscreteInterface* cpt)
        { _m_Y_CPDs.at(yI) = cpt; }
        ///Set the CPDDiscreteInterface for O
        void SetCPD_O(Index oI, CPDDiscreteInterface* cpt)
        { _m_O_CPDs.at(oI) = cpt; }

        CPDDiscreteInterface* GetCPD_Y(Index yI)
        { return(_m_Y_CPDs.at(yI)); }
        ///Set the CPDDiscreteInterface for O
        CPDDiscreteInterface* GetCPD_O(Index oI)
        { return(_m_O_CPDs.at(oI)); }
        
        ///Get the probability of all possible values of yIndex given II
        /**\li  yIndex the index of the state factor for which we request
         *      the probability of all its possible values.
         * \li  Xii,Aii, Yii are the vectors of influence instantiations
         *      of exact scopes. (I.e., Xii, Aii and Yii are of scope as 
         *      specified by GetXSoI_Y(), GetASoI_Y(), GetYSoI_Y(). )
         *
         */
        std::vector<double> GetYProbabilitiesExactScopes( 
                                        const std::vector<Index>& Xii,
                                        const std::vector<Index>& Aii,
                                        const std::vector<Index>& Yii,
                                        const Index& yIndex ) const;
        ///Get the probability of Y given X,A. All are full-length
        /**because all factors are included, we do not need to discriminate
         * Yii and Y.
         */
        double GetYProbability( const std::vector<Index>& X,
                                const std::vector<Index>& A,
                                const std::vector<Index>& Y) const;
        ///Get the probability of Y given X,A. general version
        /**We need to discriminate between 
         * \li  Y, the vector of next-stage state variables 
         *      for which we want to know the probability, 
         * \li  YII the vector of next-stage that can influence the SFs in 
         *      Yscope.
         *
         * YIIscope needs to be a superset of Yscope and YII and Y need to 
         * specify the same values for the same NS SFs.
         *
         *
         * This function returns the probability 
         * \f[ \Pr(Y | X, A, (YII \setminus Y) ) \f]
         *
         */
        double GetYProbabilityGeneral( 
                    const Scope& Xscope,
                    const std::vector<Index>& X,
                    const Scope& Ascope,
                    const std::vector<Index>& A,
                    const Scope& YIIscope,
                    const std::vector<Index>& YII,
                    const Scope& Yscope,
                    const std::vector<Index>& Y
                    ) const;

        ///Get the probability of all possible values of oIndex given II
        /**\li  oIndex the index of the observation factor (i.e., of the agent)
         *      for which we request
         *      the probability of all its possible values.
         * \li  Aii,Yii, Oii are the vectors of influence instantiations
         *      of exact scopes. (I.e., Aii, Yii and Oii are of scope as 
         *      specified by GetASoI_O(), GetYSoI_O(), GetOSoI_O(). )
         *
         */
        std::vector<double> GetOProbabilitiesExactScopes( 
                                        const std::vector<Index>& Aii,
                                        const std::vector<Index>& Yii,
                                        const std::vector<Index>& Oii,
                                        const Index& oIndex ) const;        
        std::vector<double> GetOProbabilitiesExactScopes( 
                                        const std::vector<Index>& Xii,
                                        const std::vector<Index>& Aii,
                                        const std::vector<Index>& Yii,
                                        const std::vector<Index>& Oii,
                                        const Index& oIndex ) const;
        ///Get the probability of O given A,Y. All std::vectors are full length
        double GetOProbability( const std::vector<Index>& A,
                                const std::vector<Index>& Y,
                                const std::vector<Index>& O) const;
        double GetOProbability( const std::vector<Index>& X,
                                const std::vector<Index>& A,
                                const std::vector<Index>& Y,
                                const std::vector<Index>& O) const;
        ///Compute the probability P(Ys,Os|Xs,As)
        /**All std::vectors are of specified scopes. When the probability is 
         * undefined (because X and A do not contain all necessary vars)
         * an exception is thrown
         */
        double GetYOProbability(const Scope& X, const std::vector<Index>& Xs,
                                const Scope& A, const std::vector<Index>& As,
                                const Scope& Y, const std::vector<Index>& Ys,
                                const Scope& O, const std::vector<Index>& Os) const;
        ///Sample a NS state
        std::vector<Index> SampleY(  const std::vector<Index>& X,
                                     const std::vector<Index>& A) const;
        ///Sample an observation.
        std::vector<Index> SampleO(  const std::vector<Index>& A,
                                     const std::vector<Index>& Y) const
        {return SampleO(std::vector<Index>(), A, Y); }
        
        std::vector<Index> SampleO(  const std::vector<Index>& X,
                                     const std::vector<Index>& A,
                                     const std::vector<Index>& Y) const;

        ///Perfom the Stat and Agent Scope backup
        /**this function is called by StateScopeBackup and  
         * AgentScopeBackup. If you need both, this is therefore more efficient.
         * Xout and Aout are output arguments containing the backed-up scopes.
         */
        void ScopeBackup( const Scope & Y, 
                                const Scope & X,
                                Scope& Xout,
                                Scope& Aout
                                ) const;
        ///Get the state factors that are a ancestor of the arguments
        Scope StateScopeBackup( const Scope & stateScope, 
                                const Scope & agentScope) const;
        ///Get the agent actions that are a ancestor of the arguments
        Scope AgentScopeBackup( const Scope & stateScope, 
                                const Scope & agentScope) const;

        ///Add a CPD for a NS SF y
        void AddCPDForY(Index y);
        ///Add a CPD for an observation o
        void AddCPDForO(Index o);
        
        std::string SoftPrint() const;
        std::string SoftPrintSoI_Y(Index y) const;
        std::string SoftPrintSoI_O(Index agI) const;

       
                
        

};


#endif /* !_TWOSTAGEDYNAMICBAYESIANNETWORK_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
