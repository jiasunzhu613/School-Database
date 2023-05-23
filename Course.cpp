#include "Course.hpp"
#include "Student.hpp"
#include "Teacher.hpp"

Course::Course() {
    this->courseCode = "Invalid Course Default";
    section = 0;
    teacher = nullptr;
    students = unordered_set<Student*>{};
}

Course::Course(Teacher* teacher, string courseCode, int section)
    : teacher{teacher}, courseCode{courseCode}, section{section} {
    students = unordered_set<Student*>{};
    teacher->addToCourse(this);
}

Course::Course(Teacher* teacher, string courseCode, int section,
               unordered_set<Student*> students)
    : teacher{teacher}, courseCode{courseCode}, section{section} {
    teacher->addToCourse(this);
    for (auto student : students) {
        student->addToCourse(this);
    }
}

void Course::addStudentToClass(Student* student) { student->addToCourse(this); }

void Course::addStudentBase(Student* student) { students.insert(student); }

string Course::getFullCourseCode() {
    return fmt::format("{}-{:02}", courseCode, section);
}

Teacher* Course::getTeacher() { return teacher; }
unordered_set<Student*> Course::getStudents() const { return students; }