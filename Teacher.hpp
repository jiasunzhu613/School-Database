#pragma once

#include <string>
#include <unordered_set>

#include "Course.hpp"
#include "Person.hpp"
#include "Student.hpp"
using std::string, std::unordered_set;

class Teacher : public Person {
   private:
    string teachables;
    string employeeId;
    unordered_set<Course*> courses;

   public:
    Teacher();
    Teacher(string first, string last);
    Teacher(string f, string l, string t, string id);
    Teacher(string f, string l, string a, string t, string id);
    Teacher(const Teacher&);
    Teacher& operator=(const Teacher&);
    void setTeachables(string t);
    string getTeachables();
    void setEmployeeId(string id);
    string getEmployeeId() const;
    bool isValidId(string id);
    void markStudentLate(Student& s);
    string toString();
    const unordered_set<Course*>& getCourses();
    void addToCourse(Course* course);
    void setCourses(unordered_set<Course*>);
};