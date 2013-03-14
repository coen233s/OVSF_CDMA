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

//////////////////////////////////////////////////////////////////////////////
// Misc configuration
//////////////////////////////////////////////////////////////////////////////

// Test dynamic code length allocation based on min and max code lengths
#define TEST_CODE_RANGE 1

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
