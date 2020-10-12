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

 /*
  * The purpose of this application is to support development of the RoadManager by visualizing the road network and moving objects on top.
  * Bascially it loads an OpenDRIVE file, and optionally a corresponding 3D model, and then populate vehicles at specified density. The 
  * vehicles will simply follow it's lane until a potential junction where the choice of route is randomized.
  *
  * The application can be used both to debug the RoadManager and to check OpenDRIVE files, e.g. w.r.t. gemoetry, lanes and connectivity.
  *
  * New road/track segments is indicated by a yellow large dot. Geometry segments within a road are indicated by red dots.
  * Red line is the reference lane, blue lines shows drivable lanes. Non-drivable lanes are currently not indicated. 
  */

#include <random>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "viewer.hpp"
#include "RoadManager.hpp"
#include "vehicle.hpp"
#include "CommonMini.hpp"


#define DEFAULT_SPEED   70  // km/h
#define DEFAULT_DENSITY 1   // Cars per 100 m
#define ROAD_MIN_LENGTH 30
#define SIGN(X) ((X<0)?-1:1)


static bool run_only_once = false;
static const double stepSize = 0.01;
static const double maxStepSize = 0.1;
static const double minStepSize = 0.01;
static const bool freerun = true;
static std::mt19937 mt_rand;
static double density = DEFAULT_DENSITY;
static double speed = DEFAULT_SPEED;
static double global_speed_factor = 1.0;
static int first_car_in_focus = -1;

double deltaSimTime;  // external - used by Viewer::RubberBandCamera

typedef struct
{
	int road_id_init;
	int lane_id_init;
	double heading_init;
	double s_init;
	roadmanager::Position *pos;
	double speed_factor;  // speed vary bewtween lanes, m/s
	viewer::CarModel *model;
	int id;
} Car;

std::vector<Car*> cars;

// Car models used for populating the road network
// path should be relative the OpenDRIVE file
static const char* carModelsFiles_[] =
{
	"car_white.osgb",
	"car_blue.osgb",
	"car_red.osgb",
	"car_yellow.osgb",
	"truck_yellow.osgb",
	"van_red.osgb",
	"bus_blue.osgb",
};

std::vector<osg::ref_ptr<osg::LOD>> carModels_;


void log_callback(const char *str)
{
	printf("%s\n", str);
}

int SetupCars(roadmanager::OpenDrive *odrManager, viewer::Viewer *viewer)
{
	if (density < 1E-10)
	{
		// Basically no scenario vehicles
		return 0;
	}

	for (int r = 0; r < odrManager->GetNumOfRoads(); r++)
	{
		roadmanager::Road *road = odrManager->GetRoadByIdx(r);
		double average_distance = 100.0 / density;

		if (road->GetLength() > ROAD_MIN_LENGTH)
		{
			// Populate road lanes with vehicles at some random distances
			for (double s = 10; s < road->GetLength() - average_distance; s += average_distance + 0.2 * average_distance * mt_rand() / mt_rand.max())
			{
				// Pick lane by random
				int lane_idx = ((double)road->GetNumberOfDrivingLanes(s) * mt_rand()) / (mt_rand.max)();
				roadmanager::Lane *lane = road->GetDrivingLaneByIdx(s, lane_idx);
				if (lane == 0)
				{
					LOG("Failed locate driving lane %d at s %d", lane_idx, s);
					continue;
				}


				if ((SIGN(lane->GetId()) < 0) && (road->GetLength() - s < 50) && (road->GetLink(roadmanager::LinkType::SUCCESSOR) == 0) ||
					(SIGN(lane->GetId()) > 0) && (s < 50) && (road->GetLink(roadmanager::LinkType::PREDECESSOR) == 0))
				{
					// Skip vehicles too close to road end - and where connecting road is missing
					continue;
				}

				// randomly choose model
				int carModelID = (double(sizeof(carModelsFiles_) / sizeof(carModelsFiles_[0])) * mt_rand()) / (mt_rand.max)();
				LOG("Adding car of model %d to road nr %d (road id %d s %.2f lane id %d), ", carModelID, r, road->GetId(), s, lane->GetId());

				Car *car_ = new Car;
				// Higher speeds in lanes closer to reference lane
				car_->speed_factor = 0.5 + 0.5 / abs(lane->GetId());  // Speed vary between 0.5 to 1.0 times default speed
				car_->road_id_init = odrManager->GetRoadByIdx(r)->GetId();
				car_->lane_id_init = lane->GetId();
				car_->s_init = s;
				car_->pos = new roadmanager::Position(odrManager->GetRoadByIdx(r)->GetId(), lane->GetId(), s, 0);
				car_->pos->SetHeadingRelative(lane->GetId() < 0 ? 0 : M_PI);
				car_->heading_init = car_->pos->GetHRelative();

				if ((car_->model = viewer->AddCar(carModelsFiles_[carModelID], false, osg::Vec3(0.5, 0.5, 0.5), false,"")) == 0)
				{
					return -1;
				}
				car_->id = cars.size();
				cars.push_back(car_);
				if (first_car_in_focus == -1 && lane->GetId() < 0)
				{
					first_car_in_focus = car_->id;
				}

			}
		}
	}

	if (first_car_in_focus == -1)
	{
		first_car_in_focus = 0;
	}

	return 0;
}

