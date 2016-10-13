//
// Created by kyle on 10/12/16.
//

#ifndef TANSA_JOCSPARSER_H
#define TANSA_JOCSPARSER_H
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "json.hpp"
#include "tansa/vehicle.h"
#include "tansa/core.h"
#include "tansa/trajectory.h"

//PLACEHOLDER CHOREOGRAPHY CLASS
class Choreography{Choreography(){}};
//Placeholder
struct Drone{
	Drone(Point p, unsigned droneId): startingPoint(p), id(droneId){}
	Point startingPoint;
	unsigned id;
};
//Static Jocs parsing class for reading actions and drones out of a jocs file
class Jocs{
public:
	static const std::string HOME_KEY;
	static const std::string DRONE_KEY;
	static Choreography Parse(const std::string& jocsPath);
private:
	static std::vector<Vehicle> parseVehicles(const nlohmann::json& data);
	//placeholder template. This should return action but dont' have action class yet. Was complaining about
	//empty class as template so just used int.
	static std::vector<int> parseActions(const nlohmann::json& data);
	static Drone parseDrone(nlohmann::json::reference data);
};
#endif //TANSA_JOCSPARSER_H
