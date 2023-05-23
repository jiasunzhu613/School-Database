#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "Course.hpp"
#include "JSONBackend.hpp"
#include "Person.hpp"
#include "Student.hpp"
#include "Teacher.hpp"
using std::unordered_map, std::string;

class SchoolDB {
   private:
    unordered_map<string, Teacher> teachers;
    unordered_map<string, Student> students;
    unordered_map<string, Course> courses;
    JSONBackend jsonDB;

   public:
    SchoolDB();
    unordered_map<string, Teacher> getTeachers();
    unordered_map<string, Student> getStudents();
    unordered_map<string, Course> getCourses();

    void addStudent(Student student);
    void addTeacher(Teacher teacher);
    void addCourse(Course course);

    void log();

    void reset();
    void save();

    friend void JSONBackend::load(SchoolDB*);
    friend void JSONBackend::save(SchoolDB*);
};