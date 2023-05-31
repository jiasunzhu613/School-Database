/*******************************/
/********Includes/Set Up********/
/*******************************/

#include <fmt/core.h>
#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>

#include "SchoolDB.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

using std::cout, std::cerr, std::cin, std::endl, std::unordered_set,
    std::to_string, std::string, std::vector;

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <GLFW/glfw3.h>  // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to
// maximize ease of testing and compatibility with old VS compilers. To link
// with VS2010-era libraries, VS2015+ requires linking with
// legacy_stdio_definitions.lib, which we do using this pragma. Your own project
// should not be affected, as you are likely to link with a newer binary of GLFW
// that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See
// 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

// Declare global variables
SchoolDB db;
std::regex course_match{"([A-Z]{3}([1-2]O|[1-4]C|[1-4]M|[1-4]UE|[1-4]U))"};
std::regex teacher_match{"([A-Z]{1}\\d{5})"};
std::regex student_match{"([A-Z]{1}\\d{9})"};
// Teacher-related form submission validity
bool valid_id = true;
bool valid_pw = true;
bool valid_id_sign_up = true;
bool valid_pw_sign_up = true;
bool valid_first_name_sign_up = true;
bool valid_last_name_sign_up = true;
bool valid_address_sign_up = true;
bool valid_teachables_sign_up = true;

// Student-related for submission validity
bool valid_id_create = true;
bool valid_first_name_create = true;
bool valid_last_name_create = true;
bool valid_address_create = true;
bool valid_grade_create = true;
ImVec4 table_header_color = ImVec4(0.48f, 0.31f, 0.82f, 1.00f);

// Student Table IDs
enum StudentColumnID {
    StudentColumnID_ID,
    StudentColumnID_FirstName,
    StudentColumnID_LastName,
    StudentColumnID_Grade,
    StudentColumnID_NumLates,
    StudentColumnID_Address,
};

// Input filter
struct TextFilters {
    static int FilterCourseInput(ImGuiInputTextCallbackData *data) {
        if (data->EventChar < 256 &&
            strchr("QWERTYUIOPASDFGHJKLZXCVBNMqwertyuio"
                   "pasdfghjklzxcvbnm1234",
                   (char)data->EventChar))
            return 0;
        return 1;
    }

    static int FilterTeacherIDInput(ImGuiInputTextCallbackData *data) {
        if (data->EventChar < 256 &&
            strchr("Cc1234567890", (char)data->EventChar))
            return 0;
        return 1;
    }

    static int FilterStudentIDInput(ImGuiInputTextCallbackData *data) {
        if (data->EventChar < 256 &&
            strchr("Ss1234567890", (char)data->EventChar))
            return 0;
        return 1;
    }

    static int FilterGradeInput(ImGuiInputTextCallbackData *data) {
        if (data->EventChar < 256 &&
            strchr("1234567890", (char)data->EventChar))
            return 0;
        return 1;
    }
};

// Declare functions (initialized at the bottom of file)
bool LoadTextureFromFile(const char *, GLuint *, int *, int *);

static void glfw_error_callback(int, const char *);

static void HelpMarker(const char *);

void log();

bool validateInputTeacherID(string);

bool validateInputStudentID(string);

void addingStudentToCourse(string, bool &);

void creatingStudent(bool &);