void updateCar(roadmanager::OpenDrive *odrManager, Car *car, double deltaSimTime)
{
	double speed = car->pos->GetSpeedLimit() * car->speed_factor * global_speed_factor;
	double ds = speed * deltaSimTime; // right lane is < 0 in road dir;

	if (car->pos->MoveAlongS(ds) != 0)
	{
		// Start from beginning of road - not initial s-position
		double start_s = 5;
		if (car->lane_id_init > 0)
		{
			start_s = odrManager->GetRoadById(car->road_id_init)->GetLength() - 5;
		}
		car->pos->SetLanePos(car->road_id_init, car->lane_id_init, start_s, 0, 0);
		car->pos->SetHeadingRelative(car->heading_init);
	}

	if (car->model->txNode_ != 0)
	{
		car->model->txNode_->setPosition(osg::Vec3(car->pos->GetX(), car->pos->GetY(), car->pos->GetZ()));

		car->model->quat_.makeRotate(
			car->pos->GetR(), osg::Vec3(1, 0, 0),
			car->pos->GetP(), osg::Vec3(0, 1, 0),
			car->pos->GetH(), osg::Vec3(0, 0, 1));

		car->model->txNode_->setAttitude(car->model->quat_);
	}
}

int main(int argc, char** argv)
{
	// Use logger callback
	Logger::Inst().SetCallback(log_callback);

	mt_rand.seed((unsigned int)time(0));

	// use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);	

    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName());
	arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName() + " [options]\n");
	arguments.getApplicationUsage()->addCommandLineOption("--odr <filename>", "OpenDRIVE filename");
	arguments.getApplicationUsage()->addCommandLineOption("--model <filename>", "3D model filename");
	arguments.getApplicationUsage()->addCommandLineOption("--density <number>", "density (cars / 100 m)", std::to_string((long long) (DEFAULT_DENSITY)));
	arguments.getApplicationUsage()->addCommandLineOption("--speed_factor <number>", "speed factor", std::to_string((long long) (global_speed_factor)));
	arguments.getApplicationUsage()->addCommandLineOption("--osi_features <string>", "Show OSI road features (\"on\"/\"off\") (toggle during simulation with key 'u')", "off");


	if (arguments.argc() < 2)
	{
		arguments.getApplicationUsage()->write(std::cout, 1, 120, true);
		return -1;
	}

	std::string odrFilename;
	arguments.read("--odr", odrFilename);

	std::string modelFilename;
	arguments.read("--model", modelFilename);

	arguments.read("--density", density);
	printf("density: %.2f\n", density);

	arguments.read("--speed_factor", global_speed_factor);
	printf("global speed factor: %.2f\n", global_speed_factor);

	std::string osi_features_str;
	arguments.read("--osi_features", osi_features_str);
	bool osi_features = false;
	if (osi_features_str == "on")
	{
		osi_features = true;
	}
	else if (osi_features_str == "off")
	{
		osi_features = false;
	}
	else if (osi_features_str != "")
	{
		printf("Unexpected osi_features value: %s\n", osi_features_str.c_str());
		arguments.getApplicationUsage()->write(std::cout, 1, 120, true);
		return -1;
	}
	printf("osi_features: %s\n", osi_features ? "on" : "off");

	roadmanager::Position *lane_pos = new roadmanager::Position();
	roadmanager::Position *track_pos = new roadmanager::Position();

	try
	{
		if (!roadmanager::Position::LoadOpenDrive(odrFilename.c_str()))
		{
			printf("Failed to load ODR %s\n", odrFilename.c_str());
			return -1;
		}
		roadmanager::OpenDrive *odrManager = roadmanager::Position::GetOpenDrive();

		viewer::Viewer *viewer = new viewer::Viewer(
			odrManager,
			modelFilename.c_str(),
			NULL,
			argv[0],
			arguments);

		viewer->ShowOSIFeatures(osi_features);

		if (SetupCars(odrManager, viewer) == -1)
		{
			return 4;
		}
		printf("%d cars added\n", (int)cars.size());
		viewer->SetVehicleInFocus(first_car_in_focus);

		__int64 now, lastTimeStamp = 0;

		static bool first_time = true;

		while (!viewer->osgViewer_->done())
		{
			// Get milliseconds since Jan 1 1970
			now = SE_getSystemTime();
			deltaSimTime = (now - lastTimeStamp) / 1000.0;  // step size in seconds
			lastTimeStamp = now;
			if (deltaSimTime > maxStepSize) // limit step size
			{
				deltaSimTime = maxStepSize;
			}
			else if (deltaSimTime < minStepSize)  // avoid CPU rush, sleep for a while
			{
				SE_sleep(now - lastTimeStamp);
				deltaSimTime = minStepSize;
			}

			if (!(run_only_once && !first_time))
			{
				for (size_t i = 0; i < cars.size(); i++)
				{
					updateCar(odrManager, cars[i], deltaSimTime);
				}
				first_time = false;
			}

			viewer->osgViewer_->frame();
		}
		delete viewer;
	}
	catch (std::logic_error &e)
	{
		printf("%s\n", e.what());
		return 2;
	}
	catch (std::runtime_error &e)
	{
		printf("%s\n", e.what());
		return 3;
	}

	for (size_t i = 0; i < cars.size(); i++)
	{
		delete(cars[i]);
	}
	delete track_pos;
	delete lane_pos;

	return 0;
}
