/*
 * RuleNode.cpp
 *
 *  Created on: Mar 17, 2016
 *      Author: sd
 */

#include "RuleNode.h"
#include <string.h>

RuleNode::RuleNode()
{
	memset(phone, 0, PHONE_LEN);
	min = 0;
	max = 0;
	enabled = true;
	alarmed = false;
}

RuleNode::~RuleNode()
{

}

int8_t RuleNode::getMax() const
{
	return max;
}

void RuleNode::setMax(int8_t max)
{
	this->max = max;
}

int8_t RuleNode::getMin() const
{
	return min;
}

void RuleNode::setMin(int8_t min)
{
	this->min = min;
}

const char* RuleNode::getPhone() const
{
	return phone;
}

const ROM& RuleNode::getRom() const
{
	return rom;
}

void RuleNode::setPhone(const char* phone)
{
	uint8_t i = 0;
	for (; ((i < PHONE_LEN) && (phone[i] != '\0')); i++)
		this->phone[i] = phone[i];
	this->phone[i] = '\0';
}

void RuleNode::setRom(const ROM& rom)
{
	this->rom = rom;
}

void RuleNode::zeroing()
{
	rom.zeroing();
	memset(phone, 0, PHONE_LEN);
	min = 0;
	max = 0;
}

RuleNode::RuleNode(const ROM& rom, int8_t min, int8_t max, const char* phone) :
		RuleNode()
{
	setRom(rom);
	setMin(min);
	setMax(max);
	setPhone(phone);
}

bool RuleNode::operator ==(const RuleNode& cmp) const
{
	if (this->rom != cmp.getRom())
		return false;
	if (this->min != cmp.getMin())
		return false;
	if (this->max != cmp.getMax())
		return false;
	if (strcmp(this->phone, cmp.getPhone()) != 0)
		return false;
	return true;
}

bool RuleNode::operator !=(const RuleNode& cmp) const
{
	return !operator ==(cmp);
}

bool RuleNode::isAlarmed() const
{
	return alarmed;
}

void RuleNode::setAlarmed(bool alarmed)
{
	this->alarmed = alarmed;
}

bool RuleNode::isEnabled() const
{
	return enabled;
}

void RuleNode::setEnabled(bool enabled)
{
	this->enabled = enabled;
}
