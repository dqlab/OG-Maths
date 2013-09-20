/**
 * Copyright (C) 2013 - present by OpenGamma Inc. and the OpenGamma group of companies
 *
 * Please see distribution for license.
 */

#include <stdio.h>
#include <stdlib.h>
#include "jvmmanager.hh"
#include "winprint64.h"
#include "exceptions.hh"
#include "debug.h"

namespace convert {

/**
 * JNI_OnLoad
 */

#ifdef __cplusplus
extern "C"
#endif
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void __attribute__ ((unused)) *reserved)
{
  DEBUG_PRINT("JNI_OnLoad called, initializing JVMManager\n");
  try {
    JVMManager::initialize(jvm);
  }
  catch (convertException e)
  {
    DEBUG_PRINT("Exception in JNI_OnLoad: %s\n.", e.what());
    return JNI_ERR;
  }
  DEBUG_PRINT("Successfully initialized JVMManager in JNI_OnLoad\n");
  return JNI_VERSION_1_2;
}

/**
 * JVMManager
 */

void
JVMManager::initialize(JavaVM *jvm)
{
  // FIXME: Is this necessary? What does seeing the VM pointer tell you?
#ifdef __MINGW32__
  unsigned int high SUPPRESS_UNUSED, low SUPPRESS_UNUSED;
  INT64HIGHLOW(jvm, high, low);
  DEBUG_PRINT("vm ptr at 0x%x%x\n", high, low);
#else
  DEBUG_PRINT("vm ptr at 0x%llx\n", (long long unsigned int)jvm);
#endif

  // Attempt to get env for JNI version 1.2
  int ret = jvm->GetEnv((void **)&_env, JNI_VERSION_1_2);
  if (ret)
  {
    throw convertException("Error getting JNI environment.");
  }

  // Set up cached pointers
  _jvm = jvm;
  registerReferences();
}

void
JVMManager::registerReferences()
{
  //
  // REGISTER CLASS REFERENCES
  //

  registerGlobalClassReference("com/opengamma/longdog/datacontainers/OGNumeric", &_OGNumericClazz);
  registerGlobalClassReference("com/opengamma/longdog/datacontainers/OGTerminal", &_OGTerminalClazz);
  registerGlobalClassReference("com/opengamma/longdog/datacontainers/matrix/OGArray", &_OGArrayClazz);
  registerGlobalClassReference("com/opengamma/longdog/datacontainers/ExprTypeEnum", &_OGExprTypeEnumClazz);
  registerGlobalClassReference("com/opengamma/longdog/datacontainers/matrix/OGSparseMatrix", &_OGSparseMatrixClazz);
  registerGlobalClassReference("com/opengamma/longdog/datacontainers/lazy/OGExpr", &_OGExprClazz);
  registerGlobalClassReference("[D", &_BigDDoubleArrayClazz);
  registerGlobalClassReference("com/opengamma/longdog/datacontainers/other/ComplexArrayContainer", &_ComplexArrayContainerClazz);

  //
  // REGISTER METHOD REFERENCES
  //

  registerGlobalMethodReference(&_OGNumericClazz, &_OGNumericClazz_getType, "getType", "()Lcom/opengamma/longdog/datacontainers/ExprTypeEnum;");
  registerGlobalMethodReference(&_OGTerminalClazz, &_OGTerminalClazz_getData, "getData",  "()[D");
  registerGlobalMethodReference(&_OGArrayClazz, &_OGArrayClazz_getRows, "getRows",  "()I");
  registerGlobalMethodReference(&_OGArrayClazz, &_OGArrayClazz_getCols, "getCols",  "()I");
  registerGlobalMethodReference(&_OGSparseMatrixClazz, &_OGSparseMatrixClazz_getColPtr, "getColPtr",  "()[I");
  registerGlobalMethodReference(&_OGSparseMatrixClazz, &_OGSparseMatrixClazz_getRowIdx, "getRowIdx",  "()[I");
  registerGlobalMethodReference(&_OGExprClazz, &_OGExprClazz_getExprs, "getExprs",  "()[Lcom/opengamma/longdog/datacontainers/OGNumeric;");
  registerGlobalMethodReference(&_ComplexArrayContainerClazz, &_ComplexArrayContainerClazz_ctor_DAoA_DAoA, "<init>","([[D[[D)V");

  //
  // REGISTER FIELD REFERENCES
  //

  _OGExprTypeEnumClazz__hashdefined = _env->GetFieldID(_OGExprTypeEnumClazz, "_hashDefined", "J");
  if (_OGExprTypeEnumClazz__hashdefined == 0)
  {
    throw convertException("ERROR: fieldID _hashDefined not found.");
  }
}

JavaVM*
JVMManager::getJVM()
{
  return _jvm;
}

