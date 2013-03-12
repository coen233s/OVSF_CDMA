/*
 * Configuration.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <ovsf.h>

using namespace std;

class Configuration {
private:
    Configuration();
public:
    WHCode wcCtrl;
    static Configuration& getInstance() {
        static Configuration inst;
        return inst;
    }

    void setControlChannelCode(const WHCode& code);
};

#endif /* CONFIGURATION_H_ */
