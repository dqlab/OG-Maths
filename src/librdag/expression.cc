/**
 * Copyright (C) 2012 - present by OpenGamma Inc. and the OpenGamma group of companies
 *
 * Please see distribution for license.
 */

#include <iostream>
#include "expression.hh"
#include "visitor.hh"

using namespace std;

namespace librdag
{

/*
 * OGNumeric
 */

OGNumeric::OGNumeric() {}

OGNumeric::~OGNumeric()
{
}

void OGNumeric::debug_print()
{
  cout << "Abstract OGNumeric type" << endl;
}

/*
 * OGExpr
 */

OGExpr::OGExpr()
{
  this->_args = nullptr;
}

OGExpr::OGExpr(OGExpr& copy)
{
  this->_args = new std::vector<OGNumeric *>(*copy.getArgs());
}

OGExpr::OGExpr(std::vector<OGNumeric *> *args)
{
  this->_args = args;
}

OGExpr::~OGExpr()
{
  for (std::vector<OGNumeric *>::iterator it = this->_args->begin() ; it != this->_args->end(); it++)
  {
    delete *it;
  }
  delete this->_args;
}

std::vector<OGNumeric *> *
OGExpr::getArgs()
{
	return this->_args;
}

void
OGExpr::setArgs(std::vector<OGNumeric *> * args)
{
	this->_args = args;
}

size_t
OGExpr::getNArgs()
{
  return this->_args->size();
}

void
OGExpr::debug_print()
{
	cout << "OGExpr base class" << endl;
}

void
OGExpr::accept(Visitor &v)
{
  v.visit(this);
}

/**
 * Things that extend OGExpr
 */

OGUnaryExpr::OGUnaryExpr() : OGExpr() {}

OGUnaryExpr::OGUnaryExpr(std::vector<OGNumeric*>* args)
{
  if (args->size() != 1)
  {
    //FIXME: Replace with exception when implemented.
    // For now just die
    exit(1);
  }
  this->setArgs(args);
}

OGUnaryExpr::OGUnaryExpr(OGNumeric* arg)
{
  vector<OGNumeric*> *args = new vector<OGNumeric*>();
  args->push_back(arg);
  this->setArgs(args);
}

OGBinaryExpr::OGBinaryExpr() : OGExpr() {}

OGBinaryExpr::OGBinaryExpr(std::vector<OGNumeric*>* args) {
  if (args->size() != 2)
  {
    //FIXME: Replace with exception when implemented.
    // For now just die
    exit(1);
  }
  this->setArgs(args);
}

OGBinaryExpr::OGBinaryExpr(OGNumeric* left, OGNumeric* right)
{
	vector<OGNumeric*> *args = new vector<OGNumeric*>();
	args->push_back(left);
	args->push_back(right);
	this->setArgs(args);
}

COPY::COPY() : OGUnaryExpr() {}

COPY::COPY(OGNumeric *arg) : OGUnaryExpr(arg) {}

COPY::COPY(std::vector<OGNumeric*>* args): OGUnaryExpr(args) {}

void
COPY::debug_print()
{
	cout << "COPY base class" << endl;
}

PLUS::PLUS() : OGBinaryExpr() {}

PLUS::PLUS(OGNumeric* left, OGNumeric* right) : OGBinaryExpr(left, right) {}

PLUS::PLUS(std::vector<OGNumeric*>* args): OGBinaryExpr(args) {}

void
PLUS::debug_print()
{
	cout << "PLUS base class" << endl;
}

MINUS::MINUS() : OGBinaryExpr() {}

MINUS::MINUS(OGNumeric* left, OGNumeric* right) : OGBinaryExpr(left, right) {}

MINUS::MINUS(std::vector<OGNumeric*>* args): OGBinaryExpr(args) {
}

void
MINUS::debug_print()
{
	cout << "MINUS base class" << endl;
}

SVD::SVD() : OGUnaryExpr() {}

SVD::SVD(std::vector<OGNumeric*>* args): OGUnaryExpr(args) {}

SVD::SVD(OGNumeric* arg): OGUnaryExpr(arg) {}

void
SVD::debug_print()
{
	cout << "SVD base class" << endl;
}

SELECTRESULT::SELECTRESULT() : OGBinaryExpr() {}

SELECTRESULT::SELECTRESULT(std::vector<OGNumeric*>* args): OGBinaryExpr(args) {
  // Check that the second argument is an integer
  if (dynamic_cast<OGIntegerScalar*>((*args)[1]) == NULL)
  {
    // FIXME: Throw exception when exceptions set up. die for now.
    exit(1);
  }
}

SELECTRESULT::SELECTRESULT(OGNumeric* result, OGNumeric* index): OGBinaryExpr(result, index)
{
  if (dynamic_cast<OGIntegerScalar*>((*(this->getArgs()))[1]) == NULL)
  {
    // FIXME: Throw exception when exceptions set up. die for now.
    exit(1);
  }
}

void
SELECTRESULT::debug_print()
{
	printf("SELECTRESULT base class\n");
}

} // namespace librdag