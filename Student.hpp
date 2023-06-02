#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <unordered_set>

#include "Course.hpp"
#include "Person.hpp"
#include "imgui.h"
using std::string, std::unordered_set, std::function;

typedef enum {
    SORT_ID,
    SORT_FirstName,
    SORT_LastName,
    SORT_Grade,
    SORT_NumLates,
} student_sort_t;

typedef std::function<int(const Student*, const Student*)> studentComparator;

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

    int getGrade() const;
    int getNumLates() const;
    void setGrade(int n);
    void setStudentId(string id);
    string getStudentId() const;
    void addLate();
    bool isValidId(string id);
    void addToCourse(Course* course);
    const unordered_set<Course*>& getCourses();
    string toString();

    static int idCompare(const Student*, const Student*);
    static int firstNameCompare(const Student*, const Student*);
    static int lastNameCompare(const Student*, const Student*);
    static int gradeCompare(const Student*, const Student*);
    static int numLatesCompare(const Student*, const Student*);
    static studentComparator CompareWithSortSpecs(ImGuiTableSortSpecs*);
};