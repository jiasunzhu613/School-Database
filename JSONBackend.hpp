#include <json/json.h>
#include <json/value.h>
#include <json/writer.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::string, std::vector, std::unordered_set, std::unordered_map,
    std::cout, std::endl, std::ifstream, std::ofstream;

const string FILE_LOCATION = "../school_db.json";

class SchoolDB;
class Student;
class Teacher;
class Course;
class JSONBackend {
   public:
    JSONBackend();
    void load(SchoolDB*);
    void save(SchoolDB*);

   private:
    ifstream infile;
    ofstream outfile;
    Json::Value db;
    Json::Reader jsonReader;
};