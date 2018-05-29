/* file: result.cpp */
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
#include "pivoted_qr_types.i"

#include "JComputeMode.h"
#include "JComputeStep.h"
#include "pivoted_qr/JMethod.h"
#include "pivoted_qr/JResult.h"
#include "pivoted_qr/JResultId.h"

#include "common_helpers.h"

#define matrixQId com_intel_daal_algorithms_pivoted_qr_ResultId_matrixQId
#define matrixRId com_intel_daal_algorithms_pivoted_qr_ResultId_matrixRId
#define permutationMatrixId com_intel_daal_algorithms_pivoted_qr_ResultId_permutationMatrixId

USING_COMMON_NAMESPACES()
using namespace daal::algorithms::pivoted_qr;

/*
 * Class:     Java_com_intel_daal_algorithms_pivoted_1qr_Result
 * Method:    cNewResult
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_pivoted_1qr_Result_cNewResult
(JNIEnv *env, jobject thisObj)
{
    return jniArgument<pivoted_qr::Result>::newObj();
}

/*
 * Class:     com_intel_daal_algorithms_pivoted_1qr_Result
 * Method:    cGetResultTable
 * Signature:(JI)J
 */
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_pivoted_1qr_Result_cGetResultTable
(JNIEnv *env, jobject thisObj, jlong resAddr, jint id)
{
    if ( id == matrixQId )
    {
        return jniArgument<pivoted_qr::Result>::get<pivoted_qr::ResultId, NumericTable>(resAddr, pivoted_qr::matrixQ);
    }
    else if(id == matrixRId)
    {
        return jniArgument<pivoted_qr::Result>::get<pivoted_qr::ResultId, NumericTable>(resAddr, pivoted_qr::matrixR);
    }
    else if(id == permutationMatrixId)
    {
        return jniArgument<pivoted_qr::Result>::get<pivoted_qr::ResultId, NumericTable>(resAddr, pivoted_qr::permutationMatrix);
    }

    return (jlong)0;
}

/*
 * Class:     com_intel_daal_algorithms_pivoted_1qr_Result
 * Method:    cSetResultTable
 * Signature:(JI)J
 */
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_pivoted_1qr_Result_cSetResultTable
(JNIEnv *env, jobject thisObj, jlong resAddr, jint id, jlong ntAddr)
{
    jniArgument<pivoted_qr::Result>::set<pivoted_qr::ResultId, NumericTable>(resAddr, id, ntAddr);
}
