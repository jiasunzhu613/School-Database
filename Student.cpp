#include "Student.hpp"

Student::Student(){};
Student::Student(string first, string last)
    : Person{first, last}, numLates{0}, courses{} {}
Student::Student(string f, string l, string a, int g, string id, int lates)
    : Person{f, l, a}, grade{g}, studentId{id}, numLates{lates}, courses{} {}

Student::Student(string f, string l, int g, string id, int lates)
    : Person{f, l}, grade{g}, studentId{id}, numLates{lates}, courses{} {}

int Student::getGrade() { return grade; }
string Student::getStudentId() const { return studentId; }
int Student::getNumLates() { return numLates; }
void Student::setGrade(int n) { grade = n; }
void Student::setStudentId(string id) {
    if (isValidId(id))
        studentId = id;
    else
        studentId = "invalid id";
}
void Student::addLate() { numLates++; }
bool Student::isValidId(string id) {
    const int LENGTH_OF_VALID_IDS = 10;
    const char FIRST_CHAR = 'S';

    if (id.length() != LENGTH_OF_VALID_IDS) return false;
    if (id[0] != FIRST_CHAR) return false;
    for (int i = 1; i < LENGTH_OF_VALID_IDS; ++i)
        if (id[i] < 48 || id[i] > 57) return false;

    return true;
}

void Student::addToCourse(Course* course) {
    courses.insert(course);
    course->addStudentBase(this);
}

const unordered_set<Course*>& Student::getCourses() { return courses; }

string Student::toString() {
    std::stringstream ss;
    ss << "First name: " << getFirstName() << std::endl;
    ss << "Last name: " << getLastName() << std::endl;
    ss << "Address: " << getAddress() << std::endl;
    ss << "Grade: " << getGrade() << std::endl;
    ss << "Student ID: " << getStudentId() << std::endl;
    ss << "Number of lates: " << getNumLates() << std::endl;
    return ss.str();
}