/* file: kernelfunction_linear_parameter.cpp */
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
#include "linear/JParameter.h"
#include "daal.h"

using namespace daal::algorithms::kernel_function::linear;
/*
 * Class:     com_intel_daal_algorithms_kernel_function_linear_Parameter
 * Method:    cSetK
 * Signature:(DD)J
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kernel_1function_linear_Parameter_cSetK
(JNIEnv *env, jobject thisObj, jlong parAddr, jdouble k)
{
    (*(Parameter *)parAddr).k = k;
}
/*
 * Class:     com_intel_daal_algorithms_kernel_function_linear_Parameter
 * Method:    cSetK
 * Signature:(DD)J
 */
JNIEXPORT jdouble JNICALL Java_com_intel_daal_algorithms_kernel_1function_linear_Parameter_cGetK
(JNIEnv *env, jobject thisObj, jlong parAddr)
{
    return(jdouble)(*(Parameter *)parAddr).k;
}

/*
 * Class:     com_intel_daal_algorithms_kernel_function_linear_Parameter
 * Method:    cSetK
 * Signature:(DD)J
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kernel_1function_linear_Parameter_cSetB
(JNIEnv *env, jobject thisObj, jlong parAddr, jdouble b)
{
    (*(Parameter *)parAddr).b = b;
}
/*
 * Class:     com_intel_daal_algorithms_kernel_function_linear_Parameter
 * Method:    cGetB
 * Signature:(DD)J
 */
JNIEXPORT jdouble JNICALL Java_com_intel_daal_algorithms_kernel_1function_linear_Parameter_cGetB
(JNIEnv *env, jobject thisObj, jlong parAddr)
{
    return(jdouble)(*(Parameter *)parAddr).b;
}
