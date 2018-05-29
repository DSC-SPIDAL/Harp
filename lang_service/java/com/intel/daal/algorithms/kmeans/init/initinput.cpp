/* file: initinput.cpp */
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
#include "daal.h"
#include "common_defines.i"
#include "kmeans/init/JInitInput.h"
#include "kmeans/init/JInitDistributedStep2LocalPlusPlusInput.h"
#include "kmeans/init/JInitDistributedStep4LocalPlusPlusInput.h"
#include "kmeans/init/JInitDistributedStep3MasterPlusPlusInput.h"
#include "kmeans/init/JInitDistributedStep5MasterPlusPlusInput.h"

#include "common_helpers.h"

USING_COMMON_NAMESPACES();
using namespace daal::algorithms::kmeans::init;

/*
* Class:     com_intel_daal_algorithms_kmeans_Input
* Method:    cSetData
* Signature:(JIJ)V
*/
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitInput_cSetData
(JNIEnv *, jobject, jlong inputAddr, jint id, jlong ntAddr)
{
    jniInput<kmeans::init::Input>::set<kmeans::init::InputId, NumericTable>(inputAddr, id, ntAddr);
}

/*
* Class:     com_intel_daal_algorithms_kmeans_Input
* Method:    cGetData
* Signature:(JI)J
*/
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitInput_cGetData
(JNIEnv *, jobject, jlong inputAddr, jint id)
{
    return jniInput<kmeans::init::Input>::get<kmeans::init::InputId, NumericTable>(inputAddr, id);
}

/////////////////////////////////////// plusPlus methods ///////////////////////////////////////////////////////
///////////////////////////////////////   step2Local     ///////////////////////////////////////////////////////
/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep2LocalPlusPlusInput
* Method:    cSetTable
* Signature: (JIJ)V
*/
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep2LocalPlusPlusInput_cSetTable
(JNIEnv *, jobject, jlong inputAddr, jint id, jlong ntAddr)
{
    jniInput<kmeans::init::DistributedStep2LocalPlusPlusInput>::
        set<kmeans::init::DistributedStep2LocalPlusPlusInputId, NumericTable>(inputAddr, id, ntAddr);
}

/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep2LocalPlusPlusInput
* Method:    cGetTable
* Signature: (JI)J
*/
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep2LocalPlusPlusInput_cGetTable
(JNIEnv *, jobject, jlong inputAddr, jint id)
{
    return jniInput<kmeans::init::DistributedStep2LocalPlusPlusInput>::
        get<kmeans::init::DistributedStep2LocalPlusPlusInputId, NumericTable>(inputAddr, id);
}

/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep2LocalPlusPlusInput
* Method:    cSetDataCollection
* Signature: (JIJ)V
*/
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep2LocalPlusPlusInput_cSetDataCollection
(JNIEnv *, jobject, jlong inputAddr, jint id, jlong addr)
{
    jniInput<kmeans::init::DistributedStep2LocalPlusPlusInput>::
        set<kmeans::init::DistributedLocalPlusPlusInputDataId, DataCollection>(inputAddr, id, addr);
}

/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep2LocalPlusPlusInput
* Method:    cGetDataCollection
* Signature: (JI)J
*/
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep2LocalPlusPlusInput_cGetDataCollection
(JNIEnv *, jobject, jlong inputAddr, jint id)
{
    return jniInput<kmeans::init::DistributedStep2LocalPlusPlusInput>::
        get<kmeans::init::DistributedLocalPlusPlusInputDataId, DataCollection>(inputAddr, id);
}

///////////////////////////////////////   step3Master     ///////////////////////////////////////////////////////
/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep3MasterPlusPlusInput
* Method:    cAddInput
* Signature: (JIIJ)V
*/
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep3MasterPlusPlusInput_cAddInput
(JNIEnv *, jobject, jlong inputAddr, jint id, jint key, jlong addr)
{
    jniInput<kmeans::init::DistributedStep3MasterPlusPlusInput>::
        add<kmeans::init::DistributedStep3MasterPlusPlusInputId, NumericTable>(inputAddr, id, key, addr);
}

///////////////////////////////////////   step4Local     ///////////////////////////////////////////////////////
/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep4LocalPlusPlusInput
* Method:    cSetTable
* Signature: (JIJ)V
*/
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep4LocalPlusPlusInput_cSetTable
(JNIEnv *, jobject, jlong inputAddr, jint id, jlong ntAddr)
{
    jniInput<kmeans::init::DistributedStep4LocalPlusPlusInput>::
        set<kmeans::init::DistributedStep4LocalPlusPlusInputId, NumericTable>(inputAddr, id, ntAddr);
}

/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep4LocalPlusPlusInput
* Method:    cGetTable
* Signature: (JI)J
*/
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep4LocalPlusPlusInput_cGetTable
(JNIEnv *, jobject, jlong inputAddr, jint id)
{
    return jniInput<kmeans::init::DistributedStep4LocalPlusPlusInput>::
        get<kmeans::init::DistributedStep4LocalPlusPlusInputId, NumericTable>(inputAddr, id);
}

/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep4LocalPlusPlusInput
* Method:    cSetDataCollection
* Signature: (JIJ)V
*/
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep4LocalPlusPlusInput_cSetDataCollection
(JNIEnv *, jobject, jlong inputAddr, jint id, jlong addr)
{
    jniInput<kmeans::init::DistributedStep4LocalPlusPlusInput>::
        set<kmeans::init::DistributedLocalPlusPlusInputDataId, DataCollection>(inputAddr, id, addr);
}

/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep4LocalPlusPlusInput
* Method:    cGetDataCollection
* Signature: (JI)J
*/
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep4LocalPlusPlusInput_cGetDataCollection
(JNIEnv *, jobject, jlong inputAddr, jint id)
{
    return jniInput<kmeans::init::DistributedStep4LocalPlusPlusInput>::
        get<kmeans::init::DistributedLocalPlusPlusInputDataId, DataCollection>(inputAddr, id);
}

///////////////////////////////////////   step5Master     ///////////////////////////////////////////////////////
/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep5MasterPlusPlusInput
* Method:    cAddInput
* Signature: (JIJ)V
*/
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep5MasterPlusPlusInput_cAddInput
(JNIEnv *, jobject, jlong inputAddr, jint id, jlong ntAddr)
{
    jniInput<kmeans::init::DistributedStep5MasterPlusPlusInput>::
        add<kmeans::init::DistributedStep5MasterPlusPlusInputId, NumericTable>(inputAddr, id, ntAddr);
}

/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep5MasterPlusPlusInput
* Method:    cGetInput
* Signature: (JI)J
*/
JNIEXPORT jlong JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep5MasterPlusPlusInput_cGetInput
(JNIEnv *, jobject, jlong inputAddr, jint id)
{
    return jniInput<kmeans::init::DistributedStep5MasterPlusPlusInput>::
        get<kmeans::init::DistributedStep5MasterPlusPlusInputDataId, SerializationIface>(inputAddr, id);
}

/*
* Class:     com_intel_daal_algorithms_kmeans_init_InitDistributedStep5MasterPlusPlusInput
* Method:    cSetInput
* Signature: (JIJ)V
*/
JNIEXPORT void JNICALL Java_com_intel_daal_algorithms_kmeans_init_InitDistributedStep5MasterPlusPlusInput_cSetInput
(JNIEnv *, jobject, jlong inputAddr, jint id, jlong addr)
{
    jniInput<kmeans::init::DistributedStep5MasterPlusPlusInput>::
        set<kmeans::init::DistributedStep5MasterPlusPlusInputDataId, SerializationIface>(inputAddr, id, addr);
}
