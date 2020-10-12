## esmini release notes
The purpose of this document is to highlight relevant changes introducing new features or breaking backward compatibility, i.e. requiring updates to scenarios and start scripts.

#### 2020-10-12 Version 2.0.0
The major functional change is the implementation of the OpenSCENARIO controller concept. A side effect is that much of the former application specific functionality such as Ghost concept, interactive driving control and external control now has moved out from the application(s) core and into different controllers.

Since much of the unique functionality previously implemented in EgoSimulator is moved into controllers, the actual application code gets much simpler and it makes no sense to have different applications for different use cases. Hence we decided to slim down to only two applications: 1. "esmini" which links internal modules statically and have full access to internal API's and 2. "esmini-dyn" demonstrating use of the high-level dynamic shared library.

While doing such major reworks we also took the opportunity to rename applications and some modules, hopefully making things a little bit clearer, at least for newcomers. The changes add and remove features also breaking backward compatibility, hence the update of major version number.

News:
- Support controller concept
- The following controllers, in addition to default controller, have been implemented:
  - ExternalController
      Vehicle will not be updated by the scenario. It's state is expected to be reported via gateway.
  - GhostController
      Vehicle will be launched with some headway time. It can be used as a target by a driver model, e.g. of an external vehicle.
  - FollowGhostController
      An example of a (simple) vehicle and driver model using a Ghost as reference.
  - InteractiveController
      A simple vehicle model controlled by the user via keyboard (arrow keys)
  - SloppyDriverController
      Another example of a driver model, adding some random speed and lateral deviation to the default road following behavior. This controller is useless, it's pure purpose is to provide an example of separating lontitudinal and lateral control.
  - SumoController
      A way to integrate SUMO simulation. OpenSCENARIO vehicles are reported to SUMO, and SUMO vehicles are reported back to esmini.

  Note:
    These controllers now can be utilized also via the shared library.
    These controllers now can be activated/deactivated dynamically by means of OSC triggers

Changes:
  - Special controller modes External, Internal and Hybrid have been removed (replaced by controllers)
  - ScenarioEngineDLL renamed to esminiLib
  - RoadManagerDLL renamed to esminiRMLib
  - EnvironmentSimulator removed (with controllers this got identical with the EgoSimulator)
  - EgoSimulator renamed to esmini (since it now is the recommended application)
  - ScenarioViewer renamed to esmini-dyn (kept as an example of using esminiLib)
  - OpenDriveViewer renamed to odrviewer
  - Replayer renamed to replayer
  - osi_receiver renamed to osireceiver

Note:
  - Previous use of control modes, e.g. external for interactive driving, now has to be specified in terms of controllers in the OpenSCENARIO file. Two steps needed: 1. AssignController and 2. ActivateController. See sloppy-driver.xosc for an example.
  - These changes affects some headerfile names, which might need to be updated in custom code.
  - Scripts referring to executables need updates, e.g. change any "EgoSimulator" or "EnvironmentSimulator" to "esmini"

#### 2020-10-09 Version 1.7.13
- Added support for lane change dynamics by rate

#### 2020-10-06 Version 1.7.12
- Fix pedestrian catalog support

#### 2020-10-02 Version 1.7.11
- Fix bug deleting any entity with defined controller
- Add another OpenDriveViewer example run script

#### 2020-10-02 Version 1.7.10
- Fix assumption that all controllers are SUMO config type
- Fix parameter name bug
- Add OpenDriveViewer to demo package
- Some additional minor bug fixes

##### 2020-10-01 Version 1.7.9
- Improved OSI performance
- Added pedestrian example scenario

##### 2020-09-30 Version 1.7.8
- Support correct parameter names excluding the "$" prefix (old way still supported as well)
- TimeToCollision condition
- Collision condition
- Improved road and lane connectivity w.r.t. preserve direction
- Additional bugfixes and improvements

##### 2020-09-23 Version 1.7.7
- OSI raw struct output option
- OSI sensor view in local coordinates
- Callback mechanism to override (part of) entity states
- Improved WorldCoordinate(x, y, z) to RoadCoordinate(road, lane, offset) mapping
- Many minor bugfixes and improvements (and probably a few new bugs)

##### 2020-09-04 Version 1.7.6
- Added trajectory clothoid support

2020-08-31 Version 1.7.5
- Added Hello World coding example
- Fixed a bug preventing shared library (ScenarioEngineDLL) to run with viewer on Mac
- Fixed crash when running with only OpenDRIVE road description (i.e. without scenegraph 3D model).

##### 2020-08-26 Version 1.7.2
- CSV logging feature
- EndOfRoad trigger
- Unit test framework based on Google Test
- Cleaned irrelevant error messages

