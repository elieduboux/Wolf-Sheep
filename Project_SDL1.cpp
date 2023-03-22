// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
#include "Project_SDL1.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>
#include <map>
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
// ********************************* OBJECT ***********************************
//*****************************************************************************
object::object()
{
    this->properties_ = {};
}
/////////////////////////////////////////////
void object::addPropertie(std::string pPropertie)
{
    this->properties_.push_back(pPropertie);
}
/////////////////////////////////////////////
bool object::hasPropertie(std::string pPropertie)
{
    return (std::find(this->properties_.begin(), this->properties_.end(), pPropertie) != this->properties_.end());
}
/////////////////////////////////////////////
bool object::removePropertie(std::string pPropertie)
{
    std::vector<std::string>::iterator itr = std::find(this->properties_.begin(), this->properties_.end(), pPropertie);
    if (itr != this->properties_.end())
    {
        this->properties_.erase(itr);
        return true;
    }
    return false;
}
//*****************************************************************************
// ***************************** RENDERED OBJECT ******************************
//*****************************************************************************
renderedObject::renderedObject(const std::string& file_path, SDL_Surface* window_surface_ptr, int width, int height, int x, int y):
                object()
{
    this->image_ptr_ = load_surface_for(file_path, window_surface_ptr);
    this->window_surface_ptr_ = window_surface_ptr;
    this->width_ = width;
    this->height_ = height;
    this->x_ = x;
    this->y_ = y;
}
/////////////////////////////////////////////
int renderedObject::getHeightBox() { return this->height_ * 4 / 5 ; }
int renderedObject::getWidthBox() { return this->width_/ 2 ; }
int renderedObject::getXBox() { return this->x_ + (this->width_ - this->getWidthBox()) / 2; }
int renderedObject::getYBox() { return this->y_ + (this->height_ - this->getHeightBox()) / 2;}
int renderedObject::getX() { return this->x_; }
int renderedObject::getY() { return this->y_; }
/////////////////////////////////////////////
int renderedObject::getDistance(renderedObject* pO2) 
{ 
    int xDistance = std::min(abs(this->getXBox() - pO2->getXBox()), abs(this->getXBox() - pO2->getXBox() - pO2->getWidthBox()));
    int yDistance = std::min(abs(this->getYBox() - pO2->getYBox()), abs(this->getYBox() - pO2->getYBox() - pO2->getHeightBox()));
    return std::sqrt(xDistance*xDistance + yDistance*yDistance);
}
/////////////////////////////////////////////
bool renderedObject::theresOverlap(renderedObject* vO2)
{
    return!((this->getXBox() > vO2->getXBox() + vO2->getWidthBox())
         || (this->getXBox() + this->getWidthBox() < vO2->getXBox())
         || (this->getYBox() > vO2->getYBox() + vO2->getHeightBox())
         || (this->getYBox() + this->getHeightBox() < vO2->getYBox()));
}
/////////////////////////////////////////////
bool renderedObject::hasInside(int x, int y)
{
    return (this->x_ <= x) && (this->x_ + this->width_ >= x)
        && (this->y_ <= y) && (this->y_ + this->height_ >= y);
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
movingObject::movingObject(int totalVelocity)
{
    this->totalVelocity_ = totalVelocity;
    this->setRandomVelocitys();
}
/////////////////////////////////////////////
bool movingObject::canMoveX() { return (this->getXBox() + this->xVelocity_ + this->getWidthBox() < frame_width) && (this->getXBox() + this->xVelocity_ > 0); }
bool movingObject::canMoveY() { return (this->getYBox() + this->yVelocity_ + this->getHeightBox() < frame_height) && (this->getYBox() + this->yVelocity_ > 0); }
/////////////////////////////////////////////
void movingObject::goToward(renderedObject* vMO) { this->goToward(vMO->getXBox(), vMO->getYBox()); }
void movingObject::goToward(int x, int y)
{
    this->xVelocity_ = x - this->getXBox();
    this->yVelocity_ = y - this->getYBox();
    this->adjustVelocitys();
}
/////////////////////////////////////////////
void movingObject::runAway(renderedObject* vMO) { this->runAway(vMO->getXBox(), vMO->getYBox()); }
void movingObject::runAway(int x, int y)
{
    this->xVelocity_ =  this->getXBox() - x;
    this->yVelocity_ = this->getYBox() - y;
    this->adjustVelocitys();
}
/////////////////////////////////////////////
void movingObject::setRandomVelocitys()
{
    this->xVelocity_ = (rand() % this->totalVelocity_ * 2) - this->totalVelocity_;
    if (!canMoveX())
        this->xVelocity_ = -this->xVelocity_;
    this->yVelocity_ = (((rand() % 1) * 2) - 1) * (this->totalVelocity_ - abs(this->xVelocity_));
    if (!canMoveY())
        this->yVelocity_ = -this->yVelocity_;
}
/////////////////////////////////////////////
void movingObject::adjustVelocitys()
{
    //Hors map
    while (!canMoveX() && this->xVelocity_ != 0)
        this->xVelocity_ -= (this->xVelocity_ > 0 ? 1 : -1);
    while (!canMoveY() && this->yVelocity_ != 0)
        this->yVelocity_ -= (this->yVelocity_ > 0 ? 1 : -1);
    //Vitesse trop élevé
    while (abs(this->xVelocity_) + abs(this->yVelocity_) > abs(this->totalVelocity_))
    {
        this->xVelocity_ -= (this->xVelocity_ > 0 ? 1 : -1);
        if (abs(this->xVelocity_) + abs(this->yVelocity_) > abs(this->totalVelocity_))
            this->yVelocity_ -= (this->yVelocity_ > 0 ? 1 : -1);
    }
    //Vitesse trop faible
    if (abs(this->xVelocity_) + abs(this->yVelocity_) < abs(this->totalVelocity_))
        this->setRandomVelocitys();
}
/////////////////////////////////////////////
void movingObject::interact(renderedObject* pO2)
{

    if (this->hasPropertie("wolf"))
    {
        if (pO2->hasPropertie("dog") && this->getDistance(pO2) < 150)
        {
            this->addPropertie("scared");
            this->runAway(pO2);
        }
        else if (pO2->hasPropertie("prey") && this->theresOverlap(pO2))
        {
            this->addPropertie("full");
            pO2->addPropertie("dead");
        }
        else if (pO2->hasPropertie("prey") && !this->hasPropertie("scared"))
        {
            wolf* vW = (wolf*)this;
            vW->choosePrey(pO2);
        }
    }
    else if (this->hasPropertie("prey") && pO2->hasPropertie("wolf") && this->getDistance(pO2) < 200)
    {
        this->runAway(pO2);
        if (this->removePropertie("canboost"))
            this->addPropertie("boost");
    }
    else if (this->hasPropertie("dog") && !this->hasPropertie("go") && pO2->hasPropertie("shepherd") && this->getDistance(pO2) > 100)
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
// ***************************** ANIMATED OBJECT ******************************
//*****************************************************************************
animatedObject::animatedObject(int frameInterval)
{
    this->frameInterval_ = frameInterval;
    this->frameDuration_ = frameInterval;
    this->frameIndex_ = 0;
}
/////////////////////////////////////////////
void animatedObject::setSurfaceMap()
{
    this->images_ = {};
    std::map<std::string, std::vector<std::string>> pathMap = this->getPathMap();
    std::map<std::string, std::vector<std::string>>::iterator it;
    for (it = pathMap.begin(); it != pathMap.end(); it++)
    {
        std::string vKey = it->first;
        std::vector<std::string> vPaths = it->second;
        std::vector<SDL_Surface*> vSurfaces = {};
        for (std::string vPath : vPaths)
            vSurfaces.push_back(load_surface_for(vPath, this->window_surface_ptr_));
        this->images_.insert({ vKey,vSurfaces });
    }
}
/////////////////////////////////////////////
void animatedObject::updateFrameDuration()
{
    this->frameDuration_++;
    if (this->frameDuration_ >= this->frameInterval_)
    {
        this->nextFrame();
        this->frameDuration_ = 0;
    }
}
/////////////////////////////////////////////
void animatedObject::nextFrame()
{
    std::string imageKey = this->getImageKey();
    this->frameIndex_++;
    if (this->frameIndex_ >= this->images_.at(imageKey).size())
        this->frameIndex_ = 0;
    this->image_ptr_ = this->images_.at(imageKey)[this->frameIndex_];
}
//*****************************************************************************
// ********************************* SHEPERD **********************************
//*****************************************************************************
int shepherd::ImgW = 49;
int shepherd::ImgH = 49;
shepherd::shepherd(SDL_Surface* window_surface_ptr) :
    renderedObject("media/shepherd.png", window_surface_ptr, shepherd::ImgW, shepherd::ImgH, frame_width / 2, frame_height / 2), movingObject(4)
{
    this->properties_ = { "shepherd" };
}
/////////////////////////////////////////////
void shepherd::update()
{
    this->move();
    this->draw();
}
/////////////////////////////////////////////
void shepherd::move()
{
    const uint8_t* keystate = SDL_GetKeyboardState(0);
    //Horizontal
    if (keystate[SDL_SCANCODE_LEFT]) { this->xVelocity_ = -this->totalVelocity_; }
    else if (keystate[SDL_SCANCODE_RIGHT]) { this->xVelocity_ = this->totalVelocity_; }
    else { this->xVelocity_ = 0; }
    if(canMoveX())
        this->x_ += this->xVelocity_;
    //Vertical
    if (keystate[SDL_SCANCODE_UP]) { this->yVelocity_ = -this->totalVelocity_; }
    else if (keystate[SDL_SCANCODE_DOWN]) { this->yVelocity_ = this->totalVelocity_; }
    else { this->yVelocity_ = 0; }
    if (canMoveY())
        this->y_ += this->yVelocity_;
}
//*****************************************************************************
//************************************ DOG ************************************
//*****************************************************************************
int dog::ImgW = 49;
int dog::ImgH = 49;
dog::dog(SDL_Surface* window_surface_ptr) :
    renderedObject("media/dog.png", window_surface_ptr, dog::ImgW, dog::ImgH, (rand() % (frame_width - dog::ImgW)), (rand() % (frame_height - dog::ImgH))) , movingObject(3)
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
    if (!canMoveX() || !canMoveY())
        this->setRandomVelocitys();
    this->x_ += this->xVelocity_;
    this->y_ += this->yVelocity_;
}
/////////////////////////////////////////////
void dog::updateTarget()
{
    if (this->hasPropertie("go") && abs(this->x_ - this->xTarget_) < 18 && abs(this->y_ - this->yTarget_) <18)
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
int sheep::ImgW = 68;
int sheep::ImgH = 60;
sheep::sheep(SDL_Surface* window_surface_ptr, int x, int y) :
    renderedObject("media/sheep.png", window_surface_ptr, sheep::ImgW, sheep::ImgH, x, y), animatedObject(10), movingObject(3)
{
        this->cooldown_ = 0;
        this->boostTime_ = 0;
        this->procreateTime_ = 0;
        std::string vGender[] = { "male","female" };
        int vGenderNbr = rand() % 2;
        this->properties_ = { "sheep","prey", vGender[vGenderNbr] };
        this->setSurfaceMap();
}
/////////////////////////////////////////////
    sheep::sheep(SDL_Surface * window_surface_ptr) :
        sheep(window_surface_ptr, (rand() % (frame_width - sheep::ImgW)), (rand() % (frame_height - sheep::ImgH)))
{}
/////////////////////////////////////////////
std::map<std::string, std::vector<std::string>> sheep::getPathMap()
{
    std::string p = "media/sheeps/";
    std::vector<std::string> vPNW = {};
    std::vector<std::string> vPNE = {};
    std::vector<std::string> vPSW = {};
    std::vector<std::string> vPSE = {};
    for (int i = 1; i <= 10; i++)
    {
        vPNW.push_back(p + "nw (" + std::to_string(i) + ").png");
        vPNE.push_back(p + "ne (" + std::to_string(i) + ").png");
        vPSW.push_back(p + "sw (" + std::to_string(i) + ").png");
        vPSE.push_back(p + "se (" + std::to_string(i) + ").png");
    }
    std::map<std::string, std::vector<std::string>> pathMap = { {"nw",vPNW},{"ne",vPNE},{"sw",vPSW},{"se",vPSE} };
    return pathMap;
}
/////////////////////////////////////////////
std::string sheep::getImageKey()
{
    if (this->xVelocity_ <= 0 && this->yVelocity_ >= 0) { return "sw"; }
    if (this->xVelocity_ >= 0 && this->yVelocity_ >= 0) { return "se"; }
    if (this->xVelocity_ <= 0 && this->yVelocity_ <= 0) { return "nw"; }
    return "ne";
}
/////////////////////////////////////////////
void sheep::update()
{
    this->updateBoostTime();
    this->updateProcreateTime();
    this->move();
    this->updateFrameDuration();
    this->draw();
}
/////////////////////////////////////////////
void sheep::move()
{
    if (!canMoveX() || !canMoveY())
        this->setRandomVelocitys();
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
int wolf::ImgW = 157;
int wolf::ImgH = 110;
wolf::wolf(SDL_Surface* window_surface_ptr, int x, int y) :
    renderedObject("media/wolf.png", window_surface_ptr, wolf::ImgW, wolf::ImgH, x, y), animatedObject(5), movingObject(3)
{
    this->preyDistance_ = -1;
    this->lifeTime_ = 500;
    this->properties_ = {"wolf"};
    this->setSurfaceMap();
}
/////////////////////////////////////////////
wolf::wolf(SDL_Surface* window_surface_ptr) :
    wolf::wolf(window_surface_ptr, (rand() % (frame_width - wolf::ImgW)), (rand() % (frame_height - wolf::ImgH)))
{}
/////////////////////////////////////////////
void wolf::choosePrey(renderedObject* pO2)
{
    int distance = this->getDistance(pO2);
    if (this->preyDistance_ == -1 || this->preyDistance_ > distance)
    {
        this->preyDistance_ = distance;
        this->goToward(pO2);
    }
}
/////////////////////////////////////////////
std::map<std::string, std::vector<std::string>> wolf::getPathMap()
{
    std::string p = "media/wolfs/";
    std::vector<std::string> vPNW = {};
    std::vector<std::string> vPNE = {};
    std::vector<std::string> vPSW = {};
    std::vector<std::string> vPSE = {};
    for (int i = 1; i <= 12; i++)
    {
        vPNW.push_back(p + "nw (" + std::to_string(i) + ").png");
        vPNE.push_back(p + "ne (" + std::to_string(i) + ").png");
        vPSW.push_back(p + "sw (" + std::to_string(i) + ").png");
        vPSE.push_back(p + "se (" + std::to_string(i) + ").png");
    }
    std::map<std::string, std::vector<std::string>> pathMap = { {"nw",vPNW},{"ne",vPNE},{"sw",vPSW},{"se",vPSE} };
    return pathMap;
}
/////////////////////////////////////////////
std::string wolf::getImageKey()
{
    if (this->xVelocity_ <= 0 && this->yVelocity_ >= 0){return "sw";}
    if (this->xVelocity_ >= 0 && this->yVelocity_ >= 0){return "se";}
    if (this->xVelocity_ <= 0 && this->yVelocity_ <= 0){return "nw";}
    return "ne";
}
/////////////////////////////////////////////
void wolf::update()
{
    this->removePropertie("scared");
    this->preyDistance_ = -1;
    this->updateLifeTime();
    this->move();
    this->updateFrameDuration();
    this->draw();
}
/////////////////////////////////////////////
void wolf::move()
{
    if (!canMoveX() || !canMoveY())
        this->setRandomVelocitys();
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
    this->image_ptr_ = load_surface_for("media/grass.png", window_surface_ptr);
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
    this->drawGround();
    this->updateObjects();
    this->removeDeads();
    this->addNews();
    return false;
}
/////////////////////////////////////////////
void ground::drawGround()
{
    for (int y = 0; y < frame_height; y += 61)
    {
        for (int x = 0; x < frame_width; x += 108)
        {
            SDL_Rect vRect = { x, y, 0 , 0 };
            SDL_BlitSurface(this->image_ptr_, NULL, this->window_surface_ptr_, &vRect);
        }
    }
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
    std::vector<movingObject*>::iterator it = this->movingObjects_.begin();
    while (it != this->movingObjects_.end())
    {
        if ((*it)->hasPropertie("dead"))
            it = this->movingObjects_.erase(it);
        else
            it++;
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
    for (int i = 0; i < 1; i++)
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