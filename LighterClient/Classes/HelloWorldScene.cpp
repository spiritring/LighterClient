#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#include <netdb.h>
#include "exbuffer.h"
#include <list>
#include <pthread.h>

using namespace cocos2d;
using namespace CocosDenshion;

int hSocket = 0;
HelloWorld* pHW = NULL;
std::list<std::pair<std::string, std::string> > MsgList;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}



int HelloWorld::connect(const char* ip, unsigned short port){
    
    struct sockaddr_in sa;
    struct hostent* hp;
    
    hp = gethostbyname(ip);
    if(!hp){
        return -1;
    }
    memset(&sa, 0, sizeof(sa));
    memcpy((char*)&sa.sin_addr, hp->h_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons(port);
    
    hSocket = socketHandle = socket(sa.sin_family, SOCK_STREAM, 0);
    
    if(socketHandle < 0){
        printf( "failed to create socket\n" );
        return -1;
    }
    if(::connect(socketHandle, (sockaddr*)&sa, sizeof(sa)) < 0){
        printf( "failed to connect socket\n" );
        ::close(socketHandle);
        return -1;
    }
    
    threadStart();
    
    CCLOG("Client connect OK ！ IP: %s:%d ",ip,port);
    
    //char* buffer = "helloworld!";
    //send(socketHandle,buffer,strlen(buffer),0);
    return 0;
}

int okok = 0;

void HelloWorld::draw(){
    pthread_mutex_lock(&mutex);
    CCLog("%d", MsgList.size());
    for (int i = 0; i < MsgList.size(); i++) {
        EventSys.SendMessage("Login", MsgList.begin()->second);
        MsgList.pop_front();
    }
    pthread_mutex_unlock(&mutex);
}

static void recvHandle(unsigned char *rbuf, size_t len)
{
    char buffer[1024] = {0};
    memcpy(buffer, (char*)(rbuf), len);    
    //CCLog("收到数据:%d\n%s",len, buffer);
    
    char sendBuf[1024] = {0};
    memcpy(sendBuf, buffer, 1024);
    unsigned short* BufLen = (unsigned short*)sendBuf;
    *BufLen = _ntohs(len+2, EXBUFFER_BIG_ENDIAN);
    BufLen += 1;
    *BufLen = _ntohs(len, EXBUFFER_BIG_ENDIAN);
    memcpy(sendBuf+4, (char*)(rbuf), len);
    send(hSocket, sendBuf, len+4, 0);
    
    pthread_mutex_lock(&pHW->mutex);
    MsgList.push_back(std::make_pair("Login", "Fuck"));
    pthread_mutex_unlock(&pHW->mutex);
}

static void* thread_function(void *arg) {
    char sBuffer[1024] = {0};
    exbuffer_t* value;
    value = exbuffer_new();
    value->recvHandle = recvHandle;
    
    for (;;) {
        memset(sBuffer, 0, sizeof(sBuffer));
        int bufLen = recv(pHW->socketHandle, &sBuffer, 1024, 0);
        if (bufLen > 0) {
            exbuffer_put(value,(unsigned char*)sBuffer,0,bufLen);
        }
    }
    
    exbuffer_dump(value,value->bufferlen);
    exbuffer_free(&value);
    
    return ((void *) 0);
}

int HelloWorld::threadStart() {
    int errCode = 0;
    do{
        pthread_attr_t tAttr;
        errCode = pthread_attr_init(&tAttr);
        errCode = pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
        
        if (errCode!=0) {
            pthread_attr_destroy(&tAttr);
            break;
        }
        
        
        
        errCode = pthread_create(&threadHimi, &tAttr, thread_function, this);
        
    }while (0);    
    
    return errCode;
}

void HelloWorld::LoginEvent(std::string sData) {
    CCLog("%s",sData.c_str());
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    pHW = this;
        
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Thonburi", 34);

    // ask director the window size
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    // position the label on the center of the screen
    pLabel->setPosition( ccp(size.width / 2, size.height - 20) );

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"
    pSprite = CCSprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    pSprite->setPosition( ccp(size.width/2, size.height/2) );

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    
    pthread_mutex_init(&mutex,NULL);         //在创建线程之前初始化锁！！！
    
    EventSys.RegistEvent("Login", this, &HelloWorld::LoginEvent);
    
    // 网络连接
    connect("192.168.1.120", 9000);
    
    return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
