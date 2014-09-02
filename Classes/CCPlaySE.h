/*
 * CCPlaySE.h
 *
 *  Created on: 2014/08/11
 *      Author: takyafumin
 */

#ifndef CCPLAYSE_H_
#define CCPLAYSE_H_

#include "cocos2d.h"

class CCPlaySE: public cocos2d::CCActionInstant
{

public:
    CCPlaySE(std::string sound);
    virtual ~CCPlaySE();

    virtual void update(float time);
    virtual CCFiniteTimeAction* reverse(void);
    virtual CCObject* copyWithZone(cocos2d::CCZone* pZone);

    static CCPlaySE* create(std::string sound);

protected:
    std::string m_sound;
};

#endif /* CCPLAYSE_H_ */
