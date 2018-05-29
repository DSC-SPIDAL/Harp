/* file: compressor.cpp */
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

#include "JCompressor.h"
#include "daal.h"

using namespace daal;
using namespace daal::data_management;

#include "compression_types.i"

JNIEXPORT jlong JNICALL Java_com_intel_daal_data_1management_compression_Compressor_cInit
(JNIEnv *env, jobject, jint method)
{
    jlong compressor = 0;
    switch(method)
    {
    case Zlib:
        compressor = (jlong)(new Compressor<data_management::zlib>());
        break;
    case Lzo:
        compressor = (jlong)(new Compressor<data_management::lzo>());
        break;
    case Rle:
        compressor = (jlong)(new Compressor<data_management::rle>());
        break;
    case Bzip2:
        compressor = (jlong)(new Compressor<data_management::bzip2>());
        break;
    default:
        break;
    }
    return compressor;
}

JNIEXPORT jlong JNICALL Java_com_intel_daal_data_1management_compression_Compressor_cInitParameter
(JNIEnv *env, jobject, jlong comprAddr, jint method)
{
    jlong par = 0;
    switch(method)
    {
    case Zlib:
        par = (jlong) & (((Compressor<data_management::zlib> *)comprAddr)->parameter);
        break;
    case Lzo:
        par = (jlong) & (((Compressor<data_management::lzo> *)comprAddr)->parameter);
        break;
    case Rle:
        par = (jlong) & (((Compressor<data_management::rle> *)comprAddr)->parameter);
        break;
    case Bzip2:
        par = (jlong) & (((Compressor<data_management::bzip2> *)comprAddr)->parameter);
        break;
    default:
        break;
    }
    return par;
}
