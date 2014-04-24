


#pragma once
#include "storage/prism/Program.h"
#include "models/AbstractModel.h"
#include "storage/parameters.h"

namespace storm
{
	class ParametricStormEntryPoint
	{
	private:
		std::string const& mConstants;
		storm::prism::Program const& mProgram;
		std::shared_ptr<storm::models::AbstractModel<RationalFunction>> mModel;
	public:
		ParametricStormEntryPoint(std::string const& constants, storm::prism::Program const& program) :
		mConstants(constants),
		mProgram(program)
		{
			
		}
		
		void createModel();
		std::string reachabilityToSmt2(std::string const&);
		
		virtual ~ParametricStormEntryPoint() {}
		
	};
	void storm_parametric(std::string const& constants, storm::prism::Program const&);
}

