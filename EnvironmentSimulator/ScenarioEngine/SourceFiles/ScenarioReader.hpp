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
#include "RoadNetwork.hpp"
#include "Catalogs.hpp"
#include "Entities.hpp"
#include "Init.hpp"
#include "Story.hpp"
#include "OSCPosition.hpp"
#include "OSCProperties.hpp"
#include "pugixml.hpp"
#include "OSCGlobalAction.hpp"
#include "OSCBoundingBox.hpp"
#include "Parameters.hpp"
//#include "Controller.hpp"
#include "ScenarioGateway.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace scenarioengine
{

	class ScenarioReader
	{
	public:

		ScenarioReader(Entities *entities, Catalogs *catalogs, bool disable_controllers = false) : 
			objectCnt_(0), entities_(entities), catalogs_(catalogs), disable_controllers_(disable_controllers) {}
		~ScenarioReader();
		int loadOSCFile(const char * path);
		void loadOSCMem(const pugi::xml_document &xml_doch);
		void SetGateway(ScenarioGateway* gateway) { gateway_ = gateway; }

		int RegisterCatalogDirectory(pugi::xml_node catalogDirChild);

		// RoadNetwork
		void parseRoadNetwork(RoadNetwork &roadNetwork);
		void parseOSCFile(OSCFile &file, pugi::xml_node fileNode);
		roadmanager::Trajectory* parseTrajectory(pugi::xml_node node);

		// Catalogs
		void parseCatalogs();
		Catalog* LoadCatalog(std::string name);
		roadmanager::Route* parseOSCRoute(pugi::xml_node routeNode);
		void ParseOSCProperties(OSCProperties &properties, pugi::xml_node &xml_node);
		void ParseOSCBoundingBox(OSCBoundingBox &boundingbox, pugi::xml_node &xml_node);
		Vehicle* parseOSCVehicle(pugi::xml_node vehicleNode);
		Pedestrian* parseOSCPedestrian(pugi::xml_node pedestrianNode);
		MiscObject* parseOSCMiscObject(pugi::xml_node miscObjectNode);
		Vehicle* createRandomOSCVehicle(std::string name);
		Controller* parseOSCObjectController(pugi::xml_node vehicleNode);
		void parseGlobalParameterDeclarations() { parameters.parseGlobalParameterDeclarations(&doc_); }

		// Enitites
		int parseEntities();
		Entry* ResolveCatalogReference(pugi::xml_node node);
		Object* FindObjectByName(std::string name);

		// Storyboard - Init
		void parseInit(Init &init);
		OSCPrivateAction *parseOSCPrivateAction(pugi::xml_node actionNode, Object *object);
		OSCGlobalAction *parseOSCGlobalAction(pugi::xml_node actionNode);
		void parseOSCOrientation(OSCOrientation &orientation, pugi::xml_node orientationNode);
		OSCPosition *parseOSCPosition(pugi::xml_node positionNode);

		// Storyboard - Story
		OSCCondition *parseOSCCondition(pugi::xml_node conditionNode);
		Trigger* parseTrigger(pugi::xml_node triggerNode);
		//	void parseOSCConditionGroup(OSCConditionGroup *conditionGroup, pugi::xml_node conditionGroupNode);
		int parseStoryBoard(StoryBoard &storyBoard);
		void parseOSCManeuver(OSCManeuver *maneuver, pugi::xml_node maneuverNode, ManeuverGroup *mGroup);

		std::string getScenarioFilename() { return oscFilename_; }

		std::vector<Controller*> controller_;

	private:
		pugi::xml_document doc_;
		int objectCnt_;
		std::string oscFilename_;
		Entities *entities_;
		Catalogs *catalogs_;
		Parameters parameters;
		ScenarioGateway* gateway_;
		bool disable_controllers_;

		int ParseTransitionDynamics(pugi::xml_node node, OSCPrivateAction::TransitionDynamics& td);
		ConditionGroup* ParseConditionGroup(pugi::xml_node node);
	};

}
