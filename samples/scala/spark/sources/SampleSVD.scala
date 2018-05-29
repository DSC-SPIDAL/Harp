/* file: SampleSVD.scala */
//==============================================================================
// Copyright 2017-2018 Intel Corporation
// All Rights Reserved.
//
// If this software  was obtained under  the Intel Simplified  Software License,
// the following terms apply:
//
// The source code,  information and material  ("Material") contained  herein is
// owned by Intel Corporation  or its suppliers or licensors,  and title to such
// Material remains with Intel Corporation  or its suppliers or  licensors.  The
// Material  contains  proprietary  information  of Intel or  its suppliers  and
// licensors.  The Material is protected by worldwide  copyright laws and treaty
// provisions.  No part  of  the  Material  may  be  used,  copied,  reproduced,
// modified, published, uploaded, posted,  transmitted, distributed or disclosed
// in any  way without  Intel's  prior express  written  permission.  No license
// under any patent,  copyright  or other  intellectual  property rights  in the
// Material is  granted   to or  conferred   upon  you,  either   expressly,  by
// implication,  inducement,  estoppel  or  otherwise.  Any  license  under such
// intellectual  property  rights  must be  express  and  approved  by Intel  in
// writing.
//
// Unless otherwise agreed  by Intel in  writing,  you  may not remove  or alter
// this notice  or any other  notice embedded in  Materials by  Intel or Intel's
// suppliers or licensors in any way.
//
//
// If this software was  obtained under  the Apache  License,  Version  2.0 (the
// "License"), the following terms apply:
//
// You may not  use this  file except in  compliance with  the License.  You may
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//
// Unless  required  by  applicable   law or  agreed  to  in  writing,  software
// distributed under the License is  distributed  on an "AS IS"  BASIS,  WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//
// See the  License  for  the  specific   language  governing   permissions  and
// limitations under the License.
//==============================================================================

package DAAL

import org.apache.spark.SparkConf
import org.apache.spark.SparkContext

import daal_for_mllib.SVD

import org.apache.spark.mllib.linalg.Vectors
import org.apache.spark.mllib.linalg.Matrix
import org.apache.spark.mllib.linalg.distributed.RowMatrix

import java.io._

object SampleSVD extends App {
    val conf = new SparkConf().setAppName("Spark SVD")
    val sc = new SparkContext(conf)

    val data = sc.textFile("/Spark/SVD/data/SVD.txt")
    val dataRDD = data.map(s => Vectors.dense(s.split(' ').map(_.toDouble))).cache()

    val nRows = data.count()
    val nCols = data.first.length

    val result = SVD.computeSVD(new RowMatrix(dataRDD, nRows, nCols))

    result.rows.count

    sc.stop()
}
