/*
 * NamedObject.h
 *
 *  Created on: Mar 10, 2013
 *      Author: Danke
 */

#ifndef NAMEDOBJECT_H_
#define NAMEDOBJECT_H_

#include <string>

using namespace std;

class NamedObject {
private:
    string m_name;
public:
    NamedObject(const string &name);
    virtual ~NamedObject();

    string &getName() {
        return m_name;
    }
};

#endif /* NAMEDOBJECT_H_ */
