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
 *
 * For contact information please see the included AUTHORS file.
 */

#include "ParserPOMDPFormat_Spirit.h"
//#include "CommentOrBlankParser.h"            
#include "CommentBlankOrNewlineParser.h"            

using namespace std;

namespace POMDPFormatParsing{

/**Outputs the file_position structure info (gotten from 
 * postion_iterator::get_position() ). */
std::ostream& operator<<(std::ostream& out, file_position const& lc)
{
    return out <<
            "\nFile:\t" << lc.file <<
            "\nLine:\t" << lc.line <<
            "\nCol:\t" << lc.column << endl;
}


//Default constructor
ParserPOMDPFormat_Spirit::ParserPOMDPFormat_Spirit(POMDPDiscrete* problem) :
//    UNIFORMTOK(this),
    _m_decPOMDPDiscrete(problem)
{
    if(DEBUG_POMDP_PARSE)
    {
        cout << "Creating parser, referring to problem...";
        cout << problem->MultiAgentDecisionProcess::SoftPrint();
        cout << endl;
    }
    //need to reset the parse matrix the first time...
    _m_matrixModified = true;
    _m_identListModified = false;
    _m_startStateListExclude = false;
    _m_nrA = 0;
    _m_curAI = 0;
 //   UNIFORMTOK = uniformtok(this);

}

/* 
//  Actions
void ParserPOMDPFormat_Spirit::echo(iterator_t first, iterator_t const& last)
{
    while (first != last)
        std::cout << *first++;
}

*/

//  Main program
void ParserPOMDPFormat_Spirit::Parse()
{
    string pf = GetPOMDPDiscrete()->GetProblemFile();
    const char* pf_c = pf.c_str();
    // Create a file iterator for this file
    iterator_t_fi first_fi(pf_c);
    if (!first_fi)
    {
        stringstream ss; ss << "Unable to open file: "<<pf<<" !\n";
        throw E(ss);
    }
    iterator_t_fi last_fi = first_fi.make_end();// Create an EOF iterator

    iterator_t first( first_fi, last_fi, pf_c );
    iterator_t last;

    POMDPFileParser pomdpfp(this);
    comment_cbonlp::CommentBlankorNewLineParser skip_parser;
    parse_info<iterator_t> info; 
    

    //+
    // Parse
    // Spirit changed when upgrading from 1.33.1 to 1.34.0:
    // trailing spaces are no longer accepted, which the next line
    // to fail on some problems (those with trailing comments). Adding
    // ">> !end_p" to the grammar works around this problem.
    info = parse(first, last , pomdpfp >> !end_p, skip_parser);
    if (info.full)
    {
        if(DEBUG_POMDP_PARSE) cout << ">>>Parsing succeeded\n";
    }
    else
    {
        iterator_t st = info.stop;
        iterator_t st2 = info.stop;
        for(Index i=1;i<200 && st2 != last ;i++)st2++;
        string unparsed(st, st2);
        cout << "stopped at: " << info.stop.get_position() 
            << "-> unparsed : "<< "\"" << unparsed << "\"\n";
        cout << "partial match? hit: \"" << info.hit << "\"\n";
        cout << "consumed chars - length: \"" << info.length << "\"\n";
        throw E("Parsing failed");
    }

    if(DEBUG_POMDP_PARSE)  cout << "-------------------------\n";
    return;
}



//used functor functions:
void ParserPOMDPFormat_Spirit::DebugOutput::operator()(iterator_t str, iterator_t end) const
{
    if(DEBUG_POMDP_PARSE) cout << s <<" parsed: \"" << string(str,end) << "\""<<endl;
}
void ParserPOMDPFormat_Spirit::DebugOutput::operator()(const int& i) const
{
    if(DEBUG_POMDP_PARSE) cout << s <<" parsed an int: \"" << i << "\""<<endl;
}
void ParserPOMDPFormat_Spirit::DebugOutput::operator()(const unsigned int& i) const
{
    if(DEBUG_POMDP_PARSE) cout << s <<" parsed an unsigned int: \"" << i << "\""<<endl;
}
void ParserPOMDPFormat_Spirit::DebugOutput::operator()(const double& d) const
{
    if(DEBUG_POMDP_PARSE) cout << s <<" parsed a double: \"" << d << "\""<<endl;
}
void ParserPOMDPFormat_Spirit::DebugOutputNoParsed::operator()(iterator_t , iterator_t ) const
{
    if(DEBUG_POMDP_PARSE) cout << s <<endl;
}
void ParserPOMDPFormat_Spirit::DebugOutputNoParsed::operator()(const unsigned int&) const
{
    if(DEBUG_POMDP_PARSE) cout << s <<endl;
}
void ParserPOMDPFormat_Spirit::DebugOutputNoParsed::operator()(const double &) const
{
    if(DEBUG_POMDP_PARSE) cout << s <<endl;
}

void ParserPOMDPFormat_Spirit::StoreLastParsedElement::operator()(const int i) const
{
    if(DEBUG_POMDP_PARSE) cout << "Stored Last Parsed: int "<<i<<endl;
    _m_po->_m_lp_int = i;
    _m_po->_m_lp_type = INT;
}
void ParserPOMDPFormat_Spirit::StoreLastParsedElement::operator()(const unsigned int i) const
{
    if(DEBUG_POMDP_PARSE) cout << "Stored Last Parsed:  uint "<<i<<endl;
    _m_po->_m_lp_uint = i;
    _m_po->_m_lp_type = UINT;
}

void ParserPOMDPFormat_Spirit::StoreLastParsedElement::operator()(const double f) const
{
    if(DEBUG_POMDP_PARSE) cout << "Stored Last Parsed: double "<<f<<endl;
    _m_po->_m_lp_double = f;
    _m_po->_m_lp_type = DOUBLE;
}

void ParserPOMDPFormat_Spirit::StoreLastParsedElement::operator()(iterator_t str, iterator_t end) const
{ 
    _m_po->_m_lp_string.clear();
    _m_po->_m_lp_string = string(str, end);
    if(DEBUG_POMDP_PARSE) cout << "Stored Last Parsed: string "<<
        _m_po->_m_lp_string << endl;
    _m_po->_m_lp_type = STRING;
}


void ParserPOMDPFormat_Spirit::SetNrAgents::operator()(iterator_t first, iterator_t last) const
{    
    if(DEBUG_POMDP_PARSE) {cout <<"setting number of agents to 1..."<<endl;} 
    _m_po->GetPOMDPDiscrete()->SetNrAgents(1);
    _m_po->_m_nrA = 1;
    return;
#if 0    
    if (_m_po->_m_lp_type != UINT)
    {
        stringstream msg;
        msg << "SetNrAgents::operator()(iterator_t first, iterator_t last) requires that last parsed element is a UINT!"<<
            "(at " << _m_po->_m_first->get_position() << ")" << endl;
        throw EParse(msg);
    }
    size_t nrA = _m_po->_m_lp_uint;
    if(DEBUG_POMDP_PARSE){cout <<"agt_SetNrAgents - nrA="<<nrA<<endl;} 
    _m_po->GetPOMDPDiscrete()->SetNrAgents(nrA);
    _m_po->_m_nrA = nrA;
#endif    
}

void ParserPOMDPFormat_Spirit::SetNrAgents::operator()(const int& i) const
{   
    if(DEBUG_POMDP_PARSE) {cout <<"setting number of agents to 1..."<<endl;} 
    _m_po->GetPOMDPDiscrete()->SetNrAgents(1);
    _m_po->_m_nrA = 1;
    return;
#if 0    
    if(DEBUG_POMDP_PARSE){cout << "i="<<i<< endl;}
    size_t nrA = i;     
    if(DEBUG_POMDP_PARSE){cout <<"agt_SetNrAgents - nrA="<<nrA<<endl;} 
    _m_po->GetPOMDPDiscrete()->SetNrAgents(nrA);
    _m_po->_m_nrA = nrA;
#endif    
}
/*
void ParserPOMDPFormat_Spirit::SetNrAgents::operator()(iterator_t first, iterator_t last) const
{   
    if(DEBUG_POMDP_PARSE){string  s(first, last);cout << "s="<<s << endl;}
    size_t nrA = _m_po->_m_lp_int;
    if(DEBUG_POMDP_PARSE){cout <<"agt_SetNrAgents - nrA="<<nrA<<endl;} 
    _m_po->GetPOMDPDiscrete()->SetNrAgents(nrA);
    _m_po->_m_nrA = nrA;

}
*/

void ParserPOMDPFormat_Spirit::WildCardJointAction::operator()(iterator_t str, iterator_t end) const
{
    if(DEBUG_POMDP_PARSE && 0)
        cout << "WildCardJointAction: _m_lp_JA.size()="<<_m_po->_m_lp_JA.size();
    //this (can) contain elements from a failed indiv_action parse.
    _m_po->_m_lp_JA.clear();
    for(Index AgI=0; AgI < _m_po->_m_nrA; AgI++)
    {
        if(DEBUG_POMDP_PARSE && 0) cout << "  pushed *  ";
        _m_po->_m_lp_JA.push_back(_m_po->_m_anyJAIndex);
    }
}
/*
void ParserPOMDPFormat_Spirit::StoreLPJointAction::operator()(iterator_t str, iterator_t end) const    
{
    _m_po->_m_lp_JAI.clear();
    if(_m_isJointActionIndex)
    {
        //_m_lp_JA not necessary: clear it.
        _m_po->_m_lp_JA.clear();
        _m_po->_m_lp_JAI.push_back(_m_po->_m_lp_int);
        return;
    }
    try{ MatchingJointActions(0,_m_po->_m_lp_JA); }
    catch(E e)
    {
        file_position fp =  str.get_position();
        stringstream ermsg;
        ermsg << e.SoftPrint() << endl << "(at " << fp << ")"<<endl;
        throw E(ermsg);
    }
    _m_po->_m_lp_JA.clear();
}
void ParserPOMDPFormat_Spirit::StoreLPJointAction::operator()(const unsigned int&) const
{
    _m_po->_m_lp_JAI.clear();
    if(_m_isJointActionIndex)
    {
        //_m_lp_JA not necessary: clear it.
        _m_po->_m_lp_JA.clear();
        _m_po->_m_lp_JAI.push_back(_m_po->_m_lp_int);
        return;
    }
    MatchingJointActions(0,_m_po->_m_lp_JA); 
    _m_po->_m_lp_JA.clear();
}
*/
void ParserPOMDPFormat_Spirit::StoreLPJointAction::Store() const
{
    _m_po->_m_lp_JAI.clear();
    //check to see if the last parsed index was a joint action  index
    if(_m_isJointActionIndex)
    {
        //_m_lp_JA not necessary: clear it.
        _m_po->_m_lp_JA.clear();
        //do check that the last parsed element was a index (uint)
        if (_m_po->_m_lp_type != UINT)
        {
            stringstream msg;
            msg << "StoreLPJointAction::Store() requires that last parsed element is a UINT! (last parsed index was a joint action index)"<< endl;
            throw EParse(msg);
        }
        _m_po->_m_lp_JAI.push_back(_m_po->_m_lp_uint);
        return;
    }
    MatchingJointActions(0,_m_po->_m_lp_JA); 
    _m_po->_m_lp_JA.clear();
}

//Construct the set of joint actions that may match any wildcards recursively:
void ParserPOMDPFormat_Spirit::StoreLPJointAction::MatchingJointActions (Index curAgent, vector<Index> indIndices) const
{
    if(_m_po->_m_nrA != indIndices.size())
    {
        stringstream msg;
        msg << "individual indices vector has wrong size. ("<< indIndices.size() << " while nrAgents is " << _m_po->_m_nrA << ")"<<
            "at(" << _m_po->_m_first->get_position() << ")" << endl;;
        throw EParse(msg);
    }
    if(curAgent == _m_po->_m_nrA) //past last agent: all work done
    {
        Index jaI = 0;
        try
        {
            jaI = _m_po->GetPOMDPDiscrete()->IndividualToJointActionIndices(indIndices);
            _m_po->_m_lp_JAI.push_back(jaI);
        }
        catch(E e)
        {
            stringstream ss; ss << e.SoftPrint() << "(at "<< _m_po->_m_first-> get_position()<<")"<<endl;
            throw(EParse(ss));
        }        
        if(DEBUG_POMDP_PARSE)
        {
            cout<<"MatchingJointActions: joint action index for < ";
            vector<Index>::iterator it = indIndices.begin();
            vector<Index>::iterator last = indIndices.end();
            while(it != last){ cout << *it <<" "; it++;}
            cout <<   "> = "<<jaI<<endl;
        }
    }
    else //not last agent: work to be done
    {
        if(indIndices[curAgent] == _m_po->_m_anyJAIndex)  //<- IF THIS IS A WILDCARD
        {
            //do for each action of this agent
            //size_t nrAc = _m_po->GetPOMDPDiscrete()->GetNrActions(curAgent);
            size_t nrAc = _m_po->GetPOMDPDiscrete()->GetNrSingleAgentActions();
            for(Index ai=0; ai < nrAc; ai++)
            {
                indIndices[curAgent] = ai;
                MatchingJointActions(curAgent+1, indIndices);
            }
        }
        else
            MatchingJointActions(curAgent+1, indIndices);
    }
}

void ParserPOMDPFormat_Spirit::WildCardJointObservation::operator()(iterator_t str, iterator_t end) const
{
    if(DEBUG_POMDP_PARSE )
        cout << "WildCardJointObservation: _m_lp_JOI.size()="<<_m_po->_m_lp_JOI.size();
    //this (can) contain elements from a failed indiv_action parse.
    _m_po->_m_lp_JO.clear();
    _m_po->_m_lp_JOI.clear();
    //this is different from how we handle joint actions: joint actions are 
    //immediately expanded (i.e. '*'/ANY_INDEX is replaced by all matching
    //indices.) 
    //For joint observations, this is not practical, as for most common reward
    //forms, this is not required. I.e., one will typically specify
    //R: ja : s : * : * : prob 
    //then we want to call addReward(ja,s) and not expand the succesor states
    //and joint observations...

    _m_po->_m_lp_JOI.push_back(_m_po->_m_anyJOIndex);
}

/*
void ParserPOMDPFormat_Spirit::StoreLPJointObservation::operator()(const unsigned int&) const
{
    Store();
}
void ParserPOMDPFormat_Spirit::StoreLPJointObservation::operator()(iterator_t str, iterator_t end) const
{
    Store();
}
*/
void ParserPOMDPFormat_Spirit::StoreLPJointObservation::Store() const
{
    _m_po->_m_lp_JOI.clear();
    if(_m_isJointObservationIndex) //as opposed to parsing individual indices
    {
        //_m_lp_JO not necessary: clear it.
        _m_po->_m_lp_JO.clear();
        //do check that the last parsed element was a index (uint)
        if (_m_po->_m_lp_type != UINT)
        {
            stringstream msg;
            msg << "StoreLPJointObservation::Store() requires that last parsed element is a UINT! (last parsed index was a joint action index)"<< endl;
            throw EParse(msg);
        }
        _m_po->_m_lp_JOI.push_back(_m_po->_m_lp_uint);
        return;
    }
    MatchingJointObservations(0,_m_po->_m_lp_JO); 
    _m_po->_m_lp_JO.clear();
}
/*
void ParserPOMDPFormat_Spirit::StoreLPJointObservation::operator()(iterator_t str, iterator_t end) const
{
    _m_po->_m_lp_JOI.clear();
    if(_m_isJointObservationIndex) //as opposed to parsing individual indices
    {
        //_m_lp_JO not necessary: clear it.
        _m_po->_m_lp_JO.clear();
        _m_po->_m_lp_JOI.push_back(_m_po->_m_lp_int);
        return;
    }
    try{ MatchingJointObservations(0,_m_po->_m_lp_JO); }
    catch(E e)
    {
        stringstream ermsg;
        ermsg << e.SoftPrint() << endl << "(at " << str.get_position() << ")"<<endl;
        throw E(ermsg);
    }
    _m_po->_m_lp_JO.clear();
}
*/
void ParserPOMDPFormat_Spirit::StoreLPJointObservation::MatchingJointObservations (Index curAgent, vector<Index> indIndices) const
{
    if(_m_po->_m_nrA != indIndices.size())
    {
        stringstream msg;
        msg << "individual obs indices vector has wrong size. ("<<indIndices.size() << 
            " while nrAgents is " <<_m_po->_m_nrA << ")" << "at(" << 
            _m_po->_m_first->get_position() << ")" << endl;;
        throw EParse(msg);
    }
    if(curAgent == _m_po->_m_nrA) //past last agent: all work done
    {
        Index joI = 0;
        try
        {
            joI = _m_po->GetPOMDPDiscrete()->IndividualToJointObservationIndices(
                    indIndices);
            _m_po->_m_lp_JOI.push_back(joI);
        }
        catch(E e)
        {
            stringstream ss; ss <<e.SoftPrint() << "at(" << _m_po->_m_first->get_position() <<
                ")" << endl;
            throw EParse(ss);
        }
        if(DEBUG_POMDP_PARSE)
        {
            cout<<"MatchingJointObservations: joint observation index for < ";
            vector<Index>::iterator it = indIndices.begin();
            vector<Index>::iterator last = indIndices.end();
            while(it != last){ cout << *it <<" "; it++;}
            cout <<   "> = "<<joI<<endl;
        }
    }
    else //not past last agent: work to be done
    {
        if(indIndices[curAgent] == _m_po->_m_anyJOIndex)
        {
            //do for each action of this agent
            //size_t nrObs = _m_po->GetPOMDPDiscrete()->GetNrObservations(curAgent);
            size_t nrObs = _m_po->GetPOMDPDiscrete()->GetNrSingleAgentObservations();
            for(Index oi=0; oi < nrObs; oi++)
            {
                indIndices[curAgent] = oi;
                MatchingJointObservations(curAgent+1, indIndices);
            }
        }
        else
            MatchingJointObservations(curAgent+1, indIndices);
    }
}


void ParserPOMDPFormat_Spirit::StoreLPFromState::operator()(iterator_t str, iterator_t end) const
{
    if(DEBUG_POMDP_PARSE)
        cout << "StoreLPFromState: pushing "<<string(str,end)<< " on _m_po->_m_lp_sI"<<endl;
    //we are parsing a new from state - the previous is not valid
    //any more, so we clear the state index vector
    _m_po->_m_lp_fromSI.clear();
    if(_m_po->_m_lp_type == UINT)
    {
        unsigned int index = _m_po->_m_lp_uint;
        size_t nrS =  _m_po->GetPOMDPDiscrete()->GetNrStates();
        if(index >= nrS)
        {            
            stringstream ss; ss<<"StoreLPFromState: '"<< index<< "' is not a valid state index!? Number of states is " << nrS <<" (at " <<str.get_position()<<")"<<endl; throw E(ss);
        }

        _m_po->_m_lp_fromSI.push_back( _m_po->_m_lp_uint );
    }
    else if(_m_po->_m_lp_type == STRING)
    {
        try
        {
            Index sI = _m_po->GetPOMDPDiscrete()->GetStateIndexByName(
                _m_po->_m_lp_string);
            _m_po->_m_lp_fromSI.push_back(sI);
        }
        catch(E e)
        {
            stringstream ermsg; ermsg << e.SoftPrint() << " (at " << 
                str.get_position() << ")"<<endl;
            throw E(ermsg);
        }
    }
    else if(_m_po->_m_lp_type == ASTERICK )
        _m_po->_m_lp_fromSI.push_back(_m_po->_m_anyStateIndex);
    else
        throw E("StoreLPFromState expected that the last parsed type is a state index(uint), state name (string) or wilcard ('*').");
}


void ParserPOMDPFormat_Spirit::StoreLPToState::operator()(iterator_t str, iterator_t end) const
{
    if(DEBUG_POMDP_PARSE)
        cout << "StoreLPToState: pushing "<<string(str,end)<< " on __m_po->_m_lp_sI"<<endl;
    //we are parsing a new to state - the previous is not valid
    //any more, so we clear the state index vector
    _m_po->_m_lp_toSI.clear();
    if(_m_po->_m_lp_type == UINT)
    {
        unsigned int index = _m_po->_m_lp_uint;
        size_t nrS =  _m_po->GetPOMDPDiscrete()->GetNrStates();
        if(index >= nrS)
        {            
            stringstream ss; ss<<"StoreLPToState: '"<< index<< "' is not a valid state index!? Number of states is " << nrS <<" (at " <<str.get_position()<<")"<<endl; throw E(ss);
        }

        _m_po->_m_lp_toSI.push_back( _m_po->_m_lp_uint );
    }
    else if(_m_po->_m_lp_type == STRING)
    {
        try
        {
            Index sI = _m_po->GetPOMDPDiscrete()->GetStateIndexByName(
                _m_po->_m_lp_string);
            _m_po->_m_lp_toSI.push_back(sI);
        }
        catch(E e)
        {
            stringstream ermsg; ermsg << e.SoftPrint() << " (at " << 
                str.get_position() << ")"<<endl;
            throw E(ermsg);
        }
    }
    else if(_m_po->_m_lp_type == ASTERICK )
        _m_po->_m_lp_toSI.push_back(_m_po->_m_anyStateIndex);
    else
        throw E("StoreLPToState expected that the last parsed type is a state index(uint), state name (string) or wilcard ('*').");
}

void ParserPOMDPFormat_Spirit::ProcessTProb::operator()(iterator_t str, iterator_t end) const
{
    if(_m_po->_m_lp_type != DOUBLE)
    {
        stringstream ss; ss<<"ProcessTProb:last parsed type should be a double. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }

    double prob = _m_po->_m_lp_double;
    if( _m_po->_m_lp_fromSI.size() != 1 || 
        _m_po-> _m_lp_toSI.size() != 1 )
    {
        stringstream ss; ss<<"ProcessTProb:_m_lp_fromSI or _m_lp_toSI has size != 1. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    Index sI = _m_po->_m_lp_fromSI[0];
    Index sucSI = _m_po-> _m_lp_toSI[0];
    Index jaI = 0;
    if( sI != _m_po->_m_anyStateIndex &&
        sucSI != _m_po->_m_anyStateIndex &&
        _m_po->_m_lp_JAI.size() != 1 )
    {
        jaI = _m_po->_m_lp_JAI[0];
        _m_po->GetPOMDPDiscrete()->SetTransitionProbability(sI, jaI, sucSI, prob);
        if(DEBUG_POMDP_PARSE)
            cout << "Setting T("<<sI<<","<<jaI<<","<<sucSI<<") = " <<prob<<endl;
        return;
    }
    //one or more wildcards: complex situation
    if( sI == _m_po->_m_anyStateIndex )
    {
        _m_po->_m_lp_fromSI.clear();
        for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
            _m_po->_m_lp_fromSI.push_back(si);
    }
    if( sucSI == _m_po->_m_anyStateIndex )
    {
        _m_po->_m_lp_toSI.clear();
        for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
            _m_po->_m_lp_toSI.push_back(si);
    }
    vector<Index>::iterator sI_it = _m_po->_m_lp_fromSI.begin();
    vector<Index>::iterator sI_last = _m_po->_m_lp_fromSI.end();
    while(sI_it != sI_last)
    {
        sI = *sI_it;
        vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
        vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
        while(jaI_it != jaI_last)
        {
            jaI = *jaI_it;
            vector<Index>::iterator sucSI_it = _m_po->_m_lp_toSI.begin();
            vector<Index>::iterator sucSI_last = _m_po->_m_lp_toSI.end();
            while(sucSI_it != sucSI_last)
            {
                sucSI = *sucSI_it; 
                _m_po->GetPOMDPDiscrete()->SetTransitionProbability(sI, jaI, sucSI, 
                        prob);
                if(DEBUG_POMDP_PARSE)
                    cout << "Setting T("<<sI<<","<<jaI<<","<<sucSI<<") = "<<
                        prob<<endl;
                sucSI_it++;
            }
            jaI_it++;
        }
        sI_it++;
    }
}

void ParserPOMDPFormat_Spirit::ProcessTRow::operator()(iterator_t str, iterator_t end) const
{
    if( _m_po->_m_lp_fromSI.size() != 1 )
    {
        stringstream ss; ss<<"ProcessTRow:_m_lp_fromSI has size != 1. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    if( !_m_po->IsRowMatrixLP() )
    {
        stringstream ss; ss<<"ProcessTRow: a row matrix should have been parsed. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    vector<double> row = _m_po->_m_curMatrix[0];
    size_t nrS = _m_po->GetPOMDPDiscrete()->GetNrStates();
    if( row.size() != nrS)
    {
        stringstream ss; ss<<"ProcessTRow: exected a row matrix with nrStates="<<nrS<<" entries. Found "<<row.size()<< " entries. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }

    Index sI = _m_po->_m_lp_fromSI[0];
    Index jaI = 0;
    if( sI == _m_po->_m_anyStateIndex )
    {
        _m_po->_m_lp_fromSI.clear();
        for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
            _m_po->_m_lp_fromSI.push_back(si);
    }

    vector<Index>::iterator sI_it = _m_po->_m_lp_fromSI.begin();
    vector<Index>::iterator sI_last = _m_po->_m_lp_fromSI.end();
    while(sI_it != sI_last)
    {
        sI = *sI_it;
        vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
        vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
        while(jaI_it != jaI_last)
        {
            jaI = *jaI_it;
            for(Index sucSI=0; sucSI < nrS; sucSI++)
            {
                _m_po->GetPOMDPDiscrete()->SetTransitionProbability(sI, jaI, sucSI, row[sucSI]);
                if(DEBUG_POMDP_PARSE)
                    cout << "Setting T("<<sI<<","<<jaI<<","<<sucSI<<") = "<<row[sucSI]<<endl;
            }
            jaI_it++;
        }
        sI_it++;
    }
}

void ParserPOMDPFormat_Spirit::ProcessTMatrix::operator()(iterator_t str, iterator_t end) const
{
    size_t nrS = _m_po->GetPOMDPDiscrete()->GetNrStates();
    if(_m_po->_m_lp_type == DOUBLE)
    {

#if POMDPFORMAT_NEWLINES_SKIPPED
        /* code for POMDP file format, which skips newlines and thus delivers the matrix as a single row...*/
        if(!_m_po->IsDimensionOfMatrix(1,nrS*nrS))
        {
            stringstream ss; 
            ss << "Expected a 1 x (nrS*nrS) row matrix."<<" (nrS="<< nrS <<"). At "<<str.get_position()<<endl;
            ss<< "(found matrix of size: " << _m_po->PrintDimensionOfMatrix() << ")" << endl;
            throw E(ss);        
        }
        vector<double>& row_vector =  _m_po->_m_curMatrix.at(0);

        //also need to loop through matchin (joint) actions, since wildcards might have been used
        Index jaI = 0;
        vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
        vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
        while(jaI_it != jaI_last)
        {
            jaI = *jaI_it;
            Index row_vector_index = 0;
            for(Index sI = 0; sI < nrS; sI++)
            {
                for(Index sucSI=0; sucSI < nrS; sucSI++)
                {
                    double prob = row_vector.at(row_vector_index);  // corresponds to _m_po->_m_curMatrix[sI][sucSI];
                    _m_po->GetPOMDPDiscrete()->SetTransitionProbability(sI, jaI, sucSI, prob);
                    row_vector_index++;
                    if(DEBUG_POMDP_PARSE)
                        cout << "Setting T("<<sI<<","<<jaI<<","<<sucSI<<") = "<<prob<<endl;
                }
            }
            jaI_it++;
        }
#else    
        /* code for DPOMDP file which does not skip newline, and therefore has the matrix shaped correctly */

        //an actual matrix was parsed, check curMatrix size
        if(!_m_po->IsDimensionOfMatrix(nrS,nrS))
        {
            stringstream ss; 
            ss << "Expected a nrS x nrS matrix."<<" (nrS="<< nrS <<"). At "<<str.get_position()<<endl;
            ss<< "(found matrix of size: " << _m_po->PrintDimensionOfMatrix() << ")" << endl;
            throw E(ss);
        }
        Index jaI = 0;
        for(Index sI = 0; sI < nrS; sI++)
        {
            vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
            vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
            while(jaI_it != jaI_last)
            {
                jaI = *jaI_it;
                for(Index sucSI=0; sucSI < nrS; sucSI++)
                {
                    double prob = _m_po->_m_curMatrix[sI][sucSI];
                    _m_po->GetPOMDPDiscrete()->SetTransitionProbability
                        (sI, jaI, sucSI, prob);
                    if(DEBUG_POMDP_PARSE)
                        cout << "Setting T("<<sI<<","<<jaI<<","<<
                            sucSI<<") = "<<prob<<endl;
                }
                jaI_it++;
            }
        }
#endif    
    }
    else if(_m_po->_m_lp_type == IDENTITY)    
    {
        Index jaI = 0;
        for(Index sI = 0; sI < nrS; sI++)
        {
            vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
            vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
            while(jaI_it != jaI_last)
            {
                jaI = *jaI_it;
                for(Index sucSI=0; sucSI < nrS; sucSI++)
                {
                    double prob = 0.0;
                    if(sucSI == sI)
                        prob = 1.0;

                    _m_po->GetPOMDPDiscrete()->SetTransitionProbability
                        (sI, jaI, sucSI, prob);
                    if(DEBUG_POMDP_PARSE)
                        cout << "Setting T("<<sI<<","<<jaI<<","<<
                            sucSI<<") = "<<prob<<endl;
                }
                jaI_it++;
            }
        }
    }
    else if(_m_po->_m_lp_type == UNIFORM)
    {
        double uniform_prob = 1.0 / nrS;
        Index jaI = 0;
        for(Index sI = 0; sI < nrS; sI++)
        {
            vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
            vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
            while(jaI_it != jaI_last)
            {
                jaI = *jaI_it;
                for(Index sucSI=0; sucSI < nrS; sucSI++)
                {
                    double prob = uniform_prob;//_m_po->_m_curMatrix[sI][sucSI];
                    _m_po->GetPOMDPDiscrete()->SetTransitionProbability
                        (sI, jaI, sucSI, prob);
                    if(DEBUG_POMDP_PARSE)
                        cout << "Setting T("<<sI<<","<<jaI<<","<<
                            sucSI<<") = "<<prob<<endl;
                }
                jaI_it++;
            }
        }
    }
    else
    {
        stringstream ss; ss << "Expected a matrix, 'identity' or "<<
           "'uniform'. (at "<<str.get_position()<<endl;
        cout << ss.str() << " - _m_po->_m_lp_type = "<<_m_po->_m_lp_type <<endl;
        throw E(ss);
    }
}


void ParserPOMDPFormat_Spirit::ProcessOProb::operator()(iterator_t str, iterator_t end) const
{
    if(_m_po->_m_lp_type != DOUBLE)
    {
        stringstream ss; ss<<"ProcessOProb:last parsed type should be a double. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    double prob = _m_po->_m_lp_double;
    if(        _m_po-> _m_lp_toSI.size() != 1 )
    {
        stringstream ss; ss<<"ProcessOProb: _m_lp_toSI has size != 1. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }    
    if(        _m_po->_m_lp_JOI.size() <= 0 )
    {
        stringstream ss; ss<<"ProcessOProb: _m_lp_JOI has size <= 0. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    Index sucSI = _m_po-> _m_lp_toSI[0];
    Index joI = _m_po->_m_lp_JOI[0];
    if( sucSI == _m_po->_m_anyStateIndex ) //if '*' -> replace by a list of all state indices
    {
        _m_po->_m_lp_toSI.clear();
        for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
            _m_po->_m_lp_toSI.push_back(si);
    }    
    if( joI == _m_po->_m_anyJOIndex ) //if '*' -> replace by list of all joint obs.indices
    {
        _m_po->_m_lp_JOI.clear();
        for(Index jo=0; jo<_m_po->GetPOMDPDiscrete()->GetNrJointObservations();jo++)
            _m_po->_m_lp_JOI.push_back(jo);
    }

    vector<Index>::iterator joI_it = _m_po->_m_lp_JOI.begin();
    vector<Index>::iterator joI_last = _m_po->_m_lp_JOI.end();
    while(joI_it != joI_last)
    {
        joI = *joI_it;
        vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
        vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
        while(jaI_it != jaI_last)
        {
            Index jaI = *jaI_it;
            vector<Index>::iterator sucSI_it = _m_po->
                _m_lp_toSI.begin();
            vector<Index>::iterator sucSI_last = _m_po->
                _m_lp_toSI.end();
            while(sucSI_it != sucSI_last)
            {
                sucSI = *sucSI_it; 
                _m_po->GetPOMDPDiscrete()->SetObservationProbability
                    (jaI, sucSI, joI, prob);
                if(DEBUG_POMDP_PARSE)
                    cout << "Setting O("<<jaI<<","<<
                        sucSI<<","<<joI<<") = "<<prob<<endl;
                sucSI_it++;
            }
            jaI_it++;
        }
        joI_it++;
    }
}

void ParserPOMDPFormat_Spirit::ProcessORow::operator()(iterator_t str, iterator_t end) const
{
    if( _m_po->_m_lp_toSI.size() != 1 )
    {
        stringstream ss; ss<<"ProcessORow:_m_lp_fromSI has size != 1. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    if( !_m_po->IsRowMatrixLP() )
    {
        stringstream ss; ss<<"ProcessORow: a row matrix should have been parsed. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    vector<double> row = _m_po->_m_curMatrix[0];
    size_t nrS = _m_po->GetPOMDPDiscrete()->GetNrStates();
    size_t nrJO = _m_po->GetPOMDPDiscrete()->GetNrJointObservations();
    if( row.size() != nrJO)
    {
        stringstream ss; ss<<"ProcessORow: exected a row matrix with nrJO="<<nrJO<<" entries. Found "<<row.size()<< " entries. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    if( _m_po->_m_lp_toSI[0] == _m_po->_m_anyStateIndex )
    {
        _m_po->_m_lp_toSI.clear();
        for(Index si=0; si < nrS; si++)
            _m_po->_m_lp_toSI.push_back(si);
    }    
    vector<Index>::iterator sI_it = _m_po->_m_lp_toSI.begin();
    vector<Index>::iterator sI_last = _m_po->_m_lp_toSI.end();
    while(sI_it != sI_last)
    {
        Index sucSI = *sI_it;
        vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
        vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
        while(jaI_it != jaI_last)
        {
            Index jaI = *jaI_it;
            for(Index joI=0; joI < nrJO; joI++)
            {
                _m_po->GetPOMDPDiscrete()->SetObservationProbability
                    (jaI, sucSI, joI, row[joI]);
                if(DEBUG_POMDP_PARSE)
                    cout << "Setting O("<<jaI<<","<<sucSI<<","<<joI<< ") = "<<row[joI]<<endl;
            }
            jaI_it++;
        }
        sI_it++;
    }
}

void ParserPOMDPFormat_Spirit::ProcessOMatrix::operator()(iterator_t str, iterator_t end) const
{
    size_t nrS = _m_po->GetPOMDPDiscrete()->GetNrStates();
    size_t nrJO = _m_po->GetPOMDPDiscrete()->GetNrJointObservations();
    if(_m_po->_m_lp_type == DOUBLE)
    {

#if POMDPFORMAT_NEWLINES_SKIPPED
        /* code for POMDP file format, which skips newlines and thus delivers the matrix as a single row...*/
        if(!_m_po->IsDimensionOfMatrix(1,nrS*nrJO))
        {
            stringstream ss; 
            ss << "Expected a 1 x (nrS*nrJO) row matrix."<<" (nrS="<< nrS <<", nrJO= "<< nrJO <<"). At "<<str.get_position()<<endl;
            ss<< "(found matrix of size: " << _m_po->PrintDimensionOfMatrix() << ")" << endl;
            throw E(ss);        
        }
        vector<double>& row_vector =  _m_po->_m_curMatrix.at(0);

        //also need to loop through matchin (joint) actions, since wildcards might have been used
        Index jaI = 0;
        vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
        vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
        while(jaI_it != jaI_last)
        {
            jaI = *jaI_it;
            Index row_vector_index = 0;
            for(Index sucSI=0; sucSI < nrS; sucSI++)
            {
                for(Index joI = 0; joI < nrJO; joI++)
                {
                    double prob = row_vector.at(row_vector_index);     // corresponds to _m_po->_m_curMatrix[sucSI][joI];
                    _m_po->GetPOMDPDiscrete()->SetObservationProbability(jaI, sucSI, joI, prob);
                    row_vector_index++;
                    if(DEBUG_POMDP_PARSE)
                        cout << "Setting O("<<jaI<<","<<sucSI<<","<<joI<<") = "<<prob<<endl;
                }
            }
            jaI_it++;
        }
#else    
        /* code for DPOMDP file which does not skip newline, and therefore has the matrix shaped correctly */

        //an actual matrix was parsed, check curMatrix size
        if(!_m_po->IsDimensionOfMatrix(nrS,nrJO))
        {
            stringstream ss; ss << "Expected a nrS x nrJO matrix."<<" (nrS="<< nrS <<", nrJO="<<nrJO<<"). At "<<str.get_position()<<endl;
            ss<< "(found matrix of size: " << _m_po->PrintDimensionOfMatrix() << ")" << endl;
            throw E(ss);
        }
        Index jaI = 0;
        for(Index joI = 0; joI < nrJO; joI++)
        {
            vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
            vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
            while(jaI_it != jaI_last)
            {
                jaI = *jaI_it;
                for(Index sucSI=0; sucSI < nrS; sucSI++)
                {
                    double prob = _m_po->_m_curMatrix[sucSI][joI];
                    _m_po->GetPOMDPDiscrete()->SetObservationProbability(jaI, sucSI, joI, prob);
                    if(DEBUG_POMDP_PARSE)
                        cout << "Setting O("<<jaI<<","<<sucSI<<","<<joI<<") = "<<prob<<endl;
                }
                jaI_it++;
            }
        }
#endif        
    }
    else if(_m_po->_m_lp_type == UNIFORM)
    {
        double uniform_prob = 1.0 / nrJO;
        for(Index joI = 0; joI < nrJO; joI++)
        {
            vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
            vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
            while(jaI_it != jaI_last)
            {
                Index jaI = *jaI_it;
                for(Index sucSI=0; sucSI < nrS; sucSI++)
                {
                    double prob = uniform_prob;//_m_po->_m_curMatrix[sI][sucSI];
                    _m_po->GetPOMDPDiscrete()->SetObservationProbability
                        (jaI, sucSI, joI, prob);
                    if(DEBUG_POMDP_PARSE)
                        cout << "Setting O("<<jaI<<","<<
                            sucSI<<","<<joI<<") = "<<prob<<endl;
                }
                jaI_it++;
            }
        }
    }
    else
    {
        stringstream ss; ss << "Expected a matrix  or "<<
           "'uniform'. (at "<<str.get_position()<<endl;
        cout << ss.str() << " - _m_po->_m_lp_type = "<<_m_po->_m_lp_type <<endl;
        throw E(ss);
    }
}

void ParserPOMDPFormat_Spirit::ProcessR::operator()(iterator_t str, iterator_t end) const
{
    if(_m_po->_m_lp_type != DOUBLE)
    {
        stringstream ss; ss<<"ProcessR:last parsed type should be a double. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    double reward = _m_po->_m_lp_double;
    if(        _m_po-> _m_lp_fromSI.size() != 1 )
    {
        stringstream ss; ss<<"ProcessR: _m_lp_fromSI has size != 1. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }   
    if(        _m_po-> _m_lp_toSI.size() != 1 )
    {
        stringstream ss; ss<<"ProcessR: _m_lp_toSI has size != 1. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }    
    size_t nrS =  _m_po->GetPOMDPDiscrete()->GetNrStates();
    //size_t nrJO = _m_po->GetPOMDPDiscrete()->GetNrJointObservations();
    Index sucSI = _m_po-> _m_lp_toSI[0];
    Index joI = _m_po->_m_lp_JOI[0];
    if( sucSI == _m_po->_m_anyStateIndex && joI == _m_po->_m_anyJOIndex) 
    {
        // rewards of the form R: ja : s : * : * : %f, so we can simply do
        // AddReward()
    
        if(_m_po->_m_lp_fromSI[0] == _m_po->_m_anyStateIndex)
        {
            _m_po->_m_lp_fromSI.clear();
            for(Index si=0; si < nrS; si++)
                _m_po->_m_lp_fromSI.push_back(si);
        }            
        
        vector<Index>::iterator sI_it = _m_po->_m_lp_fromSI.begin();
        vector<Index>::iterator sI_last = _m_po->_m_lp_fromSI.end();
        while(sI_it != sI_last)
        {
            Index sI = *sI_it;
            vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
            vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
            while(jaI_it != jaI_last)
            {
                Index jaI = *jaI_it;
                _m_po->GetPOMDPDiscrete()->SetReward(sI, jaI, reward);
                if(DEBUG_POMDP_PARSE)
                    cout << "Setting R("<<sI<<","<<jaI<<") = "<<reward<<endl;
                jaI_it++;
            }
            sI_it++;
        }
    }    
    else if( joI == _m_po->_m_anyJOIndex ) //but end-state is not ANY_INDEX
    {
        if( _m_po->_m_lp_fromSI[0] == _m_po->_m_anyStateIndex )
        {
            _m_po->_m_lp_fromSI.clear();
            for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
                _m_po->_m_lp_fromSI.push_back(si);
        }
/*        Obsolete test...
        if( sucSI == ANY_INDEX )
        {
            _m_po->_m_lp_toSI.clear();
            for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
                _m_po->_m_lp_toSI.push_back(si);
        }*/
        vector<Index>::iterator sI_it = _m_po->_m_lp_fromSI.begin();
        vector<Index>::iterator sI_last = _m_po->_m_lp_fromSI.end();
        while(sI_it != sI_last)
        {
            Index sI = *sI_it;
            vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
            vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
            while(jaI_it != jaI_last)
            {
                Index jaI = *jaI_it;
                vector<Index>::iterator sucSI_it = _m_po->_m_lp_toSI.begin();
                vector<Index>::iterator sucSI_last = _m_po->_m_lp_toSI.end();
                while(sucSI_it != sucSI_last)
                {
                    sucSI = *sucSI_it; 
                    _m_po->GetPOMDPDiscrete()->SetReward(sI, jaI, sucSI, reward);
                    if(DEBUG_POMDP_PARSE)
                        cout << "Setting R("<<sI<<","<<jaI<<","<<sucSI<<") = "<<
                            reward<<endl;
                    sucSI_it++;
                }
                jaI_it++;
            }
            sI_it++;
        }

    }
    else // both end-state and joI are specified. joI != ANY_INDEX
         // (otherwise it would fall under previous else if )
    {

        if( _m_po->_m_lp_fromSI[0] == _m_po->_m_anyStateIndex )
        {
            _m_po->_m_lp_fromSI.clear();
            for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
                _m_po->_m_lp_fromSI.push_back(si);
        }
        if( _m_po->_m_lp_toSI[0] == _m_po->_m_anyStateIndex )
        {
            _m_po->_m_lp_toSI.clear();
            for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
                _m_po->_m_lp_toSI.push_back(si);
        }
        vector<Index>::iterator joI_it = _m_po->_m_lp_JOI.begin();
        vector<Index>::iterator joI_last = _m_po->_m_lp_JOI.end();
        while(joI_it != joI_last)
        {
            joI = *joI_it;
            vector<Index>::iterator sI_it = _m_po->_m_lp_fromSI.begin();
            vector<Index>::iterator sI_last = _m_po->_m_lp_fromSI.end();
            while(sI_it != sI_last)
            {
                Index sI = *sI_it;
                vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
                vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
                while(jaI_it != jaI_last)
                {
                    Index jaI = *jaI_it;
                    vector<Index>::iterator sucSI_it = _m_po->_m_lp_toSI.begin();
                    vector<Index>::iterator sucSI_last = _m_po->_m_lp_toSI.end();
                    while(sucSI_it != sucSI_last)
                    {
                        sucSI = *sucSI_it; 
                        _m_po->GetPOMDPDiscrete()->SetReward(sI, jaI, sucSI,joI, 
                            reward);
                        if(DEBUG_POMDP_PARSE)
                            cout << "Setting R("<<sI<<","<<jaI<<","<<sucSI<<
                                ","<<joI<<") = "<<reward<<endl;
                        sucSI_it++;
                    }
                    jaI_it++;
                }
                sI_it++;
            }
            joI_it++;
        }
    } //end else (both end-state and joI are specified)
}

void ParserPOMDPFormat_Spirit::ProcessRRow::operator()(iterator_t str, iterator_t end) const
{
    if(_m_po->_m_lp_type != DOUBLE)
    {
        stringstream ss; ss<<"ProcessRRow:last parsed type should be a double. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }   
    //an actual matrix was parsed, check curMatrix size
    if( _m_po->_m_lp_toSI.size() != 1 )
    {
        stringstream ss; ss<<"ProcessRRow:_m_lp_fromSI has size != 1. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    if( !_m_po->IsRowMatrixLP() )
    {
        stringstream ss; ss<<"ProcessRRow: a row matrix should have been parsed. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }
    vector<double> row = _m_po->_m_curMatrix[0];
    size_t nrS = _m_po->GetPOMDPDiscrete()->GetNrStates();
    size_t nrJO = _m_po->GetPOMDPDiscrete()->GetNrJointObservations();
    if( row.size() != nrJO)
    {
        stringstream ss; ss<<"ProcessRRow: exected a row matrix with nrJO="<<nrJO<<" entries. Found "<<row.size()<< " entries. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }

    if( _m_po->_m_lp_fromSI[0] == _m_po->_m_anyStateIndex )
    {
        _m_po->_m_lp_fromSI.clear();
        for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
            _m_po->_m_lp_fromSI.push_back(si);
    }
    if( _m_po->_m_lp_toSI[0] == _m_po->_m_anyStateIndex )
    {
        _m_po->_m_lp_toSI.clear();
        for(Index si=0; si < nrS; si++)
            _m_po->_m_lp_toSI.push_back(si);
    }    

    vector<Index>::iterator sI_it = _m_po->_m_lp_fromSI.begin();
    vector<Index>::iterator sI_last = _m_po->_m_lp_fromSI.end();
    while(sI_it != sI_last)
    {
        Index sI = *sI_it;
        vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
        vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
        while(jaI_it != jaI_last)
        {
            Index jaI = *jaI_it;
            vector<Index>::iterator sucSI_it = _m_po->_m_lp_toSI.begin();
            vector<Index>::iterator sucSI_last = _m_po->_m_lp_toSI.end();
            while(sucSI_it != sucSI_last)
            {
                Index sucSI = *sucSI_it;
                for(Index joI=0; joI < nrJO; joI++) 
                {
                    _m_po->GetPOMDPDiscrete()->SetReward(sI, jaI, sucSI,joI, 
                        row[joI]);
                    if(DEBUG_POMDP_PARSE)
                        cout << "Setting R("<<sI<<","<<jaI<<","<<sucSI<<
                            ","<<joI<<") = "<<row[joI]<<endl;
                }
                sucSI_it++;
            }
            jaI_it++;
        }
        sI_it++;
    }
}

void ParserPOMDPFormat_Spirit::ProcessRMatrix::operator()(iterator_t str, iterator_t end) const
{
    size_t nrS = _m_po->GetPOMDPDiscrete()->GetNrStates();
    size_t nrJO = _m_po->GetPOMDPDiscrete()->GetNrJointObservations();
    if(_m_po->_m_lp_type != DOUBLE)
    {
        stringstream ss; ss<<"ProcessRMatrix:last parsed type should be a double. (at " <<str.get_position()<<")"<<endl; throw E(ss);
    }   
    //an actual matrix was parsed, check curMatrix size
    if(!_m_po->IsDimensionOfMatrix(nrS,nrJO))
    {
        stringstream ss; ss << "Expected a nrS x nrJO matrix."<<
           " (nrS="<< nrS <<", nrJO="<<nrJO<<"). At "<<str.get_position()<<
           endl;
        throw E(ss);
    }
    if( _m_po->_m_lp_fromSI[0] == _m_po->_m_anyStateIndex )
    {
        _m_po->_m_lp_fromSI.clear();
        for(Index si=0; si < _m_po->GetPOMDPDiscrete()->GetNrStates();si++)
            _m_po->_m_lp_fromSI.push_back(si);
    }
    vector<Index>::iterator sI_it = _m_po->_m_lp_fromSI.begin();
    vector<Index>::iterator sI_last = _m_po->_m_lp_fromSI.end();
    while(sI_it != sI_last)
    {
        Index sI = *sI_it;
        vector<Index>::iterator jaI_it = _m_po->_m_lp_JAI.begin();
        vector<Index>::iterator jaI_last = _m_po->_m_lp_JAI.end();
        while(jaI_it != jaI_last)
        {
            Index jaI = *jaI_it;
            for(Index sucSI=0; sucSI<nrS; sucSI++)
            {
                for(Index joI=0; joI < nrJO; joI++) 
                {
                    _m_po->GetPOMDPDiscrete()->SetReward(sI, jaI, sucSI,joI, 
                        _m_po->_m_curMatrix[sucSI][joI]);
                    if(DEBUG_POMDP_PARSE)
                    {
                        cout << "Setting R(" << sI << "," << jaI <<"," << sucSI;
                        cout << "," << joI << ") = " << 
                            _m_po->_m_curMatrix[sucSI][joI] << endl;
                    }
                }
            }
            jaI_it++;
        }
        sI_it++;
    }
}


}// end namespace POMDPFormatParsing 
