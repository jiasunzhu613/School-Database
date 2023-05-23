#include <algorithm> // sort
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>
#include <cassert>

//important imports
#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/writer.h>
#include <fstream>


using namespace std;

int main(){
//    ifstream infile;
//    infile.open("../random.txt");
//
//    if (infile.fail()){
//        cerr << "error opening" << endl;
//        exit(1);
//    }
//
//    int x, y;
//    infile >> x >> y;
//    cout << x << y << endl;

    //input file declaration
    ifstream infile;
    //open file into "infile"
    infile.open("../details.json");

    //json value variable
    Json::Value actualJSON;
    //json reder
    Json::Reader reader;
    //using reader to parse infile into actualJSON
    reader.parse(infile, actualJSON);

    //use this to find the name of keys in json file
    for (auto const& id : actualJSON.getMemberNames()) {
        std::cout << id << std::endl;
    }

    //print out all json data
    cout << "ALL JSON DATA: \n" << actualJSON << endl;

    //print out elements using known keys
    cout << "Name: " << actualJSON["Name"] << endl;
    cout << "DoB: " << actualJSON["DoB"] << endl;
    cout << "Subject: " << actualJSON["Subject"] << endl;
    cout << "Grade: " << actualJSON["Grade"] << endl;

    //change value of key
    actualJSON["Name"] = "Your father";
    //creat new Json::Value
    Json::Value newValue;
    newValue["StudentID"] = "S13978203";
    newValue["Address"] = "123 Sesame Street";
    //below one line added the newValue Json::Value to .json file / added to existing array
//    actualJSON["Students"].append(newValue); // this line
    actualJSON["HELLO!"] = "HI";
//    for (int i = 0; i < actualJSON["Students"].size(); i++){
//        cout << actualJSON["Students"][i] << endl;
//    }
    cout << "work0" << endl;

    cout << actualJSON.size() << endl;
    cout << "work1" << endl;
    Json::Value newnewValue;
    newnewValue["Student ID"] = "S29839028";
    newnewValue["Address"] = "123 the bomb street";
    actualJSON["Students"]["Bobby"] = newnewValue;

    cout << "work2" << endl;

//    actualJSON["Students"][actualJSON.size()]=Json::Value("HI");

    //open output file
    std::ofstream file_id;
    file_id.open("../details.json");
    //override existing json file using styledwriter
//    Json::StyledWriter styledWriter;
    //write (override) values stored in actualJSON into existing json file
//    file_id << styledWriter.write(actualJSON);
    file_id << actualJSON;

    //close output file
    file_id.close();


    return 0;
}


