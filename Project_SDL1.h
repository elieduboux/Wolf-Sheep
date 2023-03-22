// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.
#pragma once
#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_image.h"
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <map>

// Defintions
constexpr double frame_rate = 60.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 800; // Width of window in pixel
constexpr unsigned frame_height = 700; // Height of window in pixel

// Helper function to initialize SDL
void init();
//*****************************************************************************
// ********************************** OBJECT **********************************
//*****************************************************************************
class object
{
protected:
    std::vector<std::string> properties_;
public:
    object();

    bool hasPropertie(std::string pPropertie);
    bool removePropertie(std::string pPropertie);//True if removed
    void addPropertie(std::string pPropertie);
};

//*****************************************************************************
// ***************************** RENDERED OBJECT ******************************
//*****************************************************************************
class renderedObject:public object
{
protected:
    static int ImgW;
    static int ImgH;
    SDL_Surface* window_surface_ptr_;
    SDL_Surface* image_ptr_;
    int width_;//de l'image
    int height_;//de l'image
    int x_;//de l'image
    int y_;//de l'image

public:
    renderedObject(const std::string& file_path, SDL_Surface* window_surface_ptr, int width, int height, int x, int y);
    renderedObject() = default;

    int getX();
    int getY();
    void draw();
    
    bool theresOverlap(renderedObject* pO2);
    bool hasInside(int x, int y);
    int getDistance(renderedObject* pO2);
    int getWidthBox();
    int getHeightBox();
    int getXBox();
    int getYBox();
};

//*****************************************************************************
// ****************************** MOVING OBJECT *******************************
//*****************************************************************************
class movingObject : public virtual renderedObject
{
protected:
    int totalVelocity_;
    int xVelocity_;
    int yVelocity_;
public:
    movingObject(int totalVelocity);

    void setRandomVelocitys();
    bool canMoveX();
    bool canMoveY();
    void adjustVelocitys();
    void interact(renderedObject* pO2);
    void runAway(renderedObject* pO2);
    void runAway(int x, int y);
    void goToward(renderedObject* pO2);
    void goToward(int x, int y);

    virtual void update() = 0;
    virtual void move() = 0;
};
//*****************************************************************************
// ***************************** ANIMATED OBJECT ******************************
//*****************************************************************************
class animatedObject : public virtual renderedObject
{
protected:
    std::map<std::string, std::vector<SDL_Surface*>> images_;
    int frameDuration_;//Durée de la frame actuelle
    int frameInterval_;//Nombre d'appelle de update avant d'updateImage
    int frameIndex_;
    
    void setSurfaceMap();
    virtual std::map<std::string, std::vector<std::string>> getPathMap() = 0;
    virtual std::string getImageKey() = 0;

public:
    animatedObject(int frameDuration);

    void updateFrameDuration();
    void nextFrame();
    virtual void update() = 0;
};

//*****************************************************************************
// ********************************* SHEPERD **********************************
//*****************************************************************************
class shepherd : public movingObject
{
private:
    static int ImgW;
    static int ImgH;
    void move();

public:
    shepherd(SDL_Surface* window_surface_ptr);

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
    static int ImgW;
    static int ImgH;
    
public:
    dog(SDL_Surface* window_surface_ptr);

    void setXTarget(int x);
    void setYTarget(int y);
    void updateTarget();
    void update();
    void move();
};

//*****************************************************************************
// ********************************** SHEEP **********************************
//*****************************************************************************
class sheep : public animatedObject, public movingObject
{
private:
    int cooldown_;
    int boostTime_;
    int procreateTime_;
    static int ImgW;
    static int ImgH;

    std::string getImageKey();
    std::map<std::string, std::vector<std::string>> getPathMap();
   
public:
    sheep(SDL_Surface* window_surface_ptr, int x, int y);
    sheep(SDL_Surface* window_surface_ptr);
    
    void updateProcreateTime();
    void updateBoostTime();
    void update();
    void move();
};

//*****************************************************************************
// **********************************  WOLF ***********************************
//*****************************************************************************
class wolf: public animatedObject,public movingObject
{
private:
    int preyDistance_;
    int lifeTime_;
    static int ImgW;
    static int ImgH;

    std::string getImageKey();
    std::map<std::string, std::vector<std::string>> getPathMap();

public:
    wolf(SDL_Surface* window_surface_ptr, int x, int y);
    wolf(SDL_Surface* window_surface_ptr);

    void choosePrey(renderedObject* pO2);
    void updateLifeTime();
    void update();
    void move();
};

//*****************************************************************************
// ********************************** GROUND **********************************
//*****************************************************************************
class ground 
{
private:
    SDL_Surface* window_surface_ptr_;
    SDL_Surface* image_ptr_;
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
    void drawGround();
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