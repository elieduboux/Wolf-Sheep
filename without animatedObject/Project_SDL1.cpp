// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
#include "Project_SDL1.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>

void init()
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
        throw std::runtime_error("init():" + std::string(SDL_GetError()));
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
        throw std::runtime_error("init(): SDL_image could not initialize! SDL_image Error: " + std::string(IMG_GetError()));
}
// Defining a namespace without a name -> Anonymous workspace
// Its purpose is to indicate to the compiler that everything
// inside of it is UNIQUELY used within this source file.
namespace
{
    SDL_Surface* load_surface_for(const std::string& path, SDL_Surface* window_surface_ptr) 
    {
        SDL_Surface* optimizedSurface = NULL;
        SDL_Surface* loadedSurface = IMG_Load(path.c_str());
        if (loadedSurface == NULL)
            throw std::runtime_error("Unable to load image " + std::string(path.c_str()) + "!SDL_image Error");
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface(loadedSurface, window_surface_ptr->format, 0);
        if (optimizedSurface == NULL)
            throw std::runtime_error("Unable to optimize image! SDL_image Error");
        SDL_FreeSurface(loadedSurface);
        return optimizedSurface;
    }
} // namespace
//*****************************************************************************
// ***************************** RENDERED OBJECT ******************************
//*****************************************************************************
renderedObject::renderedObject(const std::string& file_path, SDL_Surface* window_surface_ptr, int width, int height, int x, int y)
{
    this->image_ptr_ = load_surface_for(file_path, window_surface_ptr);
    this->window_surface_ptr_ = window_surface_ptr;
    this->width_ = width;
    this->height_ = height;
    this->x_ = x;
    this->y_ = y;
}
/////////////////////////////////////////////
int renderedObject::getHeight() { return this->height_; }
int renderedObject::getWidth() { return this->width_; }
int renderedObject::getX() { return this->x_; }
int renderedObject::getY() { return this->y_; }
/////////////////////////////////////////////
bool renderedObject::theresOverlap(renderedObject* vO2)
{
    return!((this->x_ > vO2->getX() + vO2->getWidth())
        || (this->x_ + this->width_ < vO2->getX())
        || (this->y_ > vO2->getY() + vO2->getHeight())
        || (this->y_ + this->height_ < vO2->getY()));
}
/////////////////////////////////////////////
bool renderedObject::hasInside(int x, int y)
{
    return (this->x_ <= x) && (this->x_ + this->width_ >= x)
        && (this->y_ <= y) && (this->y_ + this->height_ >= y);
}
/////////////////////////////////////////////
int renderedObject::getDistance(renderedObject* vMO) { return this->getDistance(vMO->getX(), vMO->getY()); }
int renderedObject::getDistance(int x, int y)
{
    return  abs((int)(this->x_ - x)) + abs((int)(this->y_ - y));
}
/////////////////////////////////////////////
void renderedObject::draw()
{
    //La position (et pas la taille) de ce rectangle définie l'endroit ou la surface est collée
    SDL_Rect vRect = { (int)this->x_, (int)this->y_,0, 0 };
    SDL_BlitSurface(this->image_ptr_, NULL, this->window_surface_ptr_, &vRect);
}
//*****************************************************************************
// ****************************** MOVING OBJECT *******************************
//*****************************************************************************
movingObject::movingObject(const std::string& file_path, SDL_Surface* window_surface_ptr, int width, int height, int x, int y) :
    renderedObject(file_path, window_surface_ptr, width,  height, x, y)
{
    this->properties_ = {};
    int vTotalVelocity = 3;
    this->xVelocity_ = (rand() % vTotalVelocity*2) - vTotalVelocity;
    this->yVelocity_ = (((rand() % 1) * 2) - 1)* (vTotalVelocity - abs(this->xVelocity_));
}
/////////////////////////////////////////////
void movingObject::goToward(movingObject* vMO) { this->goToward(vMO->getX(), vMO->getY()); }
void movingObject::goToward(int x, int y)
{
    int vXDiff = x - this->x_;
    int vYDiff = y - this->y_;
    while (abs(vXDiff) + abs(vYDiff) > (abs(this->xVelocity_) + abs(this->yVelocity_)))
    {
        vXDiff -= (vXDiff > 0) - (vXDiff < 0);
        if (abs(vXDiff) + abs(vYDiff) > (abs(this->xVelocity_) + abs(this->yVelocity_)))
            vYDiff -= (vYDiff > 0) - (vYDiff < 0);
    }
    this->xVelocity_ = vXDiff;
    this->yVelocity_ = vYDiff;
}
/////////////////////////////////////////////
void movingObject::runAway(movingObject* vMO) { this->runAway(vMO->getX(), vMO->getY()); }
void movingObject::runAway(int x, int y)
{
    this->goToward(x, y);
    this->xVelocity_ = -this->xVelocity_;
    this->yVelocity_ = -this->yVelocity_;
}
/////////////////////////////////////////////
void movingObject::verifEdge()
{
    int vTotalVelocity = abs(this->xVelocity_) + abs(this->yVelocity_);
    if ((this->x_ + this->xVelocity_ + this->width_ > frame_width) || (this->x_ + this->xVelocity_ < 3) 
    || (this->y_ + this->yVelocity_ + this->height_ > frame_height) || (this->y_ + this->yVelocity_ < 3))
    {
        this->xVelocity_ = (rand() % vTotalVelocity);
        this->yVelocity_ = vTotalVelocity - abs(this->xVelocity_);
        if ((this->x_ + this->xVelocity_ + this->width_ > frame_width) || (this->x_ + this->xVelocity_ < 3))
            this->xVelocity_ = -this->xVelocity_;
        if ((this->y_ + this->yVelocity_ + this->height_ > frame_height) || (this->y_ + this->yVelocity_ < 3))
            this->yVelocity_ = -this->yVelocity_;
    }
}
/////////////////////////////////////////////
void movingObject::addPropertie(std::string pPropertie)
{
    this->properties_.push_back(pPropertie);
}
/////////////////////////////////////////////
bool movingObject::hasPropertie(std::string pPropertie)
{
    return (std::find(this->properties_.begin(), this->properties_.end(), pPropertie) != this->properties_.end()); 
}
/////////////////////////////////////////////
bool movingObject::removePropertie(std::string pPropertie)
{
    std::vector<std::string>::iterator itr = std::find(this->properties_.begin(), this->properties_.end(), pPropertie);
    if (itr != this->properties_.end())
    {
        this->properties_.erase(itr);
        return true;
    }
    return false;
}
/////////////////////////////////////////////
void movingObject::interact(movingObject* pO2)
{
    if (this->hasPropertie("wolf"))
    {
        if (pO2->hasPropertie("dog") && this->getDistance(pO2) < 200)
            this->runAway(pO2);
        else if (pO2->hasPropertie("prey") && this->theresOverlap(pO2))
        {
            this->addPropertie("full");
            pO2->addPropertie("dead");
        }
        else if (pO2->hasPropertie("prey"))
            this->goToward(pO2);
    }
    else if (this->hasPropertie("prey") && pO2->hasPropertie("wolf") && this->getDistance(pO2) < 200)
    {
        this->runAway(pO2);
        if (this->removePropertie("canboost"))
            this->addPropertie("boost");
    }
    else if (this->hasPropertie("dog") && !this->hasPropertie("go") && pO2->hasPropertie("shepherd") && this->getDistance(pO2) > 150)
        this->goToward(pO2);
    else if (this->hasPropertie("canprocreate") && this->hasPropertie("male")
        && pO2->hasPropertie("canprocreate") && pO2->hasPropertie("female") && this->theresOverlap(pO2))
    {
        this->removePropertie("canprocreate");
        pO2->removePropertie("canprocreate");
        this->addPropertie("hasprocreate");
        pO2->addPropertie("hasprocreate");
        pO2->addPropertie("pregnant");
    }
}

