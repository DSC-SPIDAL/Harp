/* file: transposed_conv2d_parameter.cpp */
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
#include "neural_networks/layers/transposed_conv2d/JTransposedConv2dParameter.h"

#include "daal.h"

#include "common_helpers.h"

USING_COMMON_NAMESPACES();
using namespace daal::algorithms::neural_networks::layers;

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cInit
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cInit
(JNIEnv *env, jobject thisObj)
{
    return (jlong)(new transposed_conv2d::Parameter);
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cGetGroupDimension
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cGetGroupDimension
(JNIEnv *env, jobject thisObj, jlong cParameter)
{
    return (jlong)((((transposed_conv2d::Parameter *)cParameter))->groupDimension);
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cSetGroupDimension
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cSetGroupDimension
(JNIEnv *env, jobject thisObj, jlong cParameter, jlong groupDimension)
{
    (((transposed_conv2d::Parameter *)cParameter))->groupDimension = (size_t)groupDimension;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cgetNKernels
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cgetNKernels
(JNIEnv *env, jobject thisObj, jlong cParameter)
{
    return (jlong)((((transposed_conv2d::Parameter *)cParameter))->nKernels);
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    csetNKernels
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_csetNKernels
(JNIEnv *env, jobject thisObj, jlong cParameter, jlong nKernels)
{
    (((transposed_conv2d::Parameter *)cParameter))->nKernels = (size_t)nKernels;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cgetNGroups
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cgetNGroups
(JNIEnv *env, jobject thisObj, jlong cParameter)
{
    return (jlong)((((transposed_conv2d::Parameter *)cParameter))->nGroups);
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    csetNGroups
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_csetNGroups
(JNIEnv *env, jobject thisObj, jlong cParameter, jlong nGroups)
{
    (((transposed_conv2d::Parameter *)cParameter))->nGroups = (size_t)nGroups;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cSetKernelSize
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cSetKernelSize
(JNIEnv *env, jobject thisObj, jlong cParameter, jlong first, jlong second)
{
    (((transposed_conv2d::Parameter *)cParameter))->kernelSizes.size[0] = first;
    (((transposed_conv2d::Parameter *)cParameter))->kernelSizes.size[1] = second;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cSetValueSizes
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cSetValueSizes
(JNIEnv *env, jobject thisObj, jlong cParameter, jlong first, jlong second)
{
    (((transposed_conv2d::Parameter *)cParameter))->valueSizes.size[0] = first;
    (((transposed_conv2d::Parameter *)cParameter))->valueSizes.size[1] = second;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cSetStride
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cSetStride
(JNIEnv *env, jobject thisObj, jlong cParameter, jlong first, jlong second)
{
    (((transposed_conv2d::Parameter *)cParameter))->strides.size[0] = first;
    (((transposed_conv2d::Parameter *)cParameter))->strides.size[1] = second;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cSetIndices
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cSetIndices
(JNIEnv *env, jobject thisObj, jlong cParameter, jlong first, jlong second)
{
    (((transposed_conv2d::Parameter *)cParameter))->indices.dims[0] = first;
    (((transposed_conv2d::Parameter *)cParameter))->indices.dims[1] = second;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cSetPadding
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cSetPadding
(JNIEnv *env, jobject thisObj, jlong cParameter, jlong first, jlong second)
{
    (((transposed_conv2d::Parameter *)cParameter))->paddings.size[0] = first;
    (((transposed_conv2d::Parameter *)cParameter))->paddings.size[1] = second;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cGetKernelSize
 * Signature: (J)J
 */
JNIEXPORT jlongArray JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cGetKernelSize
(JNIEnv *env, jobject thisObj, jlong cParameter)
{
    jlongArray sizeArray;
    sizeArray = env->NewLongArray(2);
    jlong tmp[2];
    tmp[0] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->kernelSizes.size[0]);
    tmp[1] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->kernelSizes.size[1]);
    env->SetLongArrayRegion(sizeArray, 0, 2, tmp);

    return sizeArray;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cGetValueSizes
 * Signature: (J)J
 */
JNIEXPORT jlongArray JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cGetValueSizes
(JNIEnv *env, jobject thisObj, jlong cParameter)
{
    jlongArray sizeArray;
    sizeArray = env->NewLongArray(2);
    jlong tmp[2];
    tmp[0] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->valueSizes.size[0]);
    tmp[1] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->valueSizes.size[1]);
    env->SetLongArrayRegion(sizeArray, 0, 2, tmp);

    return sizeArray;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cGetKernelSize
 * Signature: (J)J
 */
JNIEXPORT jlongArray JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cGetStride
(JNIEnv *env, jobject thisObj, jlong cParameter)
{
    jlongArray sizeArray;
    sizeArray = env->NewLongArray(2);
    jlong tmp[2];
    tmp[0] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->strides.size[0]);
    tmp[1] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->strides.size[1]);
    env->SetLongArrayRegion(sizeArray, 0, 2, tmp);

    return sizeArray;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cGetKernelSize
 * Signature: (J)J
 */
JNIEXPORT jlongArray JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cGetPadding
(JNIEnv *env, jobject thisObj, jlong cParameter)
{
    jlongArray sizeArray;
    sizeArray = env->NewLongArray(2);
    jlong tmp[2];
    tmp[0] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->paddings.size[0]);
    tmp[1] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->paddings.size[1]);
    env->SetLongArrayRegion(sizeArray, 0, 2, tmp);

    return sizeArray;
}

/*
 * Class:     com_intel_daal_algorithms_neural_networks_layers_transposed_conv2d_TransposedConv2dParameter
 * Method:    cGetKernelSize
 * Signature: (J)J
 */
JNIEXPORT jlongArray JNICALL Java_com_intel_daal_algorithms_neural_1networks_layers_transposed_1conv2d_TransposedConv2dParameter_cGetIndices
(JNIEnv *env, jobject thisObj, jlong cParameter)
{
    jlongArray sizeArray;
    sizeArray = env->NewLongArray(2);
    jlong tmp[2];
    tmp[0] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->indices.dims[0]);
    tmp[1] = (jlong)((((transposed_conv2d::Parameter *)cParameter))->indices.dims[1]);
    env->SetLongArrayRegion(sizeArray, 0, 2, tmp);

    return sizeArray;
}
