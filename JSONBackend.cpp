#include "JSONBackend.hpp"

#include "SchoolDB.hpp"

JSONBackend::JSONBackend() {
    infile.open(FILE_LOCATION);
    jsonReader.parse(infile, db);
}

void JSONBackend::load(SchoolDB* inMemoryDB) {
    for (const string id : db["Students"].getMemberNames()) {
        Json::Value studentJSON = db["Students"][id];
        int grade = studentJSON["Grade"].asInt();
        int numLates = studentJSON["Number of Lates"].asInt();
        string firstName = studentJSON["First Name"].asString();
        string lastName = studentJSON["Last Name"].asString();
        string address = studentJSON["Address"].asString();
        if (address == "")
            inMemoryDB->students.insert(std::make_pair(
                id, Student{firstName, lastName, grade, id, numLates}));
        else
            inMemoryDB->students.insert(std::make_pair(
                id,
                Student{firstName, lastName, address, grade, id, numLates}));
    }

    for (const string id : db["Teachers"].getMemberNames()) {
        Json::Value teacherJSON = db["Teacher"][id];
        string firstName = teacherJSON["First Name"].asString();
        string lastName = teacherJSON["Last Name"].asString();
        string teachables = teacherJSON["Teachables"].asString();
        string address = teacherJSON["Address"].asString();
        if (address == "")
            inMemoryDB->teachers.insert(std::make_pair(
                id, Teacher{firstName, lastName, teachables, id}));
        else
            inMemoryDB->teachers.insert(std::make_pair(
                id, Teacher{firstName, lastName, address, teachables, id}));
    }

    for (const string courseCode : db["Courses"].getMemberNames()) {
        Json::Value courseJSON = db["Courses"][courseCode];
        int hyphen = courseCode.find("-");
        string courseCodeNoSection = courseCode.substr(0, hyphen);
        int section = std::stoi(courseCode.substr(hyphen + 1));
        Teacher* teacher =
            &inMemoryDB->teachers.at(courseJSON["Teacher"].asString());

        unordered_set<Student*> students{};
        for (int i = 0; i < courseJSON["Students"].size(); ++i)
            students.insert(
                &inMemoryDB->students.at(courseJSON["Students"][i].asString()));
        inMemoryDB->courses.insert(std::make_pair(
            courseCode,
            Course{teacher, courseCodeNoSection, section, students}));
    }

    for (auto [id, student] : inMemoryDB->students)
        for (int i = 0; i < db["Students"][id]["Courses"].size(); ++i)
            student.addToCourse(&inMemoryDB->courses.at(
                db["Students"][id]["Courses"][i].asString()));

    for (auto [id, teacher] : inMemoryDB->teachers)
        for (int i = 0; i < db["Teachers"][id]["Courses"].size(); ++i)
            teacher.addToCourse(&inMemoryDB->courses.at(
                db["Teachers"][id]["Courses"][i].asString()));
}

void JSONBackend::save(SchoolDB* inMemoryDB) {
    Json::Value ndb;

    for (auto [id, student] : inMemoryDB->students) {
        Json::Value& studentJSON = ndb["Students"][id];
        studentJSON["Grade"] = student.getGrade();
        studentJSON["Number of Lates"] = student.getNumLates();
        studentJSON["First Name"] = student.getFirstName();
        studentJSON["Last Name"] = student.getLastName();
        studentJSON["Address"] = student.getAddress();
        studentJSON["Courses"] = Json::arrayValue;
        for (Course* course : student.getCourses()) {
            string s = course->getFullCourseCode();
            studentJSON["Courses"].append(s);
        }
    }

    for (auto [id, teacher] : inMemoryDB->teachers) {
        Json::Value& teacherJSON = ndb["Teachers"][id];
        teacherJSON["Teachables"] = teacher.getTeachables();
        teacherJSON["First Name"] = teacher.getFirstName();
        teacherJSON["Last Name"] = teacher.getLastName();
        teacherJSON["Address"] = teacher.getAddress();
        teacherJSON["Courses"] = Json::arrayValue;
        for (Course* course : teacher.getCourses()) {
            teacherJSON["Courses"].append(course->getFullCourseCode());
        }
    }

    for (auto [id, course] : inMemoryDB->courses) {
        Json::Value& courseJSON = ndb["Courses"][id];
        courseJSON["Teacher"] = course.getTeacher()->getEmployeeId();
        for (const Student* student : course.getStudents()) {
            courseJSON["Students"].append(student->getStudentId());
        }
    }

    db = ndb;

    cout << db << endl;
    outfile.open(FILE_LOCATION);
    outfile << db;
    outfile.close();
}