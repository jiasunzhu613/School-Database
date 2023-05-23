#include <algorithm> // sort
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>
#include <cassert>
#include <regex>

//important imports
#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/writer.h>
#include <fstream>


using namespace std;

bool validateCourseCode(string courseCode){
    // ICS4U-01
    // MPM4UE-01
    // auto add -xy after where x and y are numbers
    // 3 letters, check combinations of number + letter
    // 6 or 8 characters
    cout << courseCode.find('-') << endl;
    if (courseCode.find('-') <= courseCode.size()){
        string front = courseCode.substr(0, courseCode.find('-'));
        string end = courseCode.substr(courseCode.find('-') + 1, courseCode.size() + 1);
        //number check for end
        for (char i : end){
            if (!('0' <= i <= '9'))
                return false;
        }

        int index_of_number = 0;
        int count = 0;
        for (int i = 0; i < front.size(); i++){
            if ('1' <= front[i] <= '4'){
                index_of_number = i;
                count++;
            }
        }
        if (count != 1)
            return false;

        string subject = front.substr(0, index_of_number);
        string difficulty = front.substr(index_of_number + 1, front.size() + 1);

        //Checking if subject input valid
        for (int i = 0; i < 3; i++){
            if (!('A' <= subject[i] <= 'Z'))
                return false;
        }

        //Valid values for difficulty
        vector<string> valid_difficulty;
        valid_difficulty.push_back("O");
        valid_difficulty.push_back("C");
        valid_difficulty.push_back("M");
        valid_difficulty.push_back("U");
        valid_difficulty.push_back("UE");

        return std::find(valid_difficulty.begin(), valid_difficulty.end(), difficulty) == valid_difficulty.end();
    }else{
        int index_of_number = 0;
        int count = 0;
        for (int i = 0; i < courseCode.size(); i++){
            if ('1' <= courseCode[i] and courseCode[i] <= '4'){
                index_of_number = i;
                count++;
            }
        }
        if (count != 1)
            return false;

        string subject = courseCode.substr(0, index_of_number);
        string difficulty = courseCode.substr(index_of_number + 1, courseCode.size() + 1);

        //Checking if subject input valid
        for (int i = 0; i < 3; i++){
            if (!('A' <= subject[i] <= 'Z'))
                return false;
        }

        //Valid values for difficulty
        vector<string> valid_difficulty;
        valid_difficulty.push_back("O");
        valid_difficulty.push_back("C");
        valid_difficulty.push_back("M");
        valid_difficulty.push_back("U");
        valid_difficulty.push_back("UE");
        return !(std::find(valid_difficulty.begin(), valid_difficulty.end(), difficulty) == valid_difficulty.end());
    }
}

int main(){
    std::regex course_match{"([A-Z]{3}[1-4](O|C|M|UE|U))"};
    cout << std::regex_match("ENG4UE", course_match) << endl;
//    cout << validateCourseCode("ENG") << endl;
    cout << validateCourseCode("ENGUE") << endl;
//    cout << validateCourseCode("ENG4UE-01") << endl;

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
//    ifstream infile;
//    //open file into "infile"
//    infile.open("../details.json");
//
//    //json value variable
//    Json::Value actualJSON;
//    //json reder
//    Json::Reader reader;
//    //using reader to parse infile into actualJSON
//    reader.parse(infile, actualJSON);
//
//    //use this to find the name of keys in json file
//    for (auto const& id : actualJSON.getMemberNames()) {
//        std::cout << id << std::endl;
//    }
//
//    //print out all json data
//    cout << "ALL JSON DATA: \n" << actualJSON << endl;
//
//    //print out elements using known keys
//    cout << "Name: " << actualJSON["Name"] << endl;
//    cout << "DoB: " << actualJSON["DoB"] << endl;
//    cout << "Subject: " << actualJSON["Subject"] << endl;
//    cout << "Grade: " << actualJSON["Grade"] << endl;
//
//    //change value of key
//    actualJSON["Name"] = "Your father";
//    //creat new Json::Value
//    Json::Value newValue;
//    newValue["StudentID"] = "S13978203";
//    newValue["Address"] = "123 Sesame Street";
//    //below one line added the newValue Json::Value to .json file / added to existing array
////    actualJSON["Students"].append(newValue); // this line
//    actualJSON["HELLO!"] = "HI";
////    for (int i = 0; i < actualJSON["Students"].size(); i++){
////        cout << actualJSON["Students"][i] << endl;
////    }
//    cout << "work0" << endl;
//
//    cout << actualJSON.size() << endl;
//    cout << "work1" << endl;
//    Json::Value newnewValue;
//    newnewValue["Student ID"] = "S29839028";
//    newnewValue["Address"] = "123 the bomb street";
//    actualJSON["Students"]["Bobby"] = newnewValue;
//
//    cout << "work2" << endl;
//
////    actualJSON["Students"][actualJSON.size()]=Json::Value("HI");
//
//    //open output file
//    std::ofstream file_id;
//    file_id.open("../details.json");
//    //override existing json file using styledwriter
////    Json::StyledWriter styledWriter;
//    //write (override) values stored in actualJSON into existing json file
////    file_id << styledWriter.write(actualJSON);
//    file_id << actualJSON;
//
//    //close output file
//    file_id.close();


    return 0;
}


