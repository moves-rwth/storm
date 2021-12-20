#pragma once

#include "storm-config.h"
#ifdef STORM_HAVE_XERCES
#include <string>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLString.hpp>

#include "storm-gspn/storage/gspn/GSPN.h"

#include "storm-gspn/storage/gspn/GspnBuilder.h"

namespace storm {
namespace parser {

/* Parses a pnml-file to a gspn.
   IMPORTANT: The names of places, transitions and arcs must differ from each other.
 */
class PnmlParser {
   public:
    /*!
     * Parses the given file into the GSPN.
     *
     * @param filename The name of the file to parse.
     * @return The resulting GSPN.
     */
    storm::gspn::GSPN* parse(xercesc::DOMElement const* elementRoot);

   private:
    /*!
     * Traverse the root element.
     *
     * @param element The root element of the DOM object.
     */
    void traversePnmlElement(xercesc::DOMElement const* const element);

    /*!
     * Traverse a net or page node.
     *
     * @param node The net or page node.
     */
    void traverseNetOrPage(xercesc::DOMNode const* const node);

    /*!
     * Traverse a place node.
     *
     * @param node The place node.
     */
    void traversePlace(xercesc::DOMNode const* const node);

    /*!
     * Traverse a transition node.
     *
     * @param node The transition node.
     */
    void traverseTransition(xercesc::DOMNode const* const node);

    /*!
     * Traverse an arc node.
     *
     * @param node The arc node.
     */
    void traverseArc(xercesc::DOMNode const* const node);

    /*!
     * Traverse an initial marking node.
     *
     * @param node the initial marking node.
     * @return The number of initial tokens.
     */
    uint_fast64_t traverseInitialMarking(xercesc::DOMNode const* const node);

    /*!
     * Traverse a capacity node.
     *
     * @param node The capacity node.
     * @return The capacity for the place.
     */
    int_fast64_t traverseCapacity(xercesc::DOMNode const* const node);

    /*!
     * Traverse a inscription node.
     *
     * @param node The inscription node.
     * @return The multiplicty for the arc.
     */
    uint_fast64_t traverseMultiplicity(xercesc::DOMNode const* const node);

    /*!
     * Traverse a rate node.
     *
     * @param node The rate node.
     * @return The rate or weight of the transition.
     */
    std::string traverseTransitionValue(xercesc::DOMNode const* const node);

    /*!
     * Traverse a timed node.
     *
     * @param node The timed node.
     * @return False if the tranisition is immediate
     */
    bool traverseTransitionType(xercesc::DOMNode const* const node);

    /*!
     * Traverse a type node.
     *
     * @param node The type node.
     * @return Returns a string with the arc type.
     */
    std::string traverseArcType(xercesc::DOMNode const* const node);

    /**
     * Traverse a priority node.
     * @param node The priority node.
     * @return Returns the priority of the transition.
     */
    uint_fast64_t traversePriority(xercesc::DOMNode const* const node);

    // the constructed gspn
    storm::gspn::GspnBuilder builder;

    // default value for initial tokens
    uint_fast64_t defaultNumberOfInitialTokens = 0;

    // default value for capacities
    int_fast64_t defaultCapacity = -1;

    // default value for the transition type (false == immediate transition)
    bool defaultTransitionType = false;

    // default value for the arc type
    std::string defaultArcType = "normal";

    // default multiplicity for arcs
    uint_fast64_t defaultMultiplicity = 1;

    // default priority for transitions
    uint_fast64_t defaultPriority = 0;
};
}  // namespace parser
}  // namespace storm

#endif