// Main code
int main(int, char **) {
    db = SchoolDB{};
    db.reset();

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
    // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(
        1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can
    // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
    // them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
    // need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr.
    // Please handle those errors in your application (e.g. use an assertion, or
    // display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and
    // stored into a texture when calling
    // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
    // below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use
    // Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string
    // literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at
    // runtime from the "fonts/" folder. See Makefile.emscripten for details.
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // ImFont* font =
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
    // nullptr, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != nullptr);

    // Our state
    //    bool show_demo_window = true;
    //    bool show_another_window = false;
    //    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    string logged_in_employee;
    int pwflags1 = ImGuiInputTextFlags_Password;
    bool showPW1 = false;
    int pwflags2 = ImGuiInputTextFlags_Password;
    bool showPW2 = false;
    int pwflags3 = ImGuiInputTextFlags_Password;
    bool showPW3 = false;
    bool show_log_in_window = true;
    bool show_logged_in_window = false;
    //    bool first_use = true;
    //
    //    int my_image_width = 0;
    //    int my_image_height = 0;
    //    GLuint my_image_texture = 0;
    //    bool ret = LoadTextureFromFile("../Images/plus_sign.png",
    //    &my_image_texture,
    //                                   &my_image_width, &my_image_height);
    //    IM_ASSERT(ret);
    //
    //    int my_image_width2 = 0;
    //    int my_image_height2 = 0;
    //    GLuint my_image_texture2 = 0;
    //    bool ret2 = LoadTextureFromFile("../Images/join.png",
    //    &my_image_texture2,
    //                                    &my_image_width2, &my_image_height2);
    //    IM_ASSERT(ret2);
    bool isAddingStudentToCourse = false;
    bool isCreatingStudent = false;
    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not
    // attempt to do a fopen() of the imgui.ini file. You may manually call
    // LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
        // tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data
        // to your main application, or clear/overwrite your copy of the mouse
        // data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
        // data to your main application, or clear/overwrite your copy of the
        // keyboard data. Generally you may always pass all inputs to dear
        // imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();  // TODO: make frame have a minimum frame size
        if (!showPW1)
            pwflags1 = ImGuiInputTextFlags_Password;
        else
            pwflags1 = 0;
        if (!showPW2)
            pwflags2 = ImGuiInputTextFlags_Password;
        else
            pwflags2 = 0;
        if (!showPW3)
            pwflags3 = ImGuiInputTextFlags_Password;
        else
            pwflags3 = 0;

        log();
        if (show_logged_in_window) {
            ImGui::SetNextWindowSize(ImVec2(1280, 720));
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                    ImVec2(0.5f, 0.5f));
            // ImGuiWindowFlags_NoTitleBar
            ImGui::Begin("##a", 0);
            static vector<std::string> active_tabs{};
            static int next_tab_id = 0;
            if (next_tab_id == 0) {
                for (auto [courseCode, course] : db.getCourses()) {
                    if (course.getTeacher()->getEmployeeId() ==
                        logged_in_employee) {
                        active_tabs.push_back(courseCode);
                        next_tab_id++;
                    }
                }
            }

            static ImGuiTabBarFlags tab_bar_flags =
                ImGuiTabBarFlags_AutoSelectNewTabs |
                ImGuiTabBarFlags_Reorderable |
                ImGuiTabBarFlags_FittingPolicyResizeDown |
                ImGuiTabBarFlags_TabListPopupButton;

            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
                // Demo Trailing Tabs: click the "+" button to add a new tab
                // (in your app you may want to use a font icon instead of
                // the "+") Note that we submit it before the regular tabs,
                // but because of the ImGuiTabItemFlags_Trailing flag it
                // will always appear at the end.
                if (ImGui::TabItemButton("OPEN",
                                         ImGuiTabItemFlags_Trailing |
                                             ImGuiTabItemFlags_NoTooltip)) {
                    ImGui::OpenPopup("OPEN COURSE");
                    ImGui::SetNextWindowSize(ImVec2(400, 200));
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                            ImVec2(0.5f, 0.5f));
                }
                bool open_course_window = true;
                if (ImGui::BeginPopupModal("OPEN COURSE",
                                           &open_course_window)) {
                    vector<string> items{};
                    for (auto [courseCode, course] : db.getCourses()) {
                        if (find(active_tabs.begin(), active_tabs.end(),
                                 courseCode) == active_tabs.end() and
                            course.getTeacher()->getEmployeeId() ==
                                logged_in_employee) {
                            items.push_back(courseCode);
                        }
                    }
                    static int item_current_idx =
                        0;  // Here we store our selection data as an index.
                    // Custom size: use all width, 5 items tall
                    if (!items.empty())
                        ImGui::Text("UNOPENED COURSES:");
                    else
                        ImGui::Text(
                            "ALL AVAILABLE COURSES HAVE ALREADY BEEN OPENED.");
                    if (ImGui::BeginListBox(
                            "##listbox",
                            ImVec2(
                                -FLT_MIN,
                                5 * ImGui::GetTextLineHeightWithSpacing()))) {
                        for (int n = 0; n < items.size(); n++) {
                            const bool is_selected = (item_current_idx == n);
                            if (ImGui::Selectable(items[n].c_str(),
                                                  is_selected))
                                item_current_idx = n;

                            // Set the initial focus when opening the combo
                            // (scrolling + keyboard navigation focus)
                            if (is_selected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndListBox();
                    }
                    if (!items.empty()) {
                        if (ImGui::Button("OPEN")) {
                            active_tabs.push_back(items[item_current_idx]);
                            next_tab_id++;
                            ImGui::CloseCurrentPopup();
                        }
                    } else {
                        if (ImGui::Button("CLOSE")) ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                if (ImGui::TabItemButton("+",
                                         ImGuiTabItemFlags_Trailing |
                                             ImGuiTabItemFlags_NoTooltip)) {
                    ImGui::OpenPopup("CREATE COURSE");
                    ImGui::SetNextWindowSize(ImVec2(400, 200));
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                            ImVec2(0.5f, 0.5f));
                }
                bool add_course_window = true;
                if (ImGui::BeginPopupModal("CREATE COURSE",
                                           &add_course_window)) {
                    ImGui::Text("COURSE ID");

                    static char buf1[64] = "";
                    ImGui::InputText("##a", buf1, 64,
                                     ImGuiInputTextFlags_EnterReturnsTrue |
                                         ImGuiInputTextFlags_CharsUppercase |
                                         ImGuiInputTextFlags_CallbackCharFilter,
                                     TextFilters::FilterCourseInput);
                    // TODO: (might be too hard tbh) implement keyboard "enter"
                    // key detection and allow enter to use the button
                    // (additional feature)
                    ImGui::SameLine();
                    HelpMarker(
                        "Only courses that haven't been created will be "
                        "validated and allowed to be created. Valid course "
                        "codes (e.g. MPM4UE, ICS4U, AVI2O) consist of 3 "
                        "letters followed by a number course grade then a "
                        "course difficulty).");

                    if (!std::regex_match(buf1, course_match)) {
                        ImGui::Text("INVALID COURSE CODE");
                    } else {
                        if (ImGui::Button("CREATE")) {
                            int count = 1;
                            for (auto [courseCode, course] : db.getCourses()) {
                                if (buf1 ==
                                    courseCode.substr(0, courseCode.find('-')))
                                    count += 1;
                            }
                            Teacher t;
                            for (auto [employeeID, teacher] :
                                 db.getTeachers()) {
                                if (employeeID == logged_in_employee) {
                                    t = teacher;
                                    break;
                                }
                            }
                            string output = buf1;
                            Course c{&t, output, count, {}};
                            db.addCourse(c);
                            active_tabs.push_back(
                                fmt::format("{}-{:02}", output, count).c_str());
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndPopup();
                }

                // Submit our regular tabs
                for (int n = 0; n < active_tabs.size();) {
                    bool open = true;
                    char name[16];
                    snprintf(name, IM_ARRAYSIZE(name), "%s",
                             active_tabs[n].c_str());
                    if (ImGui::BeginTabItem(name, &open,
                                            ImGuiTabItemFlags_None)) {
                        if (ImGui::BeginTable(
                                fmt::format("Students of {}", active_tabs[n])
                                    .c_str(),
                                6,
                                ImGuiTableFlags_Sortable |
                                    ImGuiTableFlags_SortMulti |
                                    ImGuiTableFlags_RowBg |
                                    ImGuiTableFlags_Borders |
                                    ImGuiTableFlags_BordersH |
                                    ImGuiTableFlags_BordersOuterH |
                                    ImGuiTableFlags_BordersInnerH |
                                    ImGuiTableFlags_BordersV |
                                    ImGuiTableFlags_BordersOuterV |
                                    ImGuiTableFlags_BordersInnerV |
                                    ImGuiTableFlags_BordersOuter |
                                    ImGuiTableFlags_BordersInner)) {
                            ImGui::TableSetBgColor(
                                ImGuiTableBgTarget_RowBg0,
                                ImGui::GetColorU32(table_header_color));
                            ImGui::TableSetupColumn(
                                "Student ID", ImGuiTableColumnFlags_DefaultSort,
                                0.0f);
                            ImGui::TableSetupColumn("First Name", 0, 0.0f);
                            ImGui::TableSetupColumn("Last Name", 0, 0.0f);
                            ImGui::TableSetupColumn("Grade", 0, 0.0f);
                            ImGui::TableSetupColumn(
                                "Number of Lates",
                                ImGuiTableColumnFlags_PreferSortDescending,
                                0.0f);
                            ImGui::TableSetupColumn(
                                "Address", ImGuiTableColumnFlags_NoSort, 0.0f);

                            ImGui::TableHeadersRow();
                            ImGui::TableNextRow();
                            for (auto student : db.getCourses()[active_tabs[n]]
                                                    .getStudents()) {
                                ImGui::TableNextColumn();
                                ImGui::Text(student->getStudentId().c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text(student->getFirstName().c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text(student->getLastName().c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text(
                                    to_string(student->getGrade()).c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text(
                                    to_string(student->getNumLates()).c_str());
                                ImGui::SameLine();
                                if (ImGui::Button(
                                        fmt::format("+##{}",
                                                    student->getStudentId())
                                            .c_str())) {
                                    student->addLate();
                                }
                                ImGui::TableNextColumn();
                                ImGui::Text(student->getAddress().c_str());
                            }
                            ImGui::TableNextColumn();
                            if (ImGui::Button("Add Student to Course?")) {
                                isAddingStudentToCourse = true;
                                ImGui::SetNextWindowSize(ImVec2(400, 200));
                                ImGui::SetNextWindowPos(center,
                                                        ImGuiCond_Appearing,
                                                        ImVec2(0.5f, 0.5f));
                            }
                            if (isAddingStudentToCourse)
                                addingStudentToCourse(active_tabs[n],
                                                      isAddingStudentToCourse);

                            if (ImGui::Button("Create Student?")) {
                                isCreatingStudent = true;
                                ImGui::SetNextWindowSize(ImVec2(400, 200));
                                ImGui::SetNextWindowPos(center,
                                                        ImGuiCond_Appearing,
                                                        ImVec2(0.5f, 0.5f));
                            }
                            if (isCreatingStudent)
                                creatingStudent(isCreatingStudent);
                            ImGui::EndTable();
                        }

                        ImGui::EndTabItem();
                    }
                    db.save();
                    // these if's control the opening and closing of new
                    // tabs
                    if (!open) {
                        active_tabs.erase(active_tabs.begin() + n);
                    } else
                        n++;
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }
        // 2. Show a simple window that we create ourselves. We use a Begin/End
        // pair to create a named window.
        else if (show_log_in_window) {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::SetNextWindowSize(ImVec2(400, 200));
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                    ImVec2(0.5f, 0.5f));
            // ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            // ImGuiWindowFlags_NoCollapse
            ImGui::Begin("Log in!", 0, ImGuiCond_FirstUseEver);
            ImGui::Text("Teacher ID");
            static char buf1[64] = "";
            string IDLabel;
            if (valid_id)
                IDLabel = "##a";
            else
                IDLabel = "INVALID ID";
            ImGui::InputText(
                IDLabel.c_str(), buf1, 64,
                ImGuiInputTextFlags_EnterReturnsTrue |
                    ImGuiInputTextFlags_CharsUppercase |
                    ImGuiInputTextFlags_CallbackCharFilter,
                TextFilters::FilterTeacherIDInput);  // Display some text (you
                                                     // can use a format strings
                                                     // too)
            ImGui::Text("Password");
            static char password[64] = "";
            string PWLabel;
            if (valid_pw)
                PWLabel = "##b";
            else
                PWLabel = "NON-MATCHING PW";
            ImGui::InputText(PWLabel.c_str(), password, IM_ARRAYSIZE(password),
                             pwflags1);
            ImGui::SameLine();
            ImGui::Checkbox("Show Password", &showPW1);
            if (ImGui::Button("Sign In")) {
                valid_id = false;
                valid_pw = false;
                for (auto [employeeID, teacher] : db.getTeachers()) {
                    if (employeeID == buf1 and
                        teacher.getPassword() == password) {
                        logged_in_employee = buf1;
                        show_logged_in_window = true;
                        valid_id = true;
                        valid_pw = true;
                        break;
                    } else if (employeeID == buf1) {
                        valid_id = true;
                    }
                }
            }
            ImGui::SameLine();
            ImGui::Text("New Teacher?");
            ImGui::SameLine();
            if (ImGui::Button("Create Teacher ID")) {
                show_log_in_window = false;
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        } else {
            ImGui::SetNextWindowSize(ImVec2(400, 200));
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                    ImVec2(0.5f, 0.5f));
            // ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            // ImGuiWindowFlags_NoCollapse
            ImGui::Begin("Sign Up!", 0,
                         ImGuiCond_FirstUseEver | ImGuiWindowFlags_NoResize);
            ImGui::Text("Teacher ID");
            static char id1[64] = "";
            string IDLabel;
            if (valid_id_sign_up)
                IDLabel = "##a";
            else
                IDLabel = "INVALID ID";
            ImGui::InputText(
                IDLabel.c_str(), id1, 64,
                ImGuiInputTextFlags_EnterReturnsTrue |
                    ImGuiInputTextFlags_CharsUppercase |
                    ImGuiInputTextFlags_CallbackCharFilter,
                TextFilters::FilterTeacherIDInput);  // Display some text (you
                                                     // can use a format strings
                                                     // too)
            ImGui::NewLine();

            ImGui::Text("First Name");
            static char fn1[64] = "";
            string FNLabel;
            if (valid_first_name_sign_up)
                FNLabel = "##b";
            else
                FNLabel = "REQUIRED";
            ImGui::InputText(
                FNLabel.c_str(), fn1,
                64);  // Display some text (you can use a format strings too)
            ImGui::NewLine();

            ImGui::Text("Last Name");
            static char ln1[64] = "";
            string LNLabel;
            if (valid_last_name_sign_up)
                LNLabel = "##c";
            else
                LNLabel = "REQUIRED##a";
            ImGui::InputText(
                LNLabel.c_str(), ln1,
                64);  // Display some text (you can use a format strings too)
            ImGui::NewLine();

            ImGui::Text("Address");
            static char address1[64] = "";
            string ALabel;
            if (valid_address_sign_up)
                ALabel = "##d";
            else
                ALabel = "REQUIRED##b";
            ImGui::InputText(
                ALabel.c_str(), address1,
                64);  // Display some text (you can use a format strings too)
            ImGui::NewLine();

            ImGui::Text("Teachables");
            ImGui::SameLine();
            HelpMarker(
                "Each course listed as teachables must follow valid course "
                "code criterions and be separated by a space.");
            static char teachables1[64] = "";
            string TLabel;
            if (valid_teachables_sign_up)
                TLabel = "##e";
            else
                TLabel =
                    "ONE OR MORE TEACHABLES DOES NOT FOLLOW VALID FORMATTING";
            ImGui::InputText(
                TLabel.c_str(), teachables1,
                64);  // Display some text (you can use a format strings too)
            ImGui::NewLine();

            ImGui::Text("Password");
            static char password2[64] = "";
            string PWLabel1, PWLabel2;
            if (valid_pw_sign_up) {
                PWLabel1 = "##f";
                PWLabel2 = "##g";
            } else {
                PWLabel1 = "NON-MATCHING OR UNFILLED PASSWORDS ";
                PWLabel2 = "NON-MATCHING OR UNFILLED PASSWORDS##";
            }
            ImGui::InputText(PWLabel1.c_str(), password2,
                             IM_ARRAYSIZE(password2), pwflags2);
            ImGui::SameLine();
            ImGui::Checkbox("Show Password", &showPW2);
            ImGui::NewLine();

            ImGui::Text("Confirm Password");
            static char password3[64] = "";
            ImGui::InputText(PWLabel2.c_str(), password3,
                             IM_ARRAYSIZE(password3), pwflags3);
            ImGui::SameLine();
            ImGui::Checkbox("Show Password##", &showPW3);
            ImGui::NewLine();

            if (ImGui::Button("Sign Up")) {
                valid_id_sign_up = validateInputTeacherID(id1);
                valid_pw_sign_up = (strcmp(password2, password3) == 0 and
                                    (strcmp(password2, "") != 0 and
                                     strcmp(password3, "") != 0));
                valid_first_name_sign_up = strcmp(fn1, "") != 0;
                valid_last_name_sign_up = strcmp(ln1, "") != 0;
                valid_address_sign_up = strcmp(address1, "") != 0;
                valid_teachables_sign_up = true;
                string s = teachables1;
                std::string delimiter = " ";
                size_t pos = 0;
                std::string token;
                while ((pos = s.find(delimiter)) != std::string::npos) {
                    token = s.substr(0, pos);
                    if (!std::regex_match(token, course_match)) {
                        valid_teachables_sign_up = false;
                        break;
                    }
                    s.erase(0, pos + delimiter.length());
                }

                if (!std::regex_match(s, course_match)) {
                    valid_teachables_sign_up = false;
                }

                if (valid_id_sign_up and valid_pw_sign_up and
                    valid_first_name_sign_up and valid_last_name_sign_up and
                    valid_address_sign_up and valid_teachables_sign_up) {
                    db.addTeacher(Teacher{fn1, ln1, address1, teachables1, id1,
                                          password2});
                    db.save();
                    ImGui::OpenPopup("ID CREATION SUCCESSFUL!");
                }
            }
            bool account_creation_success_window = true;
            if (ImGui::BeginPopupModal("ID CREATION SUCCESSFUL!",
                                       &account_creation_success_window)) {
                ImGui::Text(
                    "YOUR ID HAS BEEN SUCCESSFULLY CREATED. \nLOG IN "
                    "THROUGH THE LOG IN WINDOW!");
                ImGui::EndPopup();
            }
            if (!account_creation_success_window) show_log_in_window = true;

            if (ImGui::Button("Return To Log In Window")) {
                show_log_in_window = true;
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // Renders panels
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

/*************************/
/********Functions********/
/*************************/

// Simple helper function to load an image into a OpenGL texture with common
// settings
bool LoadTextureFromFile(const char *filename, GLuint *out_texture,
                         int *out_width, int *out_height) {
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char *image_data =
        stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL) return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);  // This is required on WebGL for non
    // power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE);  // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void HelpMarker(const char *desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) &&
        ImGui::BeginTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void log() {
    ImGui::Begin("Log");
    ImGui::Text(db.log(ImGui::Button("Update Logging?")).c_str());
    ImGui::End();
}

void addingStudentToCourse(string courseCode, bool &isCreatingStudent) {
    std::unordered_set<Student *> students =
        db.getCourses()[courseCode].getStudents();
    ImGui::OpenPopup("Adding Student to Course");
    if (ImGui::BeginPopupModal("Adding Student to Course",
                               &isCreatingStudent)) {
        ImGui::Text("FILTER BY: ");
        ImGui::SameLine();
        static char buf1[64] = "";
        ImGui::InputText("##a", buf1, IM_ARRAYSIZE(buf1),
                         ImGuiInputTextFlags_EnterReturnsTrue);
        vector<string> items{};
        // filter/include students into list
        std::regex student_list_match{fmt::format(R"(\b\w*{}\w*\b)", buf1),
                                      std::regex_constants::icase};
        for (auto [studentID, student] : db.getStudents()) {
            bool isInCourse = false;
            for (Student *student : students) {
                if (student->getStudentId() == studentID) {
                    isInCourse = true;
                    break;
                }
            }
            if (std::regex_search(student.toString(), student_list_match) and
                !isInCourse)
                items.push_back(studentID);
        }
        static int item_current_idx =
            0;  // Here we store our selection data as an index.
        // Custom size: use all width, 5 items tall
        if (!items.empty())
            ImGui::Text("UNADDED STUDENTS:");
        else
            ImGui::Text("ALL AVAILABLE STUDENTS HAVE ALREADY BEEN ADDED.");
        if (ImGui::BeginListBox(
                "##listbox",
                ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (int n = 0; n < items.size(); n++) {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(items[n].c_str(), is_selected))
                    item_current_idx = n;
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) &&
                    ImGui::BeginTooltip()) {
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
                    ImGui::TextUnformatted(
                        db.getStudents()[items[n]].toString().c_str());
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }

                // Set the initial focus when opening the combo (scrolling +
                // keyboard navigation focus)
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
        if (!items.empty()) {
            if (ImGui::Button("ADD")) {
                //                active_tabs.push_back(items[item_current_idx]);
                //                next_tab_id++;
                Student &studentBeingAdded =
                    db.getStudents().at(string{items[item_current_idx]});

                db.getCourses()[courseCode].addStudentToClass(
                    &studentBeingAdded);
                isCreatingStudent = false;
            }
        } else {
            if (ImGui::Button("CLOSE")) isCreatingStudent = false;
        }
        ImGui::EndPopup();
    }
    //    if (ImGui::BeginPopupModal("Adding Student to
    //    Course",&isCreatingStudent)) {
    //        const int buffSize = 11;
    //        static char studentID[buffSize];
    //        ImGui::InputText("Student ID", studentID, buffSize,
    //        ImGuiInputTextFlags_EnterReturnsTrue
    //        |ImGuiInputTextFlags_CharsUppercase
    //        |ImGuiInputTextFlags_CallbackCharFilter,TextFilters::FilterStudentIDInput);
    //        if (db.getStudents().contains(string{studentID})) {
    //            if (ImGui::Button("Submit")) {
    //                Student &studentBeingAdded =
    //                        db.getStudents().at(string{studentID});
    //
    //                db.getCourses()[courseCode].addStudentToClass(
    //                        &studentBeingAdded);
    //
    //                unordered_set x =
    //                db.getCourses().at(courseCode).getStudents(); for (Student
    //                *student:
    //                        db.getCourses().at(courseCode).getStudents()) {
    //                    cout << student->getFirstName() << " "
    //                         << student->getLastName() << endl;
    //                    cout << student->getStudentId() << endl;
    //                }
    //                isCreatingStudent = false;
    //                for (char &c: studentID) c = '\0';
    //            }
    //        } else {
    //            ImGui::Text("Student with this ID doesn't exist");
    //            if (ImGui::Button("Quit?")) {
    //                isCreatingStudent = false;
    //                for (char &c: studentID) c = ' ';
    //            }
    //        }
    //
    //        ImGui::EndPopup();
    //    }
}

void creatingStudent(bool &isCreatingStudent) {
    ImGui::OpenPopup("Creating Student");
    if (ImGui::BeginPopupModal("Creating Student", &isCreatingStudent)) {
        // Student ID
        ImGui::Text("Student ID");
        static char id1[64] = "";
        string IDLabel;
        if (valid_id_create)
            IDLabel = "##a";
        else
            IDLabel = "INVALID ID";
        ImGui::InputText(
            IDLabel.c_str(), id1, 64,
            ImGuiInputTextFlags_EnterReturnsTrue |
                ImGuiInputTextFlags_CharsUppercase |
                ImGuiInputTextFlags_CallbackCharFilter,
            TextFilters::FilterStudentIDInput);  // Display some text (you can
                                                 // use a format strings too)
        ImGui::NewLine();

        // First name
        ImGui::Text("First Name");
        static char fn1[64] = "";
        string FNLabel;
        if (valid_first_name_create)
            FNLabel = "##b";
        else
            FNLabel = "REQUIRED";
        ImGui::InputText(
            FNLabel.c_str(), fn1,
            64);  // Display some text (you can use a format strings too)
        ImGui::NewLine();

        // Last name
        ImGui::Text("Last Name");
        static char ln1[64] = "";
        string LNLabel;
        if (valid_last_name_create)
            LNLabel = "##c";
        else
            LNLabel = "REQUIRED##a";
        ImGui::InputText(
            LNLabel.c_str(), ln1,
            64);  // Display some text (you can use a format strings too)
        ImGui::NewLine();

        // Address
        ImGui::Text("Address");
        static char address1[64] = "";
        string ALabel;
        if (valid_address_create)
            ALabel = "##d";
        else
            ALabel = "REQUIRED##b";
        ImGui::InputText(
            ALabel.c_str(), address1,
            64);  // Display some text (you can use a format strings too)
        ImGui::NewLine();

        // Grade
        ImGui::Text("Grade");
        static char grade[64] = "";
        string GLabel;
        if (valid_grade_create)
            GLabel = "##e";
        else
            GLabel = "REQUIRED##c";
        ImGui::InputText(
            GLabel.c_str(), grade, 64, ImGuiInputTextFlags_CallbackCharFilter,
            TextFilters::FilterGradeInput);  // Display some text (you can use a
                                             // format strings too)
        ImGui::NewLine();

        if (ImGui::Button("Create")) {
            valid_id_create = validateInputStudentID(id1);
            valid_first_name_create = strcmp(fn1, "") != 0;
            valid_last_name_create = strcmp(ln1, "") != 0;
            valid_address_create = strcmp(address1, "") != 0;
            valid_grade_create = strcmp(grade, "") != 0;

            if (valid_id_create and valid_first_name_create and
                valid_last_name_create and valid_address_create and
                valid_grade_create) {
                db.addStudent(
                    Student{fn1, ln1, address1, std::stoi(grade), id1});
                db.save();
                ImGui::OpenPopup("STUDENT CREATION SUCCESSFUL!");
                isCreatingStudent = false;
            }
        }
        bool account_creation_success_window = true;
        if (ImGui::BeginPopupModal("STUDENT CREATION SUCCESSFUL!",
                                   &account_creation_success_window)) {
            ImGui::Text(
                "STUDENT HAS BEEN SUCCESSFULLY CREATED. \n ADD STUDENT TO "
                "COURSE USING THE \"ADDING STUDENT TO COURSE?\" BUTTON");
            ImGui::EndPopup();
        }
        ImGui::EndPopup();
    }
}

bool validateInputTeacherID(string in) {
    if (!std::regex_match(in, teacher_match)) return false;
    return db.getTeachers().find(in) == db.getTeachers().end();
}

bool validateInputStudentID(string in) {
    if (!std::regex_match(in, student_match)) return false;
    return db.getStudents().find(in) == db.getStudents().end();
}