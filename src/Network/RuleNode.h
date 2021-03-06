/*
 * RuleNode.h
 *
 *  Created on: Mar 17, 2016
 *      Author: sd
 */

#ifndef MAINWORKER_RULENODE_H_
#define MAINWORKER_RULENODE_H_

#include "DallasOneWire/ROM.h"
#include <inttypes.h>

#define PHONE_LEN 14

class RuleNode
{
public:
	RuleNode();
	RuleNode(const ROM& rom, int8_t min, int8_t max, const char * phone);
	~RuleNode();

	int8_t getMax() const;
	void setMax(int8_t max);
	int8_t getMin() const;
	void setMin(int8_t min);
	const char* getPhone() const;
	void setPhone(const char * phone);
	const ROM& getRom() const;
	void setRom(const ROM& rom);

	void zeroing();

	bool operator ==(const RuleNode& cmp) const;
	bool operator !=(const RuleNode& cmp) const;

	bool isAlarmed() const;
	void setAlarmed(bool alarmed);
	bool isEnabled() const;
	void setEnabled(bool enabled);

private:
	ROM rom;
	char phone[PHONE_LEN];
	int8_t min;
	int8_t max;

	bool enabled;
	bool alarmed;
};

#endif /* MAINWORKER_RULENODE_H_ */
