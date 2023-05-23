#pragma once

#include <string>
using std::string;

class Person {
   private:
    string firstName;
    string lastName;
    string address;

   public:
    Person();
    Person(string first, string last);
    Person(string first, string last, string add);
    string getFirstName();
    string getLastName();
    string getAddress();
    void setFirstName(string n);
    void setLastName(string n);
    void setAddress(string a);
    string toString();
};