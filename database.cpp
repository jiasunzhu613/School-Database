#include <fmt/core.h>
#include <stdio.h>

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

SchoolDB db;

// Simple helper function to load an image into a OpenGL texture with common
// settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture,
                         int* out_width, int* out_height) {
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data =
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

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void log() {
    ImGui::Begin("Log");
    ImGui::Text(db.log(ImGui::Button("Update Logging?")).c_str());
    ImGui::End();
}

void addingStudentToCourse(string courseCode, bool& isCreatingStudent) {
    ImGui::OpenPopup("Adding Student to Course");
    if (ImGui::BeginPopupModal("Adding Student to Course",
                               &isCreatingStudent)) {
        const int buffSize = 11;
        static char studentID[buffSize];
        ImGui::InputText("Student ID", studentID, buffSize);

        if (db.getStudents().contains(string{studentID})) {
            if (ImGui::Button("Submit")) {
                Student& studentBeingAdded =
                    db.getStudents().at(string{studentID});

                db.getCourses()[courseCode].addStudentToClass(
                    &studentBeingAdded);

                unordered_set x = db.getCourses().at(courseCode).getStudents();
                for (Student* student :
                     db.getCourses().at(courseCode).getStudents()) {
                    cout << student->getFirstName() << " "
                         << student->getLastName() << endl;
                    cout << student->getStudentId() << endl;
                }
                isCreatingStudent = false;
                for (char& c : studentID) c = '\0';
            }
        } else {
            ImGui::Text("Student with this ID doesn't exist");
            if (ImGui::Button("Quit?")) {
                isCreatingStudent = false;
                for (char& c : studentID) c = ' ';
            }
        }

        ImGui::EndPopup();
    }
}

// Main code
int main(int, char**) {
    db = SchoolDB{};
    db.reset();

    string x = db.log(true);
    cout << db.log() << endl;

    //    Teacher BenHuddy{"Benjamin", "Hudson", "ICS4U", "C69696"};
    //    Teacher Hughes{"Andy", "Hughes", "MPM4UE", "C42042"};
    //
    //    Student WillyGao{"William", "Gao", 69, "S123456789"};
    //    Student JZhubers{"Jonathan", "Zhu", 99, "S696969696"};
    //    Student RahulVedula{"Rahul", "Vedula", 5, "S420420420"};
    //    Student NB{"Naman", "Biyani", 100, "S987654321"};
    //
    //    Course Comp_Sci{&BenHuddy, "ICS4U", 1, {&WillyGao, &JZhubers, &NB}};
    //    Course Math{&Hughes, "MPM4UE", 1, {&WillyGao, &RahulVedula}};
    //
    //    db.addCourse(Comp_Sci);
    //    db.addCourse(Math);
    //
    //    db.addTeacher(BenHuddy);
    //    db.addTeacher(Hughes);
    //
    //    db.addStudent(WillyGao);
    //    db.addStudent(JZhubers);
    //    db.addStudent(RahulVedula);
    //    db.addStudent(NB);
    //
    //    for (auto course : NB.getCourses())
    //        cout << course->getFullCourseCode() << endl;
    //     db.save();

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
    // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
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
    std::regex course_match{"([A-Z]{3}([1-2]O|[1-4]C|[1-4]M|[1-4]UE|[1-4]U))"};
    ImVec4 table_header_color = ImVec4(0.48f, 0.31f, 0.82f, 1.00f);
    int pwflags1 = ImGuiInputTextFlags_Password;
    bool showPW1 = false;
    int pwflags2 = ImGuiInputTextFlags_Password;
    bool showPW2 = false;
    int pwflags3 = ImGuiInputTextFlags_Password;
    bool showPW3 = false;
    bool show_log_in_window = true;
    bool show_logged_in_window = false;
    bool first_use = true;

    int my_image_width = 0;
    int my_image_height = 0;
    GLuint my_image_texture = 0;
    bool ret = LoadTextureFromFile("../Images/plus_sign.png", &my_image_texture,
                                   &my_image_width, &my_image_height);
    IM_ASSERT(ret);

    int my_image_width2 = 0;
    int my_image_height2 = 0;
    GLuint my_image_texture2 = 0;
    bool ret2 = LoadTextureFromFile("../Images/join.png", &my_image_texture2,
                                    &my_image_width2, &my_image_height2);
    IM_ASSERT(ret2);
    bool isAddingStudentToCourse = false;
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
            if (first_use) {
                ImGui::SetCursorPos(ImVec2(
                    (ImGui::GetWindowSize().x - (my_image_width / 5)) * 0.5f,
                    (ImGui::GetWindowSize().y - (my_image_height / 5)) * 0.5f));
                if (ImGui::ImageButton(
                        (void*)(intptr_t)my_image_texture,
                        ImVec2(my_image_width / 5, my_image_height / 5))) {
                    ImGui::OpenPopup("CREATE SCHOOL");
                    ImGui::SetNextWindowSize(ImVec2(400, 200));
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                            ImVec2(0.5f, 0.5f));
                }
                bool create_school_window = true;
                if (ImGui::BeginPopupModal("CREATE SCHOOL",
                                           &create_school_window)) {
                    ImGui::Text("SCHOOL ID or SCHOOL NAME");
                    static char buf3[64] = "";
                    ImGui::InputText("##a", buf3, 64);
                    ImGui::Text("Password");
                    static char password2[64] = "";
                    ImGui::InputText("##b", password2, IM_ARRAYSIZE(password2),
                                     pwflags2);
                    ImGui::SameLine();
                    ImGui::Checkbox("Show Password", &showPW2);
                    ImGui::Text("Confirm Password");
                    static char password3[64] = "";
                    ImGui::InputText("##c", password3, IM_ARRAYSIZE(password3),
                                     pwflags3);
                    ImGui::SameLine();
                    ImGui::Checkbox("Show Password##", &showPW3);
                    if (ImGui::Button("CREATE")) {
                        ImGui::CloseCurrentPopup();
                        first_use = false;
                    }
                    ImGui::EndPopup();
                }
                // TODO: click "+" opens up new window or popup to create and
                // configure a new school
                ImGui::SetCursorPos(
                    ImVec2((ImGui::GetWindowSize().x - (my_image_width2 / 15) -
                            (1.5 * my_image_width / 5)) *
                               0.5f,
                           (ImGui::GetWindowSize().y - (my_image_height2 / 15) -
                            ((my_image_height2 / 15) - my_image_height / 5)) *
                               0.5f));
                if (ImGui::ImageButton(
                        (void*)(intptr_t)my_image_texture2,
                        ImVec2(my_image_width2 / 15, my_image_height2 / 15))) {
                    ImGui::OpenPopup("JOIN SCHOOL");
                    ImGui::SetNextWindowSize(ImVec2(400, 200));
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                            ImVec2(0.5f, 0.5f));
                }
                bool join_school_window = true;
                if (ImGui::BeginPopupModal("JOIN SCHOOL",
                                           &join_school_window)) {
                    ImGui::Text("SCHOOL ID or SCHOOL NAME");
                    static char buf3[64] = "";
                    ImGui::InputText("##a", buf3, 64);
                    ImGui::Text("Password");
                    static char password2[64] = "";
                    ImGui::InputText("##b", password2, IM_ARRAYSIZE(password2),
                                     pwflags2);
                    ImGui::SameLine();
                    ImGui::Checkbox("Show Password", &showPW2);
                    // TODO: click join opens up a pop up to enter code and
                    // passwords to get into a school
                    if (ImGui::Button("JOIN")) {
                        ImGui::CloseCurrentPopup();
                        first_use = false;
                    }
                    ImGui::EndPopup();
                }
                ImGui::End();
            } else {
                static vector<std::string> active_tabs{};
                static int next_tab_id = 0;
                if (next_tab_id == 0) {
                    for (auto [courseCode, course] : db.getCourses()) {
                        active_tabs.push_back(courseCode);
                        next_tab_id++;
                    }
                }

                // TabItemButton() and Leading/Trailing flags are distinct
                // features which we will demo together. (It is possible to
                // submit regular tabs with Leading/Trailing flags, or
                // TabItemButton tabs without Leading/Trailing flags... but they
                // tend to make more sense together)
                //            static bool show_leading_button = true;
                //            static bool show_trailing_button = true;
                //            ImGui::Checkbox("Show Leading TabItemButton()",
                //            &show_leading_button); ImGui::Checkbox("Show
                //            Trailing TabItemButton()", &show_trailing_button);

                // Expose some other flags which are useful to showcase how they
                // interact with Leading/Trailing tabs
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
                        // Input filter
                        struct TextFilters {
                            // Return 0 (pass) if the character is 'i' or 'm' or
                            // 'g' or 'u' or 'i'
                            static int FilterImGuiLetters(
                                ImGuiInputTextCallbackData* data) {
                                if (data->EventChar < 256 &&
                                    strchr("QWERTYUIOPASDFGHJKLZXCVBNMqwertyuio"
                                           "pasdfghjklzxcvbnm1234",
                                           (char)data->EventChar))
                                    return 0;
                                return 1;
                            }
                        };

                        static char buf1[64] = "";
                        ImGui::InputText(
                            "##a", buf1, 64,
                            ImGuiInputTextFlags_EnterReturnsTrue |
                                ImGuiInputTextFlags_CharsUppercase |
                                ImGuiInputTextFlags_CallbackCharFilter,
                            TextFilters::FilterImGuiLetters);  // Display some
                                                               // text (you can
                                                               // use a format
                                                               // strings too)
                        // TODO: implement keyboard "enter" key detection and
                        // allow enter to use the button (additional feature)
                        if (!std::regex_match(buf1, course_match)) {
                            ImGui::Text("Invalid course code");
                        } else {
                            if (ImGui::Button("CREATE")) {
                                int count = 1;
                                for (auto [courseCode, course] :
                                     db.getCourses()) {
                                    if (buf1 == courseCode.substr(
                                                    0, courseCode.find('-')))
                                        count += 1;
                                }
                                // Load newly created course into .json file
                                // WHY TF DOES NORMAL VARIABLE INTO REFERENCE
                                // WORK AND POINTER INTO USING THE POINTER
                                // DOESNT THIS LANGUAGE SUCKS
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
                                    fmt::format("{}-{:02}", output, count)
                                        .c_str());
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
                                    fmt::format("Students of {}",
                                                active_tabs[n])
                                        .c_str(),
                                    6,
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
                                ImGui::TableNextRow();
                                ImGui::TableSetBgColor(
                                    ImGuiTableBgTarget_RowBg0,
                                    ImGui::GetColorU32(table_header_color));
                                ImGui::TableNextColumn();
                                ImGui::Text("Student ID");
                                ImGui::TableNextColumn();
                                ImGui::Text("First Name");
                                ImGui::TableNextColumn();
                                ImGui::Text("Last Name");
                                ImGui::TableNextColumn();
                                ImGui::Text("Grade");
                                ImGui::TableNextColumn();
                                ImGui::Text("Number of Lates");
                                ImGui::TableNextColumn();
                                ImGui::Text("Address");
                                for (auto student :
                                     db.getCourses()[active_tabs[n]]
                                         .getStudents()) {
                                    ImGui::TableNextColumn();
                                    ImGui::Text(
                                        student->getStudentId().c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::Text(
                                        student->getFirstName().c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::Text(student->getLastName().c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::Text(
                                        to_string(student->getGrade()).c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::Text(
                                        to_string(student->getNumLates())
                                            .c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::Text(student->getAddress().c_str());
                                }
                                ImGui::TableNextColumn();
                                if (ImGui::Button("Add Student to Course?"))
                                    isAddingStudentToCourse = true;
                                if (isAddingStudentToCourse)
                                    addingStudentToCourse(
                                        active_tabs[n],
                                        isAddingStudentToCourse);

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
            // TODO: link login section to .json files
            ImGui::Begin("Log in!", 0, ImGuiCond_FirstUseEver);
            ImGui::Text("Teacher ID");
            static char buf1[64] = "";
            ImGui::InputText(
                "##a", buf1,
                64);  // Display some text (you can use a format strings too)
            ImGui::Text("Password");
            static char password[64] = "";
            ImGui::InputText("##b", password, IM_ARRAYSIZE(password), pwflags1);
            ImGui::SameLine();
            ImGui::Checkbox("Show Password", &showPW1);
            if (ImGui::Button("Sign In")) {
                for (auto [employeeID, teacher] : db.getTeachers()) {
                    if (employeeID == buf1) {
                        logged_in_employee = buf1;
                        show_logged_in_window = true;
                        break;
                    }
                }

                if (!show_log_in_window) ImGui::Text("INVALID EMPLOYEE ID");
                // TODO: Validate account
                // TODO: Save account info
            }
            ImGui::SameLine();
            ImGui::Text("New User?");
            ImGui::SameLine();
            if (ImGui::Button("Create Account")) {
                show_log_in_window = false;
                // TODO: Move to other window
                // TODO: Create account info
                // TODO: Save account info
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
            ImGui::Text("Email");
            static char buf3[64] = "";
            ImGui::InputText(
                "##a", buf3,
                64);  // Display some text (you can use a format strings too)
            ImGui::Text("Password");
            static char password2[64] = "";
            ImGui::InputText("##b", password2, IM_ARRAYSIZE(password2),
                             pwflags2);
            ImGui::SameLine();
            ImGui::Checkbox("Show Password", &showPW2);
            ImGui::Text("Confirm Password");
            static char password3[64] = "";
            ImGui::InputText("##c", password3, IM_ARRAYSIZE(password3),
                             pwflags3);
            ImGui::SameLine();
            ImGui::Checkbox("Show Password##", &showPW3);
            if (ImGui::Button("Sign Up")) {
                // TODO: Check if password and confirm password are equal
                // TODO: Validate account
                // TODO: Save account info
                ImGui::OpenPopup("ACCOUNT CREATION SUCCESSFUL!");
            }
            bool account_creation_success_window = true;
            if (ImGui::BeginPopupModal("ACCOUNT CREATION SUCCESSFUL!",
                                       &account_creation_success_window)) {
                ImGui::Text(
                    "YOUR ACCOUNT HAS BEEN SUCCESSFULLY CREATED. \nLOG IN "
                    "THROUGH THE LOG IN WINDOW!");
                if (ImGui::Button("Close")) {
                    ImGui::CloseCurrentPopup();
                    show_log_in_window = true;
                }
                ImGui::EndPopup();
            }
            if (!account_creation_success_window) show_log_in_window = true;

            if (ImGui::Button("Return To Log In Window")) {
                show_log_in_window = true;
                // TODO: Move to other window
                // TODO: Create account info
                // TODO: Save account info
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