//*****************************************************************************
// ********************************* SHEPERD **********************************
//*****************************************************************************
shepherd::shepherd(SDL_Surface* window_surface_ptr) :
    movingObject("media/shepherd.png", window_surface_ptr, 49, 49, frame_width / 2, frame_height / 2)
{
    this->properties_ = { "shepherd" };
}
void shepherd::update()
{
    this->move();
    this->draw();
}
void shepherd::move()
{
    const uint8_t* keystate = SDL_GetKeyboardState(0);
    //Horizontal
    if (keystate[SDL_SCANCODE_LEFT]) { this->xVelocity_ = -4; }
    else if (keystate[SDL_SCANCODE_RIGHT]) { this->xVelocity_ = 4; }
    else { this->xVelocity_ = 0; }
    if (this->x_ + this->xVelocity_ > 3 && this->x_ + this->xVelocity_ + this->width_ < frame_width)
        this->x_ += this->xVelocity_;
    //Vertical
    if (keystate[SDL_SCANCODE_UP]) { this->yVelocity_ = -4; }
    else if (keystate[SDL_SCANCODE_DOWN]) { this->yVelocity_ = 4; }
    else { this->yVelocity_ = 0; }
    if (this->y_ + this->yVelocity_ > 3 && this->y_ + this->yVelocity_ + this->height_ < frame_height)
        this->y_ += this->yVelocity_;
}