##### 2020-08-21 Version 1.7.0
- SUMO support integrated (via libsumostatic), first limited shot.
  - SUMO vehicles created by means of object controller. See example cut-in_sumo.xosc.
  - NOTE that you need to re-run cmake script in order to fetch SUMO dependency package (including headers and pre-built libraries).
- Further OSI values populated, e.g. velocity and acceleration
- OSI trace file not created by default, activated with argument "--osi_file on"
- Condition/trigger timer now based on simulation time instead of system time

##### 2020-07-24 Version 1.6.3
- OSI support extended with road information (lane and road marks)
- Bugfix: RelativeTargetLane (used in LaneChangeAction) is now calculated correctly, skipping reference lane and considering vehicle orientation so that positive lane changes will go left and negative to the right. Scenarios making use of RelativeTargetLane might need to be updated accordingly.

##### 2020-06-18 Version 1.6.0
- OSI support, initial framework established.
- So far population of OSI global groundtruth moving objects.
- OSI data is populated and provided to user in three ways:
    1. OSI trace file (always created, in folder where application was started)
	2. UDP messages (option to send OSI messages to specified host/IP address)
	3. API to fetch OSI data via function call (in ScenarioEngineDLL)
- osi_receiver is a minimalistic demo-application showing how to receive OSI over UDP

##### 2020-05-19 Version 1.5.0
- Updated to support OpenSCENARIO 1.0. Note: no legacy support for 0.9.1
- Updated demo scenarios to v1.0
- Improved condition handling, fully supporting AND (within ConditionGroup) and OR (Multiple ConditionGroups) combinations
- Refactored runtime model supporting StoryBoardElement states, also obeying nrOfExecutions attribute
- Initial Trajectory support. PolyLine only, so far and limited testing performed.
- Added support for TraveledDistance condition

##### 2020-05-05 Version 1.4.6
- Added heading to sensors so that one entity can have multiple sensors in different directions
- Improved position Delta functionality. It will calculate shortest path and distance between two positions more generically and correctly.
- Added road model (multi_intersections.xodr/osgb) with multiple junctions. Useful for testing the shortest path functionality.
- Added new camera mode: top view (press 'k' multiple times to toggle or specify "--camera_mode top" as command line argument).
- Some bugfixes. E.g. ParamPoly3 with Normalized parameter range.

##### 2020-03-25 Version 1.4
- Demos (and binaries) supplied for Mac (Catalina) and Linux (Ubuntu 18.04 and Kubuntu 18.04) in addition to Windows.
- jpeg screenshot. User can save a screen shot at any time by pressing key 'c'.
- QuitAction. User can specify when to quit the scenario (and application) by means of OSC conditions, just as any OSC action.
- Fixed timestep (--fixed_timestep <sec>). Enabling scenarios to be executed in split of a second and, by using the recording feature, it can be replayed in viewer afterwards.
- Threads (--threads). Put scenario execution into a separate thread, decoupled from the viewer. Example of potential use case: Make it possible to pause scenario while moving camera.
- New keyboard shortcut commands, and some moved to new key. Please see run/readme.txt for complete set.

##### 2020-03-08 Version 1.3
- Anti-Alias filter control
  - EgoSimulator now takes argument aa_mode <number of sub samplings>
      0 means no Anti-Alias. 4 is default.
  - One use case is when running esmini (EgoSimulator) within docker on Linux - which seem to not support AntiAlias.
- Mac CI build environment added. However only RoadManager and ScenarioEngine as shared libraries, and without graphics (OSG) support.
- Linux binaries now includes graphics (OSG) support.
- Demo package for Linux added. Now CI builds and deploy demo for both Windows and Linux (Ubuntu 18.04).

##### 2019-12-06 Version 1.2
- Catalog handling updates
  - Parameter assignment implemented (enabling variants or configuration of re-used catalog items)
  - Name and structure updates:
      When a scenario refers to a catalog entry esmini will first locate the catalog by searching all specified catalog directories for a file named "catalog name".xosc. So a catalog name is defined by the base file name. Filename extension .xosc is mandatory. The actual Catalog XML element attribute "name" is ignored.
      See synchronize.xosc for an example on how to (re-)use manuever catalog entries.
- Trail visualization can be switched off by application argument (--trail <on/off>) and toggled by pressing key "t" in viewer window.

##### 2019-11-20 Version 1.1
- External control flag replaced by enumeration
    Available modes: internal, external, hybrid
    EgoSimulator and EnvironmentSimulator argument syntax changed from --ext_control <on|off> to --control <internal|external|hybrid>
