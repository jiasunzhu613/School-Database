#pragma once

#include <fmt/core.h>
#include <string>
#include <unordered_set>

using std::to_string, std::string, std::unordered_set;

// Forward
class Teacher;
// Forward
class Student;

class Course {
   private:
    Teacher* teacher;
    string courseCode;
    int section;
    unordered_set<Student*> students;

   public:
    Course();
    Course(Teacher* teacher, string courseCode, int section);
    Course(Teacher* teacher, string courseCode, int section,
           unordered_set<Student*> students);
    void addStudentToClass(Student* student);
    void addStudentBase(Student* student);
    string getFullCourseCode();
    Teacher* getTeacher();
    unordered_set<Student*> getStudents() const;
};