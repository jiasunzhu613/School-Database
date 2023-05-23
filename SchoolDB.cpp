#include "SchoolDB.hpp"

SchoolDB::SchoolDB() {
    JSONBackend jsonDB{};
    // jsonDB.load(this);
}

unordered_map<string, Teacher> SchoolDB::getTeachers() { return teachers; }
unordered_map<string, Student> SchoolDB::getStudents() { return students; }
unordered_map<string, Course> SchoolDB::getCourses() { return courses; }

void SchoolDB::reset() { jsonDB.load(this); }
void SchoolDB::save() { jsonDB.save(this); }

void SchoolDB::addStudent(Student student) {
    students[student.getStudentId()] = student;
}
void SchoolDB::addTeacher(Teacher teacher) {
    teachers[teacher.getEmployeeId()] = teacher;
}
void SchoolDB::addCourse(Course course) {
    courses[course.getFullCourseCode()] = course;
}