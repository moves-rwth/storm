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
#ifndef _OGET_H_
#define _OGET_H_ 1

/* the include directives */
#include "Globals.h"

#include "ObservationModelMapping.h"
#include "ObservationModelMappingSparse.h"
#include "EventObservationModelMapping.h"
#include "EventObservationModelMappingSparse.h"

/** \brief OGet can be used for direct access to the observation model.  */
class OGet 
{
public:
    virtual ~OGet() = 0;
    //get (data) functions:
    virtual double Get(Index jaI, Index sucSI, Index joI) const = 0;
    virtual double Get(Index sI, Index jaI, Index sucSI, Index joI) const
    {return Get(jaI, sucSI, joI);}
};

//http://www.parashift.com/c++-faq-lite/pointers-to-members.html
//says "defined even though it's pure virtual; it's faster this way; trust me"
inline OGet::~OGet() {}

/** \brief OGet_ObservationModelMapping can be used for direct access
 * to a ObservationModelMapping.  */
class OGet_ObservationModelMapping : public OGet
{
 
private:
    std::vector<ObservationModelMapping::Matrix* > _m_O;
public:
    OGet_ObservationModelMapping( ObservationModelMapping* om)
    {
        _m_O = om->_m_O;
    };

    virtual double Get(Index jaI, Index sucSI, Index joI) const
        {  { return((*_m_O[jaI])(sucSI,joI)); } }

};

/** \brief OGet_ObservationModelMappingSparse can be used for direct
 * access to a ObservationModelMappingSparse.  */
class OGet_ObservationModelMappingSparse : public OGet
{
 
private:
    std::vector<ObservationModelMappingSparse::SparseMatrix* > _m_O;
public:
    OGet_ObservationModelMappingSparse( ObservationModelMappingSparse* om)
    {
        _m_O = om->_m_O;
    };

    virtual double Get(Index jaI, Index sucSI, Index joI) const
        {  { return((*_m_O[jaI])(sucSI,joI)); } }

};

class OGet_EventObservationModelMapping : public OGet
{
 
private:
    std::vector<std::vector <EventObservationModelMapping::Matrix* > > _m_O;
public:
    OGet_EventObservationModelMapping( EventObservationModelMapping* om)
    {
        _m_O = om->_m_O;
    };

    virtual double Get(Index jaI, Index sucSI, Index joI) const
    {  throw E("Cannot refer to an Event Observation Model with (o,s',a). Use Get(s,a,s',o) instead."); }

    virtual double Get(Index sI, Index jaI, Index sucSI, Index joI) const
        {  { return((*_m_O[jaI][sI])(sucSI,joI)); } }

};

class OGet_EventObservationModelMappingSparse : public OGet
{
 
private:
    std::vector<std::vector <EventObservationModelMappingSparse::SparseMatrix* > > _m_O;
public:
    OGet_EventObservationModelMappingSparse( EventObservationModelMappingSparse* om)
    {
        _m_O = om->_m_O;
    };

    virtual double Get(Index jaI, Index sucSI, Index joI) const
    {  throw E("Cannot refer to an Event Observation Model with (o,s',a). Use Get(s,a,s',o) instead."); }

    virtual double Get(Index sI, Index jaI, Index sucSI, Index joI) const
        {  { return((*_m_O[jaI][sI])(sucSI,joI)); } }

};

#endif /* !_OGET_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
