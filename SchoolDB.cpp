#include "SchoolDB.hpp"

SchoolDB::SchoolDB() {
    JSONBackend jsonDB{};
    // jsonDB.load(this);
}

unordered_map<string, Teacher>& SchoolDB::getTeachers() { return teachers; }
unordered_map<string, Student>& SchoolDB::getStudents() { return students; }
unordered_map<string, Course>& SchoolDB::getCourses() { return courses; }

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

void SchoolDB::addCourse(Teacher* teacher, string courseCode, int section) {
    courses[courseCode] = Course{teacher, courseCode, section};
}

void SchoolDB::addCourse(string teacher, string courseCode, int section) {
    courses[courseCode] = Course{&teachers[teacher], courseCode, section};
}

string SchoolDB::log(bool update) {
    static stringstream ss{};
    if (update) {
        ss.str("");
        ss.clear();
        Json::Value ndb;

        for (auto [id, student] : students) {
            Json::Value& studentJSON = ndb["Students"][id];
            studentJSON["Grade"] = student.getGrade();
            studentJSON["Number of Lates"] = student.getNumLates();
            studentJSON["First Name"] = student.getFirstName();
            studentJSON["Last Name"] = student.getLastName();
            studentJSON["Address"] = student.getAddress();
            studentJSON["Courses"] = Json::arrayValue;
            for (Course* course : students[id].getCourses()) {
                string s = course->getFullCourseCode();
                studentJSON["Courses"].append(s);
            }
        }

        for (auto [id, teacher] : teachers) {
            Json::Value& teacherJSON = ndb["Teachers"][id];
            teacherJSON["Teachables"] = teacher.getTeachables();
            teacherJSON["Password"] = teacher.getPassword();
            teacherJSON["First Name"] = teacher.getFirstName();
            teacherJSON["Last Name"] = teacher.getLastName();
            teacherJSON["Address"] = teacher.getAddress();
            teacherJSON["Courses"] = Json::arrayValue;
            for (Course* course : teacher.getCourses()) {
                teacherJSON["Courses"].append(course->getFullCourseCode());
            }
        }

        for (auto [id, course] : courses) {
            Json::Value& courseJSON = ndb["Courses"][id];
            courseJSON["Teacher"] = course.getTeacher()->getEmployeeId();
            for (const Student* student : course.getStudents()) {
                courseJSON["Students"].append(student->getStudentId());
            }
        }

        ss << ndb;
    }
    return ss.str();
}