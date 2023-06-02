#include "Student.hpp"

Student::Student(){};
Student::Student(string first, string last)
    : Person{first, last}, numLates{0}, courses{} {}
Student::Student(string f, string l, string a, int g, string id, int lates)
    : Person{f, l, a}, grade{g}, studentId{id}, numLates{lates}, courses{} {}

Student::Student(string f, string l, int g, string id, int lates)
    : Person{f, l}, grade{g}, studentId{id}, numLates{lates}, courses{} {}

int Student::getGrade() const { return grade; }
string Student::getStudentId() const { return studentId; }
int Student::getNumLates() const { return numLates; }
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

int Student::idCompare(const Student* s1, const Student* s2) {
    return s1->getStudentId().compare(s2->getStudentId());
}
int Student::firstNameCompare(const Student* s1, const Student* s2) {
    return s1->getFirstName().compare(s2->getFirstName());
}
int Student::lastNameCompare(const Student* s1, const Student* s2) {
    return s1->getLastName().compare(s2->getLastName());
}

int Student::gradeCompare(const Student* s1, const Student* s2) {
    if (s1->getGrade() > s2->getGrade()) return 1;
    if (s1->getGrade() < s2->getGrade()) return -1;
    return 0;
}
int Student::numLatesCompare(const Student* s1, const Student* s2) {
    if (s1->getNumLates() > s2->getNumLates()) return 1;
    if (s1->getNumLates() < s2->getNumLates()) return -1;
    return 0;
}

studentComparator invert(studentComparator c) {
    return [c](const Student* s1, const Student* s2) { return -1 * c(s1, s2); };
}

studentComparator comp(studentComparator c1, studentComparator c2) {
    return [c1, c2](const Student* s1, const Student* s2) {
        int x = c1(s1, s2);
        if (x == 0)
            return x;
        else
            return c2(s1, s2);
    };
}

studentComparator Student::CompareWithSortSpecs(
    ImGuiTableSortSpecs* sortSpecs) {
    studentComparator s = [](const Student* s1, const Student* s2) {
        return 0;
    };

    for (int i = 0; i < sortSpecs->SpecsCount; ++i) {
        const ImGuiTableColumnSortSpecs* sort_spec = &sortSpecs->Specs[i];

        studentComparator new_s = [](const Student* s1, const Student* s2) {
            return 0;
        };
        switch (sort_spec->ColumnUserID) {
            case SORT_ID:
                new_s = idCompare;
                break;
            case SORT_FirstName:
                new_s = firstNameCompare;
                break;
            case SORT_LastName:
                new_s = lastNameCompare;
                break;
            case SORT_Grade:
                new_s = gradeCompare;
                break;
            case SORT_NumLates:
                new_s = numLatesCompare;
                break;
        }
        if (sort_spec->SortDirection == ImGuiSortDirection_Descending)
            new_s = invert(new_s);

        s = comp(s, new_s);
    }
    return s;
}