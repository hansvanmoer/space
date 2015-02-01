#include "Session.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <string>

#include "Settings.h"

using namespace Game;

const Scalar Session::MIN_ZOOM_LEVEL{0.00001};

const Scalar Session::MAX_ZOOM_LEVEL{0.1};

Session::ScrollRegion::ScrollRegion() : scrolling(), bounds(), vector(){}

void Session::ScrollRegion::check(int x, int y){
    scrolling = bounds.contains(x,y);
}

Session::Session() : viewPoint_(Position{}, 0.1, ViewMode::STRATEGIC), window_(), settings_(), running_(false), scrollRegions() {}

void Session::startEventLoop() {
    using clock = std::chrono::high_resolution_clock;
    using duration = typename clock::duration;
    using time = typename clock::time_point;

    settings_ = ApplicationSystem<SettingsSystem>::instance().applicationSettings();
    viewPoint_ = ViewPoint{Position
        {0.25, 0.}, 0.5, ViewMode::STRATEGIC};
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
    glVertex3d(100., 0., 0.);
    glVertex3d(100., 100., 0.);
    glEnd();
    viewPoint_.loadProjectionMatrix();
}

void Session::handleEvents() {
    int mouseWheelDelta = 0;
    sf::Event event;
    while (window_.nextEvent(event)) {
        if (event.type == sf::Event::Closed) {
            running_.store(false);
        } else if (event.type == sf::Event::Resized) {
            resize(event.size.width, event.size.height);
        } else if (event.type == sf::Event::MouseWheelMoved) {
            mouseWheelDelta += event.mouseWheel.delta;
        } else if(event.type == sf::Event::MouseMoved){
            for(int i = 0;i < 8; ++i){
                scrollRegions[i].check(event.mouseMove.x, event.mouseMove.y);
            }
        }
    }
    zoom(mouseWheelDelta);
    for(int i = 0; i < 8; ++i){
        if(scrollRegions[i].scrolling){
            viewPoint_.position.x+=scrollRegions[i].vector.x*viewPoint_.zoom;
            viewPoint_.position.y+=scrollRegions[i].vector.y*viewPoint_.zoom;
        }
    }
}

void Session::resize(int width, int height){
    glViewport(0, 0, width, height);
    scrollRegions[0].bounds = Core::Bounds<int>{50, width-50, 0, 50};
    scrollRegions[1].bounds = Core::Bounds<int>{width-50, width, 0, 50};
    scrollRegions[2].bounds = Core::Bounds<int>{width-50, width, 50, height - 50};
    scrollRegions[3].bounds = Core::Bounds<int>{width - 50, width, height - 50, height};
    scrollRegions[4].bounds = Core::Bounds<int>{50, width - 50, height - 50, height};
    scrollRegions[5].bounds = Core::Bounds<int>{0,50, height - 50, height};
    scrollRegions[6].bounds = Core::Bounds<int>{0,50,50,height - 50};
    scrollRegions[7].bounds = Core::Bounds<int>{0,50,0,50};
    
    Scalar cardSpeed = settings_.controlSettings.mouseScrollSpeed;
    Scalar diagSpeed = sqrt(2)*cardSpeed;
    
    scrollRegions[0].vector = Core::Point<Scalar>{0.,-cardSpeed*height};
    scrollRegions[1].vector = Core::Point<Scalar>{-diagSpeed*width, -diagSpeed*height};
    scrollRegions[2].vector = Core::Point<Scalar>{-cardSpeed*width, 0};
    scrollRegions[3].vector = Core::Point<Scalar>{-diagSpeed*width, diagSpeed*height};
    scrollRegions[4].vector = Core::Point<Scalar>{0, cardSpeed*height};
    scrollRegions[5].vector = Core::Point<Scalar>{diagSpeed*width, diagSpeed*height};
    scrollRegions[6].vector = Core::Point<Scalar>{cardSpeed*width, 0};
    scrollRegions[7].vector = Core::Point<Scalar>{diagSpeed*width, -diagSpeed*height};
}

void Session::zoom(int delta) {
    Scalar newZoom = viewPoint_.zoom*=pow(2.0, delta);
    if (newZoom > MAX_ZOOM_LEVEL) {
        newZoom = MAX_ZOOM_LEVEL;
    } else if (newZoom < MIN_ZOOM_LEVEL) {
        newZoom = MIN_ZOOM_LEVEL;
    }
    viewPoint_.zoom = newZoom;
}