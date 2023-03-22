// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.
#pragma once
#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_image.h"
#include <iostream>
#include <map>
#include <memory>
#include <vector>

// Defintions
constexpr double frame_rate = 60.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 800; // Width of window in pixel
constexpr unsigned frame_height = 700; // Height of window in pixel

// Helper function to initialize SDL
void init();
//*****************************************************************************
// ***************************** RENDERED OBJECT ******************************
//*****************************************************************************
class renderedObject
{
protected:
    renderedObject(const std::string& file_path, SDL_Surface* window_surface_ptr, int width, int height, int x, int y);
    SDL_Surface* window_surface_ptr_;
    SDL_Surface* image_ptr_;
    int width_;
    int height_;
    int x_;
    int y_;
public:
    int getWidth();
    int getHeight();
    int getX();
    int getY();
    void draw();
    bool hasInside(int x, int y);
    bool theresOverlap(renderedObject* pO2);
    int getDistance(renderedObject* pO2);
    int getDistance(int x, int y);
};
//*****************************************************************************
// ****************************** MOVING OBJECT *******************************
//*****************************************************************************
class movingObject : public renderedObject
{
protected:
    std::vector<std::string> properties_;
    int xVelocity_;
    int yVelocity_;
    virtual void move() = 0;
public:
    movingObject(const std::string& file_path, SDL_Surface* window_surface_ptr, int width, int height, int x, int y);
    movingObject() = default;
    ~movingObject() = default;
    virtual void update() = 0;
    void verifEdge();
    bool hasPropertie(std::string pPropertie);
    bool removePropertie(std::string pPropertie);//True if removed
    void addPropertie(std::string pPropertie);
    void interact(movingObject* pO2);
    void runAway(movingObject* pO2);
    void runAway(int x, int y);
    void goToward(movingObject* pO2);
    void goToward(int x, int y);
    
};
//*****************************************************************************
// ********************************* SHEPERD **********************************
//*****************************************************************************
class shepherd : public movingObject
{
private:
    void move();
public:
    shepherd(SDL_Surface* window_surface_ptr);
    ~shepherd() = default;
    void update();
};

//*****************************************************************************
// ************************************ DOG ***********************************
//*****************************************************************************
class dog : public movingObject
{
private:
    int xTarget_;
    int yTarget_;
    void move();
public:
    dog(SDL_Surface* window_surface_ptr);
    ~dog() = default;
    void update();
    void setXTarget(int x);
    void setYTarget(int y);
    void updateTarget();
};

//*****************************************************************************
// ********************************** SHEEP **********************************
//*****************************************************************************
class sheep : public movingObject
{
private:
    int cooldown_;
    int boostTime_;
    int procreateTime_;
    void move();
public:
    sheep(SDL_Surface* window_surface_ptr, int x, int y);
    sheep(SDL_Surface* window_surface_ptr);
    ~sheep() = default;
    void update();
    void updateBoostTime();
    void updateProcreateTime();
};

//*****************************************************************************
// **********************************  WOLF ***********************************
//*****************************************************************************
class wolf: public movingObject
{
private:
    int lifeTime_;
    void move();
public:
    wolf(SDL_Surface* window_surface_ptr, int x, int y);
    wolf(SDL_Surface* window_surface_ptr);
    ~wolf() = default;
    void update();
    void updateLifeTime();
};

//*****************************************************************************
// ********************************** GROUND **********************************
//*****************************************************************************
class ground 
{
private:
    SDL_Surface* window_surface_ptr_;
    shepherd* shepherd_;
    std::vector<movingObject*> movingObjects_;

public:
    ground(SDL_Surface* window_surface_ptr);
    ~ground() = default;
    void addMovingObject(movingObject* pO);
    bool update();//true si quit
    void updateObjects();
    void removeDeads();
    void addNews();
    bool mouseEvents();//true si quit
    int getScore();
};

//*****************************************************************************
// *******************************  APPLICATION  ******************************
//*****************************************************************************
class application 
{
private:
    SDL_Window* window_ptr_;
    SDL_Surface* window_surface_ptr_;
    SDL_Event window_event_;
    ground* g_;

public:
    application(unsigned n_sheep, unsigned n_wolf); // Ctor
    ~application() = default;                       // dtor
    int loop(unsigned period);  
};