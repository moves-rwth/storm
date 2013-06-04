/*
 * Ltl2dstarFormulaAdapter.h
 *
 *  Created on: 29.05.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_ADAPTER_LTL2DSTARFORMULAADAPTER_H_
#define STORM_ADAPTER_LTL2DSTARFORMULAADAPTER_H_

#include "formula/Ltl.h"
#include "models/AtomicPropositionsLabeling.h"

#include "LTLFormula.hpp"	//LTL2Dstar include file

namespace storm {
namespace adapters {

/*!
 * @brief
 * Conversion routines for creating an equivalent formula of the LTL2DStar classes out of storm formula classes.
 *
 * @note
 * 		Storm allows bounded until and eventually operators, which are not available in LTL2Dstar
 * 		These formulas are unfolded into an equivalent formula using nested next operators.
 *
 * This class uses the visitor pattern for traversing the LTL formula.
 * Due to the limitations of templates in C++ the callback for the visit functions are of type void. As a workaround,
 * intermediate result formulas are stored in the field intermediateResult.
 *
 */
template<class T>
class Ltl2dstarFormulaAdapter :
		public storm::property::ltl::visitor::AbstractLtlFormulaVisitor<T>,
		public storm::property::ltl::IAndVisitor<T>,
		public storm::property::ltl::IApVisitor<T>,
		public storm::property::ltl::IBoundedEventuallyVisitor<T>,
		public storm::property::ltl::IBoundedUntilVisitor<T>,
		public storm::property::ltl::IEventuallyVisitor<T>,
		public storm::property::ltl::IGloballyVisitor<T>,
		public storm::property::ltl::INextVisitor<T>,
		public storm::property::ltl::INotVisitor<T>,
		public storm::property::ltl::IOrVisitor<T>,
		public storm::property::ltl::IUntilVisitor<T> {
public:
	virtual ~Ltl2dstarFormulaAdapter() {
		// TODO Auto-generated destructor stub
	}

	/*!
	 * Visit method for an ltl And formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * @param formula Reference to the And formula that is to be visited.
	 */
	virtual void visitAnd(const storm::property::ltl::And<T>& formula) {
		this->visit(formula.getLeft());
		LTLNode_p left = intermediateResult;
		this->visit(formula.getRight());
		LTLNode_p right = intermediateResult;
		intermediateResult.reset(new LTLNode(LTLNode::T_AND, left, right));
	}

	/*!
	 * Visit method for an ltl Ap formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * @param formula Reference to the Ap formula that is to be visited.
	 */
	virtual void visitAp(const storm::property::ltl::Ap<T>& formula) {
		intermediateResult.reset(new LTLNode(apset->find(formula.getAp())));
	}

	/*!
	 * Visit method for an ltl Bounded Eventually formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * As Bounded Eventually is not supported in LTL2Dstar, the formula is unfolded into an equivalent
	 * formula using nested Next operators.
	 *
	 * @param formula Reference to the Bounded Eventually formula that is to be visited.
	 */
	virtual void visitBoundedEventually(const storm::property::ltl::BoundedEventually<T>& formula) {
		this->visit(formula.getChild());
		LTLNode_p child = intermediateResult;

		//As ltl2dstar does not support bounded eventually, we transform it to (phi || X (phi || X ( ... ) ) )
		for (uint_fast64_t i = 0; i < formula.getBound(); i++) {
			LTLNode_p nextOp(new LTLNode(LTLNode::T_NEXTSTEP, intermediateResult));
			intermediateResult.reset(new LTLNode(LTLNode::T_OR, child, nextOp));
		}
		//TODO: Check if this works, even though the object child is never cloned, but referenced several time in the tree...
	}

	/*!
	 * Visit method for an ltl Bounded Until formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * As Bounded Eventually is not supported in LTL2Dstar, the formula is unfolded into an equivalent
	 * formula using nested Next operators.
	 *
	 * @param formula Reference to the Bounded Until formula that is to be visited.
	 */
	virtual void visitBoundedUntil(const storm::property::ltl::BoundedUntil<T>& formula) {
		this->visit(formula.getLeft());
		LTLNode_p left = intermediateResult;
		this->visit(formula.getRight());
		LTLNode_p right = intermediateResult;

		//As ltl2dstar does not support bounded until, we transform it to (phi || psi && X (phi || X ( ... ) ) )
		for (uint_fast64_t i = 0; i < formula.getBound(); i++) {
			LTLNode_p nextOp(new LTLNode(LTLNode::T_NEXTSTEP, intermediateResult));
			LTLNode_p andOp(new LTLNode(LTLNode::T_AND, left, nextOp));
			intermediateResult.reset(new LTLNode(LTLNode::T_OR, right, andOp));
		}
		//TODO: Check if this works, even though the objects left and right are never cloned,
		//but referenced several time in the tree...
	}

	/*!
	 * Visit method for an ltl Eventually formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * In LTL2DStar, the type for this formula is called "Finally".
	 *
	 * @param formula Reference to the Eventually formula that is to be visited.
	 */
	virtual void visitEventually(const storm::property::ltl::Eventually<T>& formula) {
		this->visit(formula.getChild());
		LTLNode_p child = intermediateResult;
		intermediateResult.reset(new LTLNode(LTLNode::T_FINALLY, child));
	}

	/*!
	 * Visit method for an ltl Globally formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * @param formula Reference to the Globally formula that is to be visited.
	 */
	virtual void visitGlobally(const storm::property::ltl::Globally<T>& formula) {
		this->visit(formula.getChild());
		LTLNode_p child = intermediateResult;
		intermediateResult.reset(new LTLNode(LTLNode::T_GLOBALLY, child));
	}

	/*!
	 * Visit method for an ltl Next formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * @param formula Reference to the Next formula that is to be visited.
	 */
	virtual void visitNext(const storm::property::ltl::Next<T>& formula) {
		this->visit(formula.getChild());
		LTLNode_p child = intermediateResult;
		intermediateResult.reset(new LTLNode(LTLNode::T_NEXTSTEP, child));
	}

	/*!
	 * Visit method for an ltl Not formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * @param formula Reference to the Not formula that is to be visited.
	 */
	virtual void visitNot(const storm::property::ltl::Not<T>& formula) {
		this->visit(formula.getChild());
		LTLNode_p child = intermediateResult;
		intermediateResult.reset(new LTLNode(LTLNode::T_NOT, child));
	}

	/*!
	 * Visit method for an ltl Or formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * @param formula Reference to the Or formula that is to be visited.
	 */
	virtual void visitOr(const storm::property::ltl::Or<T>& formula) {
		this->visit(formula.getLeft());
		LTLNode_p left = intermediateResult;
		this->visit(formula.getRight());
		LTLNode_p right = intermediateResult;
		intermediateResult.reset(new LTLNode(LTLNode::T_OR, left, right));
	}

	/*!
	 * Visit method for an ltl Until formula.
	 * Stores the converted formula (in LTL2Dstar classes) in the field intermediateResult
	 *
	 * @param formula Reference to the Until formula that is to be visited.
	 */
	virtual void visitUntil(const storm::property::ltl::Until<T>& formula) {
		this->visit(formula.getLeft());
		LTLNode_p left = intermediateResult;
		this->visit(formula.getRight());
		LTLNode_p right = intermediateResult;
		intermediateResult.reset(new LTLNode(LTLNode::T_UNTIL, left, right));
	}

	/*!
	 * @brief
	 * Start the conversion of the formula tree
	 *
	 * As LTL2DStar uses the shared pointers of the boost library, the converted object is stored in these.
	 *
	 * @param formula The LTL formula to be converted
	 * @return A boost shared pointer to the converted formula.
	 */
	LTLNode_p convertToLtlNode_p(storm::property::ltl::AbstractLtlFormula<T> const& formula) {
		this->visit(formula);
		return intermediateResult;
	}

	/*!
	 * Returns an APSet object which contains all atomic propositions of the labeling parameter
	 * @param labeling The labeling of which the APs are extracted
	 * @return
	 */
	static APSet_cp extractAPSetFromLabeling(storm::models::AtomicPropositionsLabeling const& labeling) {
		//TODO: Make sure that the index of the AP is the same in the labeling and the APSet.
		APSet* apset = new APSet();

		//Insert each AP of the labeling into the APSet
		for (auto it = labeling.getNameToLabelingMap().begin();
			 it != labeling.getNameToLabelingMap().end();
			 it++) {
			apset->addAP(it->first);
		}

		//Insert our object into the shared pointer
		return APSet_cp(apset);
	}

	/*!
	 * Convert an LTL formula using the storm data structures into an equivalent one for LTL2Dstar.
	 *
	 * @note
	 * 		Storm allows bounded until and eventually operators, which are not available in LTL2Dstar
	 * 		These formulas are unfolded into an equivalent formula using nested next operators.
	 *
	 * @param formula The formula object to convert
	 * @param labeling A labeling that contains (at least) each AP that is used in the formula
	 * @return
	 */
	static LTLFormula convert(storm::property::ltl::AbstractLtlFormula<T> const& formula,
							  storm::models::AtomicPropositionsLabeling const& labeling) {
		//First an APSet is extracted out of the labeling
		APSet_cp apset = extractAPSetFromLabeling(labeling);

		//The visitor has to be an actual object, which is created here
		Ltl2dstarFormulaAdapter adapter(apset);

		//The return value is the result of the visitor object
		return LTLFormula(adapter.convertToLtlNode_p(formula), apset);
	}

protected:
	/*!
	 * Constructor is protected, as the object should only be created in the static convert method.
	 *
	 * @param apset A set of atomic propositions (of LTL2Dstar)
	 */
	Ltl2dstarFormulaAdapter(APSet_cp apset) :
		apset(apset) {
		//Intentionally left empty
	}

private:
	/*!
	 * Used to store the intermediate results of the conversion routine
	 */
	LTLNode_p intermediateResult;

	/*!
	 * Stores the apset information...
	 */
	APSet_cp apset;



};

} /* namespace adapters */
} /* namespace storm */

#endif /* STORM_ADAPTER_LTL2DSTARFORMULAADAPTER_H_ */
