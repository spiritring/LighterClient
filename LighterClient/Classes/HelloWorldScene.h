#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "TSEvent.h"

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    int connect(const char* ip, unsigned short port);
    int threadStart();
    
    
    void LoginEvent(std::string sData);
    
    void draw();

    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HelloWorld);
    
public:
    int socketHandle;
    pthread_t threadHimi;
    TSEvent<HelloWorld> EventSys;
    pthread_mutex_t mutex;
    cocos2d::CCSprite* pSprite;
};

#endif // __HELLOWORLD_SCENE_H__
