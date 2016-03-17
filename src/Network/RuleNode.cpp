/*
 * RuleNode.cpp
 *
 *  Created on: Mar 17, 2016
 *      Author: sd
 */

#include "RuleNode.h"

RuleNode::RuleNode()
{
	this->zeroing();
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
	for (uint8_t i = 0; ((i < PHONE_LEN) && (phone[i] != '\0')); i++)
		this->phone[i] = phone[i];
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
