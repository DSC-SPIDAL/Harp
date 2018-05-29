/* file: homogen_tensor_byte_buffer_impl.cpp */
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

#include "JHomogenTensorByteBufferImpl.h"
#include "daal.h"
#include "common_helpers_functions.h"

using namespace daal;
using namespace daal::services;
using namespace daal::data_management;

/*
 * Class:     com_intel_daal_data_management_data_HomogenTensorByteBufferImpl
 * Method:    getIndexType
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_intel_daal_data_1management_data_HomogenTensorByteBufferImpl_getIndexType
  (JNIEnv * env, jobject thisObject, jlong cObject)
{
    Tensor *tensor = ((TensorPtr *)cObject)->get();
    int tag = tensor->getSerializationTag();

    if(tag == features::internal::getIndexNumType<float>() + SERIALIZATION_HOMOGEN_TENSOR_ID ||
       tag == features::internal::getIndexNumType<float>() + SERIALIZATION_MKL_TENSOR_ID)
    {
        return (jint)(features::internal::getIndexNumType<float>());
    }
    if(tag == features::internal::getIndexNumType<double>() + SERIALIZATION_HOMOGEN_TENSOR_ID ||
       tag == features::internal::getIndexNumType<double>() + SERIALIZATION_MKL_TENSOR_ID)
    {
        return (jint)(features::internal::getIndexNumType<double>());
    }
    if(tag == features::internal::getIndexNumType<int>() + SERIALIZATION_HOMOGEN_TENSOR_ID)
    {
        return (jint)(features::internal::getIndexNumType<int>());
    }
    if(tag == features::internal::getIndexNumType<long>() + SERIALIZATION_HOMOGEN_TENSOR_ID)
    {
        return (jint)(features::internal::getIndexNumType<long>());
    }
    if(tag == features::internal::getIndexNumType<size_t>() + SERIALIZATION_HOMOGEN_TENSOR_ID)
    {
        return (jint)(features::internal::getIndexNumType<size_t>());
    }

    return (jint)features::DAAL_OTHER_T;
}

/*
 * Class:     com_intel_daal_data_management_data_HomogenTensorByteBufferImpl
 * Method:    getDoubleSubtensorBuffer
 * Signature: (J[JJJLjava/nio/ByteBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_com_intel_daal_data_1management_data_HomogenTensorByteBufferImpl_getDoubleSubtensorBuffer
  (JNIEnv *env, jobject thisObject, jlong cObject, jlongArray jDims, jlong jRangeIdx, jlong jRangeNum, jobject jBuff)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());
    SubtensorDescriptor<double> block;

    jsize len   = env->GetArrayLength(jDims);
    jlong *dimSizes = env->GetLongArrayElements(jDims, 0);
    Collection<size_t> dims;
    for(size_t i=0; i<len; i++)
    {
        dims.push_back( dimSizes[i] );
    }
    env->ReleaseLongArrayElements(jDims, dimSizes, 0);

    DAAL_CHECK_THROW(tensor->getSubtensor(dims.size(), &(dims[0]), (size_t)jRangeIdx, (size_t)jRangeNum, readOnly, block));
    double *data = block.getPtr();
    size_t  size = block.getSize();

    double *dst = (double *)(env->GetDirectBufferAddress(jBuff));

    for(size_t i = 0; i < size; i++)
    {
        dst[i] = data[i];
    }

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }

    DAAL_CHECK_THROW(tensor->releaseSubtensor(block));
    return jBuff;
}

/*
 * Class:     com_intel_daal_data_management_data_HomogenTensorByteBufferImpl
 * Method:    getFloatSubtensorBuffer
 * Signature: (J[JJJLjava/nio/ByteBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_com_intel_daal_data_1management_data_HomogenTensorByteBufferImpl_getFloatSubtensorBuffer
  (JNIEnv *env, jobject thisObject, jlong cObject, jlongArray jDims, jlong jRangeIdx, jlong jRangeNum, jobject jBuff)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());
    SubtensorDescriptor<float> block;

    jsize len   = env->GetArrayLength(jDims);
    jlong *dimSizes = env->GetLongArrayElements(jDims, 0);
    Collection<size_t> dims;
    for(size_t i=0; i<len; i++)
    {
        dims.push_back( dimSizes[i] );
    }
    env->ReleaseLongArrayElements(jDims, dimSizes, 0);

    DAAL_CHECK_THROW(tensor->getSubtensor(dims.size(), &(dims[0]), (size_t)jRangeIdx, (size_t)jRangeNum, readOnly, block));
    float *data = block.getPtr();
    size_t  size = block.getSize();

    float *dst = (float *)(env->GetDirectBufferAddress(jBuff));

    for(size_t i = 0; i < size; i++)
    {
        dst[i] = data[i];
    }

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }

    DAAL_CHECK_THROW(tensor->releaseSubtensor(block));
    return jBuff;
}

/*
 * Class:     com_intel_daal_data_management_data_HomogenTensorByteBufferImpl
 * Method:    getIntSubtensorBuffer
 * Signature: (J[JJJLjava/nio/ByteBuffer;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_com_intel_daal_data_1management_data_HomogenTensorByteBufferImpl_getIntSubtensorBuffer
  (JNIEnv *env, jobject thisObject, jlong cObject, jlongArray jDims, jlong jRangeIdx, jlong jRangeNum, jobject jBuff)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());
    SubtensorDescriptor<int> block;

    jsize len   = env->GetArrayLength(jDims);
    jlong *dimSizes = env->GetLongArrayElements(jDims, 0);
    Collection<size_t> dims;
    for(size_t i=0; i<len; i++)
    {
        dims.push_back( dimSizes[i] );
    }
    env->ReleaseLongArrayElements(jDims, dimSizes, 0);

    DAAL_CHECK_THROW(tensor->getSubtensor(dims.size(), &(dims[0]), (size_t)jRangeIdx, (size_t)jRangeNum, readOnly, block));
    int *data = block.getPtr();
    size_t  size = block.getSize();

    int *dst = (int *)(env->GetDirectBufferAddress(jBuff));

    for(size_t i = 0; i < size; i++)
    {
        dst[i] = data[i];
    }

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }

    DAAL_CHECK_THROW(tensor->releaseSubtensor(block));
    return jBuff;
}

/*
 * Class:     com_intel_daal_data_management_data_HomogenTensorByteBufferImpl
 * Method:    releaseDoubleSubtensorBuffer
 * Signature: (J[JJJLjava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_data_1management_data_HomogenTensorByteBufferImpl_releaseDoubleSubtensorBuffer
  (JNIEnv *env, jobject thisObject, jlong cObject, jlongArray jDims, jlong jRangeIdx, jlong jRangeNum, jobject jBuff)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());
    SubtensorDescriptor<double> block;

    jsize len   = env->GetArrayLength(jDims);
    jlong *dimSizes = env->GetLongArrayElements(jDims, 0);
    Collection<size_t> dims;
    for(size_t i=0; i<len; i++)
    {
        dims.push_back( dimSizes[i] );
    }
    env->ReleaseLongArrayElements(jDims, dimSizes, 0);

    DAAL_CHECK_THROW(tensor->getSubtensor(dims.size(), &(dims[0]), (size_t)jRangeIdx, (size_t)jRangeNum, writeOnly, block));
    double *data = block.getPtr();
    size_t  size = block.getSize();

    double *dst = (double *)(env->GetDirectBufferAddress(jBuff));

    for(size_t i = 0; i < size; i++)
    {
        data[i] = dst[i];
    }

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }

    DAAL_CHECK_THROW(tensor->releaseSubtensor(block));
}

/*
 * Class:     com_intel_daal_data_management_data_HomogenTensorByteBufferImpl
 * Method:    releaseFloatSubtensorBuffer
 * Signature: (J[JJJLjava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_data_1management_data_HomogenTensorByteBufferImpl_releaseFloatSubtensorBuffer
  (JNIEnv *env, jobject thisObject, jlong cObject, jlongArray jDims, jlong jRangeIdx, jlong jRangeNum, jobject jBuff)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());
    SubtensorDescriptor<float> block;

    jsize len   = env->GetArrayLength(jDims);
    jlong *dimSizes = env->GetLongArrayElements(jDims, 0);
    Collection<size_t> dims;
    for(size_t i=0; i<len; i++)
    {
        dims.push_back( dimSizes[i] );
    }
    env->ReleaseLongArrayElements(jDims, dimSizes, 0);

    DAAL_CHECK_THROW(tensor->getSubtensor(dims.size(), &(dims[0]), (size_t)jRangeIdx, (size_t)jRangeNum, writeOnly, block));
    float *data = block.getPtr();
    size_t  size = block.getSize();

    float *dst = (float *)(env->GetDirectBufferAddress(jBuff));

    for(size_t i = 0; i < size; i++)
    {
        data[i] = dst[i];
    }

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }

    DAAL_CHECK_THROW(tensor->releaseSubtensor(block));
}

/*
 * Class:     com_intel_daal_data_management_data_HomogenTensorByteBufferImpl
 * Method:    releaseIntSubtensorBuffer
 * Signature: (J[JJJLjava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_data_1management_data_HomogenTensorByteBufferImpl_releaseIntSubtensorBuffer
  (JNIEnv *env, jobject thisObject, jlong cObject, jlongArray jDims, jlong jRangeIdx, jlong jRangeNum, jobject jBuff)
{
    Tensor *tensor = static_cast<Tensor *>(((SerializationIfacePtr *)cObject)->get());
    SubtensorDescriptor<int> block;

    jsize len   = env->GetArrayLength(jDims);
    jlong *dimSizes = env->GetLongArrayElements(jDims, 0);
    Collection<size_t> dims;
    for(size_t i=0; i<len; i++)
    {
        dims.push_back( dimSizes[i] );
    }
    env->ReleaseLongArrayElements(jDims, dimSizes, 0);

    DAAL_CHECK_THROW(tensor->getSubtensor(dims.size(), &(dims[0]), (size_t)jRangeIdx, (size_t)jRangeNum, writeOnly, block));
    int *data = block.getPtr();
    size_t  size = block.getSize();

    int *dst = (int *)(env->GetDirectBufferAddress(jBuff));

    for(size_t i = 0; i < size; i++)
    {
        data[i] = dst[i];
    }

    if(tensor->getErrors()->size() > 0)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), tensor->getErrors()->getDescription());
    }

    DAAL_CHECK_THROW(tensor->releaseSubtensor(block));
}
