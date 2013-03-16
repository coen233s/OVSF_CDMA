/*
 * Stat.cpp
 *
 *  Created on: Mar 15, 2013
 *      Author: danke
 */

#include <algorithm>
#include <stat/Stat.h>

using namespace std;

Stat::Stat()
: m_sum(0)
{
}

Stat::Stat(const Stat &rhs)
: m_sum(rhs.m_sum)
{
}

Stat &Stat::operator=(const Stat &rhs)
{
	m_sum = rhs.m_sum;
	return *this;
}

Stat::~Stat() {
}

void Stat::clear() {
	m_sum = 0;
}

void Stat::addData(float data)
{
	m_sum += data;
}

void Stat::addMaxData(float data)
{
	m_sum = max(m_sum, data);
}

void Stat::addStat(const Stat &rhs)
{
	m_sum += rhs.m_sum;
}
