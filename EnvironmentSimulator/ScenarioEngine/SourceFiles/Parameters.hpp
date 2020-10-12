/* 
 * esmini - Environment Simulator Minimalistic 
 * https://github.com/esmini/esmini
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) partners of Simulation Scenarios
 * https://sites.google.com/view/simulationscenarios
 */

#pragma once

#include <string.h>
#include "pugixml.hpp"
#include "OSCParameterDeclarations.hpp"
#include <vector>

namespace scenarioengine
{
	// base class for controllers
	class Parameters
	{
	public:
		Parameters() : paramDeclarationsSize_(0) {}
		int paramDeclarationsSize_;  // original size, exluding added parameters
		std::vector<ParameterStruct> catalog_param_assignments;
		OSCParameterDeclarations parameterDeclarations_;

		// ParameterDeclarations
		void parseGlobalParameterDeclarations(pugi::xml_document* doc_);
		void parseParameterDeclarations(pugi::xml_node xml_node, OSCParameterDeclarations* pd);
		std::string getParameter(OSCParameterDeclarations& parameterDeclarations, std::string name);
		void addParameter(std::string name, std::string value);
		void addParameterDeclarations(pugi::xml_node xml_node);
		void RestoreParameterDeclarations();  // To what it was before addParameterDeclarations

		// Use always this method when reading attributes, it will resolve any variables
		std::string ReadAttribute(pugi::xml_node, std::string attribute, bool required = false);
	};
}