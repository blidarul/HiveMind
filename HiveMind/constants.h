#pragma once

#define DEBUG 1

// map constants
constexpr auto DEBUG_MAP_FILE_PATH = "debug_map.txt";
constexpr auto MAX_MAP_SIZE = 100000; // max width * height

// drone constants
constexpr int DRONE_SPEED = 3;
constexpr int DRONE_MAX_BATTERY = 100;
constexpr int DRONE_BATTERY_CONSUMPTION_RATE = 10;
constexpr int DRONE_OPERATION_COST = 15;
constexpr int DRONE_MAX_CAPACITY = 1;

//robot constants
constexpr int ROBOT_SPEED = 1;
constexpr int ROBOT_MAX_BATTERY = 300;
constexpr int ROBOT_BATTERY_CONSUMPTION_RATE = 2;
constexpr int ROBOT_OPERATION_COST = 1;
constexpr int ROBOT_MAX_CAPACITY = 4;

//scooter constants
constexpr int SCOOTER_SPEED = 2;
constexpr int SCOOTER_MAX_BATTERY = 200;
constexpr int SCOOTER_BATTERY_CONSUMPTION_RATE = 5;
constexpr int SCOOTER_OPERATION_COST = 4;
constexpr int SCOOTER_MAX_CAPACITY = 2;