//*****************************************************************************
//************************************ DOG ************************************
//*****************************************************************************
dog::dog(SDL_Surface* window_surface_ptr) :
    movingObject("media/dog.png", window_surface_ptr, 49, 49, (rand() % (frame_width - 49 - 3) + 3), (rand() % (frame_height - 49 - 3) + 3))
{
    this->properties_ = { "dog"};
}
/////////////////////////////////////////////
void dog::setXTarget(int x){int vXMax = frame_width - this->width_; this->xTarget_ = std::min(vXMax, x);}
void dog::setYTarget(int y) {int vYMax = frame_height - this->height_;this->yTarget_ = std::min(vYMax, y);}
/////////////////////////////////////////////
void dog::update()
{
    this->updateTarget();
    this->move();
    this->draw();
}
/////////////////////////////////////////////
void dog::move()
{ 
    this->verifEdge();
    this->x_ += this->xVelocity_;
    this->y_ += this->yVelocity_;
}
/////////////////////////////////////////////
void dog::updateTarget()
{
    if (this->hasPropertie("go") && this->getDistance(this->xTarget_, this->yTarget_) < 5)
        this->removePropertie("go");
    else if (this->hasPropertie("go"))
        this->goToward(this->xTarget_, this->yTarget_);
    SDL_Rect vRect = { this->x_ - 2,this->y_ - 2, this->width_ + 4,this->height_ + 4 };
    if (this->hasPropertie("clicked"))
        SDL_FillRect(this->window_surface_ptr_, &vRect, 0xFF0000);
    if (this->hasPropertie("go"))
        SDL_FillRect(this->window_surface_ptr_, &vRect, 0x0080FF);
}
//*****************************************************************************
//*********************************** SHEEP ***********************************
//*****************************************************************************
sheep::sheep(SDL_Surface* window_surface_ptr, int x, int y) :
    movingObject("media/sheep.png", window_surface_ptr, 67, 71, x, y)
{
    this->cooldown_ = 0;
    this->boostTime_ = 0;
    this->procreateTime_ = 0;
    std::string vGender[] = { "male","female" };
    int vGenderNbr = rand() % 2;
    if (vGenderNbr == 1)
        this->image_ptr_ = load_surface_for("media/sheep2.png", window_surface_ptr);
    this->properties_ = { "sheep","prey", vGender[vGenderNbr]};
}
sheep::sheep(SDL_Surface* window_surface_ptr) :
    sheep(window_surface_ptr, (rand() % (frame_width - 67 - 3) + 3), (rand() % (frame_height - 71 - 3) + 3))
{}
/////////////////////////////////////////////
void sheep::update()
{
    this->updateBoostTime();
    this->updateProcreateTime();
    this->move();
    this->draw();
}
/////////////////////////////////////////////
void sheep::move()
{
    this->verifEdge();
    this->x_ += this->xVelocity_;
    this->y_ += this->yVelocity_;
}
/////////////////////////////////////////////
void sheep::updateBoostTime()
{
    this->cooldown_--;
    this->boostTime_--;
    if (this->cooldown_ <= 0 && !this->hasPropertie("canboost"))
        this->addPropertie("canboost");
    if (this->removePropertie("boost"))
    {
        this->addPropertie("boosted");
        this->cooldown_ = 200;
        this->boostTime_ = 15;
        this->xVelocity_ += 2 * ((this->xVelocity_ > 0) - (this->xVelocity_ < 0));
        this->yVelocity_ += 2 * ((this->yVelocity_ > 0) - (this->yVelocity_ < 0));
    }
    if (this->boostTime_ <= 0 && this->removePropertie("boosted"))
    {
        this->xVelocity_ -= 2 * ((this->xVelocity_ > 0) - (this->xVelocity_ < 0));
        this->yVelocity_ -= 2 * ((this->yVelocity_ > 0) - (this->yVelocity_ < 0));
    }
}
/////////////////////////////////////////////
void sheep::updateProcreateTime()
{
    this->procreateTime_--;
    if (this->removePropertie("hasprocreate"))
        this->procreateTime_ = 500;
    else if (this->procreateTime_ <= 0 && !this->hasPropertie("canprocreate"))
        this->addPropertie("canprocreate");
}
//*****************************************************************************
//*********************************** WOLF ************************************
//*****************************************************************************
wolf::wolf(SDL_Surface* window_surface_ptr, int x, int y) :
    movingObject("media/wolf.png", window_surface_ptr, 62, 42, x, y)
{
    this->lifeTime_ = 500;
    this->properties_ = {"wolf"};
}
wolf::wolf(SDL_Surface* window_surface_ptr) :
    wolf::wolf(window_surface_ptr, (rand() % (frame_width - 62 - 3) +3), (rand() % (frame_height - 42 -3) +3))
{}
/////////////////////////////////////////////
void wolf::update()
{
    this->updateLifeTime();
    this->move();
    this->draw();
}
/////////////////////////////////////////////
void wolf::move()
{
    
    this->verifEdge();
    this->x_ += this->xVelocity_;
    this->y_ += this->yVelocity_;
}
/////////////////////////////////////////////
void wolf::updateLifeTime()
{
    this->lifeTime_--;
    if (this->removePropertie("full"))
        this->lifeTime_ = 500;
    else if (this->lifeTime_ <= 0)
        this->addPropertie("dead");
}

