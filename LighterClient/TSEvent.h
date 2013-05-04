//
//  TSEvent.h
//  LighterClient
//
//  Created by TSEnel on 13-5-4.
//
//

#ifndef LighterClient_TSEvent_h
#define LighterClient_TSEvent_h

#include <string>
#include <map>

template<typename T>
class TSEvent {
public:
    typedef void (T::*pClassFun)(std::string);

    
    bool RegistEvent(std::string sEventKey, T* pInstance, pClassFun pFunction) {
        m_MapEvent[sEventKey] = std::make_pair(pInstance, pFunction);
        return true;
    }
    
    bool UnRegistEvent(std::string sEventKey) {
        if (m_MapEvent.count(sEventKey)) {
            m_MapEvent.erase(sEventKey);
            return true;
        }
        return false;
    }
    
    bool SendMessage(std::string sEventKey, std::string sData) {
        if (m_MapEvent.count(sEventKey)) {
            T* pInstance = m_MapEvent[sEventKey].first;
            pClassFun pFun = m_MapEvent[sEventKey].second;
            (pInstance->*pFun)(sData);
            return true;
        }
        return true;
    }

private:
    std::map<std::string, std::pair<T*, pClassFun> > m_MapEvent;
};

#endif









