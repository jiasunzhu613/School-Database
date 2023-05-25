#include "Person.hpp"

Person::Person() {}
Person::Person(string first, string last) : firstName{first}, lastName{last} {}
Person::Person(string first, string last, string add)
    : firstName{first}, lastName{last}, address{add} {}

string Person::getFirstName() const { return firstName; }
string Person::getLastName() const { return lastName; }
string Person::getAddress() const { return address; }

void Person::setFirstName(string n) { firstName = n; }
void Person::setLastName(string n) { lastName = n; }
void Person::setAddress(string a) { address = a; }
string Person::toString() {
    return "Hi, my name is " + firstName + " " + lastName + " and I live at " +
           address;
}