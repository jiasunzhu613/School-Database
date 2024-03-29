#include "Teacher.hpp"
Teacher::Teacher() {}
Teacher::Teacher(string first, string last) : Person{first, last}, courses{} {}
Teacher::Teacher(string f, string l, string a, string t, string id, string pw)
    : Person{f, l, a}, teachables{t}, employeeId{id}, password{pw}, courses{} {}
Teacher::Teacher(string f, string l, string t, string id, string pw)
    : Teacher{f, l, "", t, id, pw} {}

string Teacher::getTeachables() { return teachables; }

void Teacher::setTeachables(string t) { teachables = t; }

void Teacher::setEmployeeId(string id) {
    if (isValidId(id))
        employeeId = id;
    else
        employeeId = "invalid id";
}
string Teacher::getEmployeeId() const { return employeeId; }

void Teacher::setPassword(string pw) {
    password = pw;
}
string Teacher::getPassword() const { return password; }

bool Teacher::isValidId(string id) {
    const int LENGTH_OF_VALID_IDS = 6;
    const char FIRST_CHAR = 'C';

    if (id.length() != LENGTH_OF_VALID_IDS) return false;
    if (id[0] != FIRST_CHAR) return false;
    for (int i = 1; i < LENGTH_OF_VALID_IDS; ++i)
        if (id[i] < 48 || id[i] > 57) return false;

    return true;
}

void Teacher::markStudentLate(Student& s) { s.addLate(); }

string Teacher::toString() {
    return "Hi, I'm a teacher and my name is " + getFirstName() + " " +
           getLastName() + " and I live at " + getAddress() + ". My ID is " +
           employeeId;
}
const unordered_set<Course*>& Teacher::getCourses() { return courses; }
void Teacher::addToCourse(Course* course) { courses.insert(course); }
void Teacher::setCourses(unordered_set<Course*> c) { courses = c; }