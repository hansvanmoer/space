#include "Session.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <string>

#include "Settings.h"

using namespace Game;

Session::Session() : viewPoint_(Position{}, 0.1, ViewMode::STRATEGIC), window_(), running_(false){}

void Session::startEventLoop() {
    using clock = std::chrono::high_resolution_clock;
    using duration = typename clock::duration;
    using time = typename clock::time_point;
    
    viewPoint_ = ViewPoint{Position{0.25, 0.}, 0.5, ViewMode::STRATEGIC};
    std::string title{"Main window"};
    window_.open(Core::UnicodeString{title.begin(), title.end()}, Core::Bounds<int>{0, 800, 0, 600}, 24);
    window_.activateContext();
    std::cout << "running open gl version " << glGetString(GL_VERSION) << std::endl;
    WindowEvent event;
    running_.store(true);
    VideoSettings videoSettings = ApplicationSystem<SettingsSystem>::instance().applicationSettings().videoSettings;
    duration waitBetweenFrames = std::chrono::duration_cast<duration>(std::chrono::milliseconds{1000 / videoSettings.framesPerSecond});
    time lastFrame = clock::now();
    while (running_.load()) {
        handleEvents();
        draw();
        window_.render();
        time afterRender = clock::now();
        duration elapsed = afterRender - lastFrame;
        if (elapsed < waitBetweenFrames) {
            lastFrame = afterRender;
            duration sleepDuration = waitBetweenFrames - elapsed;
            std::this_thread::sleep_for(sleepDuration);
        } else {
            lastFrame = afterRender;
        }
    }
}

void Session::draw() {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor4f(1.f, 0.f, 0.f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBegin(GL_TRIANGLES);
    glVertex3d(0., 0., 0.);
    glVertex3d(1., 0., 0.);
    glVertex3d(1., 1., 0.);
    glEnd();
    viewPoint_.loadProjectionMatrix();
}

void Session::handleEvents() {
    sf::Event event;
    while (window_.nextEvent(event)) {
        if (event.type == sf::Event::Closed) {
            running_.store(false);
        } else if (event.type == sf::Event::Resized) {
            glViewport(0, 0, event.size.width, event.size.height);
        } else if(event.type == sf::Event::MouseWheelMoved){
            
        }
    }
}