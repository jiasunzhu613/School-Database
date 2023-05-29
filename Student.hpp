#pragma once

#include <string>
#include <unordered_set>
#include <sstream>

#include "Course.hpp"
#include "Person.hpp"
using std::string, std::unordered_set;

class Student : public Person {
   private:
    int grade;
    string studentId;
    int numLates;
    unordered_set<Course*> courses;

   public:
    Student();
    Student(string first, string last);
    Student(string f, string l, string a, int g, string id, int lates = 0);
    Student(string f, string l, int g, string id, int lates = 0);

    int getGrade();
    int getNumLates();
    void setGrade(int n);
    void setStudentId(string id);
    string getStudentId() const;
    void addLate();
    bool isValidId(string id);
    void addToCourse(Course* course);
    const unordered_set<Course*>& getCourses();
    string toString();
};