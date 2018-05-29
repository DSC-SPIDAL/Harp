/* file: java_batch_container_service.h */
/*******************************************************************************
* Copyright 2014-2018 Intel Corporation
* All Rights Reserved.
*
* If this  software was obtained  under the  Intel Simplified  Software License,
* the following terms apply:
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*
*
* If this  software  was obtained  under the  Apache License,  Version  2.0 (the
* "License"), the following terms apply:
*
* You may  not use this  file except  in compliance  with  the License.  You may
* obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*
* Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
* distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the   License  for the   specific  language   governing   permissions  and
* limitations under the License.
*******************************************************************************/

/*
//++
//  Implementation of the class that implements common part of container for algorithms
//--
*/
#ifndef __JAVA_BATCH_CONTAINER_SERVICE_H__
#define __JAVA_BATCH_CONTAINER_SERVICE_H__

#include <jni.h>

#include "java_callback.h"
#include "daal_string.h"

namespace daal
{
namespace services
{

using namespace daal::data_management;

class JavaBatchContainerService : public daal::algorithms::AnalysisContainerIface<batch>, public JavaCallback
{
public:
    JavaBatchContainerService(JavaVM *_jvm, jobject _javaObject) : JavaCallback(_jvm, _javaObject), resultStorage(NULL), _input(NULL), _parameter(NULL) {}
    JavaBatchContainerService(const JavaBatchContainerService &other) : JavaCallback(other), resultStorage(NULL), _input(NULL), _parameter(NULL)
    {
        JavaCallback::ThreadLocalStorage tls = _tls.local();
        jint status = jvm->AttachCurrentThread((void **)(&tls.jniEnv), NULL);
        JNIEnv *env = tls.jniEnv;

        jclass javaObjectClass = tls.jniEnv->GetObjectClass(javaObject);
        if(javaObjectClass == 0)
        {
            tls.jniEnv->ThrowNew(tls.jniEnv->FindClass("java/lang/Exception"), "javaObjectClass could not be initialized");
            return;
        }

        jmethodID getInputMethodID = tls.jniEnv->GetMethodID(javaObjectClass, "getCInput", "()J");
        if(getInputMethodID == 0)
        {
            tls.jniEnv->ThrowNew(tls.jniEnv->FindClass("java/lang/Exception"), "getCInput() method ID could not be initialized");
            return;
        }

        _input = (daal::algorithms::Input *)(tls.jniEnv->CallLongMethod(javaObject, getInputMethodID));

        jmethodID getParameterMethodID = tls.jniEnv->GetMethodID(javaObjectClass, "getCParameter", "()J");
        if(getInputMethodID == 0)
        {
            tls.jniEnv->ThrowNew(tls.jniEnv->FindClass("java/lang/Exception"), "getCParameter() method ID could not be initialized");
            return;
        }

        _parameter =  (daal::algorithms::Parameter *)(tls.jniEnv->CallLongMethod(javaObject, getParameterMethodID));

        setArguments(_input, NULL, _parameter);
        if(!tls.is_main_thread)
        {
            status = jvm->DetachCurrentThread();
        }

        _tls.local() = tls;
    }

    virtual ~JavaBatchContainerService() {}

    services::Status compute(const char *javaResultClassName, const char *javaInputClassName = NULL, const char *javaParameterClassName = NULL)
    {
        JavaCallback::ThreadLocalStorage tls = _tls.local();
        jint status = jvm->AttachCurrentThread((void **)(&tls.jniEnv), NULL);
        JNIEnv *env = tls.jniEnv;

        if(javaResultClassName == NULL)
        {
            env->ThrowNew(env->FindClass("java/lang/Exception"), "javaResultClassName could not be empty"); return services::Status();
        }

        jclass javaObjectClass = env->GetObjectClass(javaObject);
        if(javaObjectClass == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "javaObjectClass could not be initialized"); return services::Status(); }

        if(javaInputClassName)
        {
            setInputToJava(env, javaObjectClass, javaObject, javaInputClassName);
        }

        if(javaParameterClassName)
        {
            setParameterToJava(env, javaObjectClass, javaObject, javaParameterClassName);
        }

        setResultToJava(env, javaObjectClass, javaObject, javaResultClassName);

        services::String fullFunctionSignature("()", 2);
        services::String javaResultClassNameString(javaResultClassName, strnlen(javaResultClassName, String::__DAAL_STR_MAX_SIZE));
        fullFunctionSignature.add(javaResultClassNameString);

