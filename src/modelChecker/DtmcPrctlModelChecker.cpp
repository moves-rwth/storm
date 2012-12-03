/*
 * DtmcPrctlModelChecker.cpp
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#include "DtmcPrctlModelChecker.h"

namespace mrmc {

namespace modelChecker {

template<class T>
DtmcPrctlModelChecker<T>::DtmcPrctlModelChecker(mrmc::models::Dtmc<T>* dtmc) {
   this->dtmc = dtmc;
}

template<class T>
DtmcPrctlModelChecker<T>::~DtmcPrctlModelChecker() {
   delete this->dtmc;
}


} //namespace modelChecker

} //namespace mrmc
