#include "LoginScene.h"
#include "SimpleAudioEngine.h"
#include "DrawView.h"
#include "CCSWFNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

CCScene* LoginScene::scene()
{
    CCScene *scene = CCScene::create();
    LoginScene *layer = LoginScene::create();
    
    scene->addChild(layer);
    
    return scene;
}

bool LoginScene::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    CCSWFNode *swfBg = CCSWFNode::create("wkszLogin/app_mashaladi.swf");
    swfBg->setPosition(ccp(size.width/2+30, size.height/2-100));
    swfBg->setScale(0.8f);
    swfBg->runAction();
    swfBg->setRepeat(true);
    this->addChild(swfBg,1000);

    // fallenink: 为什么这里一定需要一个其他精灵，才能让动画放得出来？？？
    CCSprite *sptmp = CCSprite::create("wkszLogin/tree.png");
    sptmp->setPosition(ccp(920,80));
    this->addChild(sptmp,-7);
    
    flag = 0;
    return true;
}


