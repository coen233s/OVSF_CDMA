/*
 * Stat.h
 *
 *  Created on: Mar 15, 2013
 *      Author: danke
 */

#ifndef STAT_H_
#define STAT_H_

class Stat {
public:
	float m_sum;

	Stat();
	Stat(const Stat &rhs);

	virtual ~Stat();

	void clear();
	void addData(float data);

	void addStat(const Stat &rhs);

	Stat &operator=(const Stat &rhs);
};

#endif /* STAT_H_ */