        jmethodID computeMethodID = env->GetMethodID(javaObjectClass, "compute",  fullFunctionSignature.c_str());
        if(computeMethodID == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "computeMethodID could not be initialized"); return services::Status(); }

        env->CallObjectMethod(javaObject, computeMethodID);

        if(!tls.is_main_thread)
        {
            status = jvm->DetachCurrentThread();
        }

        _tls.local() = tls;
        return services::Status();
    }

    virtual JavaBatchContainerService * cloneImpl() = 0;

    SerializationIfacePtr *resultStorage;
    daal::algorithms::ResultPtr _result;
    daal::algorithms::Input *_input;
    daal::algorithms::Parameter *_parameter;

    void setJavaResult(daal::algorithms::ResultPtr result)
    {
        _result = result;
    }

    virtual void setInputToJava(JNIEnv *env, jclass javaObjectClass, jobject javaObject, const char *javaInputClassName)
    {
        jclass javaInputClass = env->FindClass(javaInputClassName);
        if(javaInputClass == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "javaInputClass could not be initialized"); return; }

        jfieldID inputFieldID = env->GetFieldID(javaObjectClass, "input", javaInputClassName);
        if(inputFieldID == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "inputFieldID could not be initialized"); return; }

        jobject javaInput = env->GetObjectField(javaObject, inputFieldID);
        if(javaInput == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "jobject javaInput could not be initialized"); return; }

        jmethodID setterID = env->GetMethodID(javaInputClass, "setCInput", "(J)V");
        if(setterID == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "ID setterID could not be initialized"); return; }

        env->CallVoidMethod(javaInput, setterID, (jlong)(_in));
    }

    virtual void setParameterToJava(JNIEnv *env, jclass javaObjectClass, jobject javaObject, const char *javaParameterClassName)
    {
        jclass javaParameterClass = env->FindClass(javaParameterClassName);
        if(javaParameterClass == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "javaParameterClass could not be initialized"); return; }

        jfieldID parameterFieldID = env->GetFieldID(javaObjectClass, "parameter", javaParameterClassName);
        if(parameterFieldID == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "parameterFieldID could not be initialized"); return; }

        jobject javaParameter = env->GetObjectField(javaObject, parameterFieldID);
        if(javaParameter == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "jobject javaParameter could not be initialized"); return; }

        jmethodID setterID = env->GetMethodID(javaParameterClass, "setCParameter", "(J)V");
        if(setterID == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "ID setterID could not be initialized"); return; }

        env->CallVoidMethod(javaParameter, setterID, (jlong)(_par));
    }

    virtual void setResultToJava(JNIEnv *env, jclass javaObjectClass, jobject javaObject, const char *javaResultClassName)
    {
        jmethodID getContextID = env->GetMethodID(javaObjectClass, "getContext", "()Lcom/intel/daal/services/DaalContext;");
        if(getContextID == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "getContextID could not be initialized"); return; }

        jobject javaContextObject = env->CallObjectMethod(javaObject, getContextID);
        if(javaContextObject == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "javaContextObject could not be initialized"); return; }

        jclass javaResultClass = env->FindClass(javaResultClassName);
        if(javaResultClass == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "javaResultClass could not be initialized"); return; }

        jmethodID constructResultID = env->GetMethodID(javaResultClass, "<init>", "(Lcom/intel/daal/services/DaalContext;J)V");
        if(constructResultID == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "constructResultID could not be initialized"); return; }

        resultStorage = new SerializationIfacePtr(staticPointerCast<SerializationIface, daal::algorithms::Result>(_result));
        jobject javaResultObject = env->NewObject(javaResultClass, constructResultID, javaContextObject, jlong(resultStorage));
        if(javaResultObject == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "javaResultObject could not be initialized"); return; }

        services::String javaResultClassNameString(javaResultClassName, strnlen(javaResultClassName, String::__DAAL_STR_MAX_SIZE));
        services::String rightPart(")V", 2);
        services::String fullFunctionSignature("(", 1);
        fullFunctionSignature.add(javaResultClassName);
        fullFunctionSignature.add(rightPart);

        jmethodID setResultMethodID = env->GetMethodID(javaObjectClass, "setResult", fullFunctionSignature.c_str());
        if(setResultMethodID == 0) { env->ThrowNew(env->FindClass("java/lang/Exception"), "ResultMethodID could not be initialized"); return; }

        env->CallVoidMethod(javaObject, setResultMethodID, javaResultObject);
    }
};

} // namespace daal::services
} // namespace daal

#endif