//*****************************************************************************
// ********************************** GROUND **********************************
//*****************************************************************************
ground::ground(SDL_Surface* window_surface_ptr):
    window_surface_ptr_{window_surface_ptr}
{
    this->shepherd_ = new shepherd(this->window_surface_ptr_);
    this->movingObjects_ = {};
}
/////////////////////////////////////////////
void ground::addMovingObject(movingObject* pO)
{
    this->movingObjects_.push_back(pO);
}
/////////////////////////////////////////////
int ground::getScore()
{
    int vScore = 0;
    for (movingObject* vMO : this->movingObjects_)
        if (vMO->hasPropertie("sheep"))
            vScore++;
    return vScore;
}
/////////////////////////////////////////////
bool ground::update()
{
    
    if (this->mouseEvents())
        return true;
    SDL_Rect vRect = { 0,0,frame_width ,frame_height };
    SDL_FillRect(this->window_surface_ptr_, &vRect, 0x04A88D);
    this->updateObjects();
    this->removeDeads();
    this->addNews();
    return false;
}
/////////////////////////////////////////////
void ground::updateObjects()
{
    for (movingObject* vMovingObject : this->movingObjects_)
    {
        for (movingObject* vMovingObject2 : this->movingObjects_)
            if (vMovingObject2 != vMovingObject)
                vMovingObject->interact(vMovingObject2);
        vMovingObject->update();
    }
}
/////////////////////////////////////////////
void ground::removeDeads()
{
    std::vector<movingObject*>::iterator vIt = this->movingObjects_.begin();
    while (vIt != this->movingObjects_.end())
    {
        if ((*vIt)->hasPropertie("dead"))
            vIt = this->movingObjects_.erase(vIt);
        else
            vIt++;
    }
}
/////////////////////////////////////////////
void ground::addNews()
{
    int n = this->movingObjects_.size();
    for (int i = 0; i < n - 1; i++)
    {
        movingObject* vMO = this->movingObjects_[i];
        if (vMO->removePropertie("pregnant"))
            this->addMovingObject(new sheep(this->window_surface_ptr_, vMO->getX(), vMO->getY()));
    }
}
/////////////////////////////////////////////
bool ground::mouseEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_QUIT: return true;
            case SDL_MOUSEBUTTONDOWN:
                for (movingObject* vMovingObject : this->movingObjects_) 
                {
                    if (vMovingObject->hasPropertie("dog") && vMovingObject->hasInside(e.motion.x, e.motion.y))
                    {
                        vMovingObject->addPropertie("clicked");
                        vMovingObject->removePropertie("go");
                    }
                    else if (vMovingObject->hasPropertie("dog") && vMovingObject->removePropertie("clicked"))
                    {
                        vMovingObject->addPropertie("go");
                        dog* vD = (dog*)vMovingObject;
                        vD->setXTarget(e.motion.x);
                        vD->setYTarget(e.motion.y);
                    }
               }
        }
    }
}
//*****************************************************************************
//******************************** APPLICATION ********************************
//*****************************************************************************
application::application(unsigned n_sheep, unsigned n_wolf)
{
    //window_ptr_
    this->window_ptr_ = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED, frame_width, frame_height, 0);
    if (!this->window_ptr_)
        throw std::runtime_error(std::string(SDL_GetError()));
    //window_surface_ptr_
    this->window_surface_ptr_ = SDL_GetWindowSurface(this->window_ptr_);
     SDL_BlitSurface( this->window_surface_ptr_, NULL, this->window_surface_ptr_, NULL);
     if (!this->window_surface_ptr_)
         throw std::runtime_error(std::string(SDL_GetError()));
    SDL_UpdateWindowSurface(this->window_ptr_);
    //ground_
    this->g_ = new ground(this->window_surface_ptr_);
    for (int i = 0; i < n_sheep; i++)
        this->g_->addMovingObject(new sheep(this->window_surface_ptr_));
    for (int i = 0; i < n_wolf; i++)
        this->g_->addMovingObject(new wolf(this->window_surface_ptr_));
    this->g_->addMovingObject(new shepherd(this->window_surface_ptr_));
    for (int i = 0; i < 2; i++)
        this->g_->addMovingObject(new dog(this->window_surface_ptr_));
}
/////////////////////////////////////////////
int application::loop(unsigned period)
{
    auto start = SDL_GetTicks();
    while ((SDL_GetTicks() - start < period*1000)) 
    {
        if (this->g_->update())
            return 1;
        SDL_UpdateWindowSurface(this->window_ptr_);
        SDL_Delay(700*frame_time);
    }
    printf("\nScore : %d\n", this->g_->getScore());
    return 0;
}