void
JVMManager::registerGlobalMethodReference(jclass * globalRef, jmethodID * methodToSet, const char * methodName, const char * methodSignature)
{
  jmethodID tmp = NULL;
  tmp = _env->GetMethodID(*globalRef, methodName, methodSignature);
  *methodToSet = tmp;
  if (methodToSet == 0)
  {
    DEBUG_PRINT("ERROR: method %s() not found.\n",methodName);
    throw convertException("Method not found");
  }
  else
  {
#ifdef __MINGW32__
  unsigned int high SUPPRESS_UNUSED, low SUPPRESS_UNUSED;
  INT64HIGHLOW(methodToSet, high, low);
  DEBUG_PRINT("Method found %s() 0x%x%x\n", methodName, high, low);
#else
  DEBUG_PRINT("Method found %s() 0x%llx\n", methodName, (long long unsigned int)methodToSet);
#endif
  }
}

void
JVMManager::registerGlobalClassReference(const char * FQclassname, jclass * globalRef)
{
  jclass tmpClass = NULL; // tmp class reference
  // find OGNumeric
  tmpClass = NULL;
  tmpClass = _env->FindClass(FQclassname); // find class
  if(tmpClass==NULL)
  {
    DEBUG_PRINT("Cannot find class %s in JNI_OnLoad.\n", FQclassname);
    throw convertException("Class not found.");
  }

  *globalRef = NULL;
  *globalRef = (jclass) (_env->NewGlobalRef(tmpClass));
  if(*globalRef==NULL)
  {
    DEBUG_PRINT("Cannot create Global reference for %s.\n",FQclassname);
    throw convertException("Cannot create global reference.");
  }
}

/**
 * JVMManager accessors
 */

jclass JVMManager::getOGNumericClazz()
{ return _OGNumericClazz; }
jclass JVMManager::getOGExprClazz()
{ return _OGExprClazz; }
jclass JVMManager::getOGArrayClazz()
{ return _OGArrayClazz; }
jclass JVMManager::getOGTerminalClazz()
{ return _OGTerminalClazz; }
jclass JVMManager::getOGScalarClazz()
{ return _OGScalarClazz; }
jclass JVMManager::getOGSparseMatrixClazz()
{ return _OGSparseMatrixClazz; }
jclass JVMManager::getBigDDoubleArrayClazz()
{ return _BigDDoubleArrayClazz; }
jclass JVMManager::getComplexArrayContainerClazz()
{ return _ComplexArrayContainerClazz; }
jclass JVMManager::getOGExprTypeEnumClazz()
{ return _OGExprTypeEnumClazz; }
jmethodID JVMManager::getOGTerminalClazz_getData()
{ return _OGTerminalClazz_getData; }
jmethodID JVMManager::getOGNumericClazz_getType()
{ return _OGNumericClazz_getType; }
jmethodID JVMManager::getOGExprClazz_getExprs()
{ return _OGExprClazz_getExprs; }
jmethodID JVMManager::getOGExprClazz_getNExprs()
{ return _OGExprClazz_getNExprs; }
jmethodID JVMManager::getOGArrayClazz_getRows()
{ return _OGArrayClazz_getRows; }
jmethodID JVMManager::getOGArrayClazz_getCols()
{ return _OGArrayClazz_getCols; }
jmethodID JVMManager::getOGSparseMatrixClazz_getColPtr()
{ return _OGSparseMatrixClazz_getColPtr; }
jmethodID JVMManager::getOGSparseMatrixClazz_getRowIdx()
{ return _OGSparseMatrixClazz_getRowIdx; }
jmethodID JVMManager::getComplexArrayContainerClazz_ctor_DAoA_DAoA()
{ return _ComplexArrayContainerClazz_ctor_DAoA_DAoA; }
jfieldID JVMManager:: getOGExprTypeEnumClazz__hashdefined()
{ return _OGExprTypeEnumClazz__hashdefined; }


// Instantiation of JVMManager's fields

JavaVM* JVMManager::_jvm;
JNIEnv* JVMManager::_env;
jclass JVMManager::_OGNumericClazz;
jclass JVMManager::_OGExprClazz;
jclass JVMManager::_OGArrayClazz;
jclass JVMManager::_OGTerminalClazz;
jclass JVMManager::_OGScalarClazz;
jclass JVMManager::_OGSparseMatrixClazz;
jclass JVMManager::_BigDDoubleArrayClazz;
jclass JVMManager::_ComplexArrayContainerClazz;
jclass JVMManager::_OGExprTypeEnumClazz;
jmethodID JVMManager::_OGTerminalClazz_getData;
jmethodID JVMManager::_OGNumericClazz_getType;
jmethodID JVMManager::_OGExprClazz_getExprs;
jmethodID JVMManager::_OGExprClazz_getNExprs;
jmethodID JVMManager::_OGArrayClazz_getRows;
jmethodID JVMManager::_OGArrayClazz_getCols;
jmethodID JVMManager::_OGSparseMatrixClazz_getColPtr;
jmethodID JVMManager::_OGSparseMatrixClazz_getRowIdx;
jmethodID JVMManager::_ComplexArrayContainerClazz_ctor_DAoA_DAoA;
jfieldID  JVMManager::_OGExprTypeEnumClazz__hashdefined;


} // namespace convert
