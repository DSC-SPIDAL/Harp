/* file: tensor_impl.cpp */
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

#include <jni.h>

#include "JTensorImpl.h"
#include "daal.h"

#include "java_tensor.h"
#include "common_defines.i"
#include "common_helpers_functions.h"

using namespace daal;
using namespace daal::services;
using namespace daal::data_management;

JavaVM* daal::JavaTensorBase::globalJavaVM = NULL;
tbb::enumerable_thread_specific<jobject> daal::JavaTensorBase::globalDaalContext;

/*
 * Class:     com_intel_daal_data_management_data_TensorImpl
 * Method:    cAllocateDataMemory
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_data_1management_data_TensorImpl_cAllocateDataMemory
  (JNIEnv * env, jobject thisObject, jlong cObject)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());

    DAAL_CHECK_THROW(tensor->allocateDataMemory());

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }
}

/*
 * Class:     com_intel_daal_data_management_data_TensorImpl
 * Method:    cFreeDataMemory
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_data_1management_data_TensorImpl_cFreeDataMemory
  (JNIEnv * env, jobject thisObject, jlong cObject)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());

    DAAL_CHECK_THROW(tensor->freeDataMemory());

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }
}

/*
 * Class:     com_intel_daal_data_management_data_TensorImpl
 * Method:    cGetDimensions
 * Signature: (J)[J
 */
JNIEXPORT jlongArray JNICALL Java_com_intel_daal_data_1management_data_TensorImpl_cGetDimensions
  (JNIEnv * env, jobject thisObject, jlong cObject)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());

    Collection<size_t> dims = tensor->getDimensions();

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }

    size_t size = dims.size();

    jlongArray jDims = env->NewLongArray(size);

    for(size_t i=0; i<size; i++)
    {
        jlong val = (jlong)dims[i];
        env->SetLongArrayRegion(jDims, i, 1, &val);
    }

    return jDims;
}

/*
 * Class:     com_intel_daal_data_management_data_TensorImpl
 * Method:    cSetDimensions
 * Signature: (J[J)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_data_1management_data_TensorImpl_cSetDimensions
  (JNIEnv * env, jobject thisObject, jlong cObject, jlongArray jDims)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());

    jsize len   = env->GetArrayLength(jDims);
    jlong *dimSizes = env->GetLongArrayElements(jDims, 0);
    Collection<size_t> dims;
    for(size_t i=0; i<len; i++)
    {
        dims.push_back( dimSizes[i] );
    }
    env->ReleaseLongArrayElements(jDims, dimSizes, 0);

    DAAL_CHECK_THROW(tensor->setDimensions( dims ));

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }
}

/*
 * Class:     com_intel_daal_data_management_data_TensorImpl
 * Method:    cNewJavaTensor
 * Signature: ([JI)J
 */
JNIEXPORT jlong JNICALL Java_com_intel_daal_data_1management_data_TensorImpl_cNewJavaTensor
  (JNIEnv *env, jobject thisObj, jlongArray jDims, jint tag)
{
    JavaVM *jvm;
    // Get pointer to the Java VM interface function table
    jint status = env->GetJavaVM(&jvm);
    if(status != 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), "Error on GetJavaVM");
        return 0;
    }

    jsize len   = env->GetArrayLength(jDims);
    jlong *dimSizes = env->GetLongArrayElements(jDims, 0);
    Collection<size_t> dims;
    for(size_t i=0; i<len; i++)
    {
        dims.push_back( dimSizes[i] );
    }
    env->ReleaseLongArrayElements(jDims, dimSizes, 0);

    // Create C++ object of the class Tensor
    Tensor *tnsr = 0;
    switch(tag){
        case SERIALIZATION_JAVANIO_HOMOGEN_TENSOR_ID:
            tnsr = new daal::JavaTensor<SERIALIZATION_JAVANIO_HOMOGEN_TENSOR_ID>(dims, jvm, thisObj);
            break;
        default:
            break;
    }

    if(tnsr->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tnsr->getErrors()->getDescription());
    }

    return (jlong)(new SerializationIfacePtr(tnsr));
}
