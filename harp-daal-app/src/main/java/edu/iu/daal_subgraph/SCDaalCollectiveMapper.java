/*
 * Copyright 2013-2017 Indiana University
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package edu.iu.daal_subgraph;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapred.CollectiveMapper;
import java.io.*;
import java.util.*;

import edu.iu.dymoro.Rotator;
import edu.iu.harp.example.IntArrPlus;
import edu.iu.harp.example.DoubleArrPlus;
import edu.iu.harp.partition.Partition;
import edu.iu.harp.partition.Table;
import edu.iu.harp.resource.IntArray;
import edu.iu.harp.resource.DoubleArray;
import edu.iu.harp.resource.LongArray;
import edu.iu.harp.schdynamic.DynamicScheduler;
import edu.iu.harp.schstatic.StaticScheduler;
import edu.iu.daal.*;

// packages from Daal 
import com.intel.daal.algorithms.subgraph.*;
import com.intel.daal.data_management.data.NumericTable;
import com.intel.daal.data_management.data.HomogenNumericTable;
// import com.intel.daal.data_management.data.HomogenBMNumericTable;
import com.intel.daal.data_management.data.SOANumericTable;
import com.intel.daal.data_management.data_source.DataSource;
import com.intel.daal.data_management.data_source.FileDataSource;
import com.intel.daal.services.DaalContext;

/*
 * use key-object; in-memory
 * Instead of using allgather, using rotation
 */
public class SCDaalCollectiveMapper  extends CollectiveMapper<String, String, Object, Object> {
	private int numMappers;
	private int numColor;
	private int isom;
	private int sizeTemplate;
	private String templateFile;
	private String wholeTemplateName;
	private Random rand = new Random();
	private int numMaxThreads;
	private int numThreads;
    private int harpThreads; 
	private int numIteration;
    private int numCores;
    private int tpc;
    private long send_array_limit;
    private boolean rotation_pipeline;
    private String affinity;
	boolean useLocalMultiThread;
    private int vert_num_count =0;
    private int vert_num_count_total = 0;
    private int adj_len = 0;
    private int max_v_id = 0;
    private Graph t;
    private Table<IntArray> abs_ids_table;
    private int[] mapper_id_vertex; 

    //DAAL related
    private static DaalContext daal_Context = new DaalContext();

	@Override
	protected void setup(Context context) throws IOException, InterruptedException {

		LOG.info("start setup");

		Configuration configuration = context.getConfiguration();
    	numMappers = configuration.getInt(SCConstants.NUM_MAPPERS, 10);
    	templateFile = configuration.get(SCConstants.TEMPLATE_PATH);
    	useLocalMultiThread = configuration.getBoolean(SCConstants.USE_LOCAL_MULTITHREAD, true);
    	rotation_pipeline = configuration.getBoolean(SCConstants.ROTATION_PIPELINE, true);

    	LOG.info("init templateFile");
    	LOG.info(templateFile);

    	numThreads =configuration.getInt(SCConstants.THREAD_NUM, 10);

        //always use the maximum hardware threads to load in data and convert data 
        harpThreads = Runtime.getRuntime().availableProcessors();
        LOG.info("Num Threads " + numThreads);
        LOG.info("Num harp load data threads " + harpThreads);

        numCores = configuration.getInt(SCConstants.CORE_NUM, 24);
        affinity = configuration.get(SCConstants.THD_AFFINITY);
        tpc = configuration.getInt(SCConstants.TPC, 2);

        send_array_limit = (configuration.getInt(SCConstants.SENDLIMIT, 250))*1024L*1024L;
        numIteration =configuration.getInt(SCConstants.NUM_ITERATION, 10);
        LOG.info("Harp-DAAL Subgraph Counting Iteration: " + numIteration);

	}

    protected void mapCollective( KeyValReader reader, Context context) throws IOException, InterruptedException {

		LOG.info("Start collective mapper" );
		this.logMemUsage();
		LOG.info("Memory Used: "+ (Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory()));

        long startTime = System.currentTimeMillis();
        LinkedList<String> vFiles = getVFiles(reader);
        try {
            runSC(vFiles, context.getConfiguration(), context);
        } catch (Exception e) {
            LOG.error("Fail to run Subgraph Counting.", e);
        }
        LOG.info("Total execution time: "
                + (System.currentTimeMillis() - startTime));
		
	}

    private void runSC(final LinkedList<String> vFilePaths,
            final Configuration configuration,
            final Context context) throws Exception {

		Configuration conf = context.getConfiguration();

        // ------------------------- read in graph data -------------------------
        // from HDFS to cpp data structure by using libhdfs
		LOG.info("Start read Graph Data");
        long readGraphbegintime = System.currentTimeMillis();

        // Step 1: convert List<String> to int[] and pack them into HomogenNumericTable
        // pass Homogen into input object and trigger input to load in data
        HomogenNumericTable[] file_tables = load_data_names(vFilePaths);
        Distri scAlgorithm = new Distri(daal_Context, Double.class, Method.defaultSC);
        scAlgorithm.input.set(InputId.filenames, file_tables[0]);
        scAlgorithm.input.set(InputId.fileoffset, file_tables[1]);

        scAlgorithm.input.readGraph();

        int localVNum = scAlgorithm.input.getLocalVNum();
        int localMaxV = scAlgorithm.input.getLocalMaxV();
        int localADJlen = scAlgorithm.input.getLocalADJLen(); 

        LOG.info("vert num count local: " + localVNum + "; local max v id: " + localMaxV + "; total local nbrs num: " + localADJlen);

        //allreduce to get the global max v_id 
        Table<IntArray> table_max_id = new Table<>(0, new IntArrMax());
        IntArray array_max_id = IntArray.create(1, false);
        array_max_id.get()[0] = localMaxV;
        table_max_id.addPartition(new Partition<>(0, array_max_id));
        this.allreduce("sc", "get-max-v-id", table_max_id);
        max_v_id = table_max_id.getPartition(0).get().get()[0];
        //load MaxV into daal obj
        scAlgorithm.input.setGlobalMaxV(max_v_id);
        LOG.info("Max vertex id of full graph: " + max_v_id);
        table_max_id.release();
        table_max_id = null;

        int[] local_abs_ids = new int[localVNum];
        HomogenNumericTable local_abs_ids_daal = new HomogenNumericTable(daal_Context, local_abs_ids, localVNum, 1);
        scAlgorithm.input.set(InputId.localV, local_abs_ids_daal);

        //start init daal graph structure
        scAlgorithm.input.initGraph();

        //communication allgather to get all global ids
        abs_ids_table = new Table<>(0, new IntArrPlus());
        IntArray abs_ids_array = new IntArray(local_abs_ids, 0, localVNum);
        abs_ids_table.addPartition(new Partition<>(this.getSelfID(), abs_ids_array));

        this.allgather("sc", "collect all abs ids", abs_ids_table);

        //create an label array to store mapper ids info
        mapper_id_vertex = new int[max_v_id+1];
        for(int p=0; p<this.getNumWorkers();p++)
        {
            // mapper_vertex_array stores abs ids from each mapper
            int[] mapper_vertex_array = abs_ids_table.getPartition(p).get().get();
            for(int q=0;q<mapper_vertex_array.length;q++)
                mapper_id_vertex[mapper_vertex_array[q]] = p;
        }

        // LOG.info("Finish creating mapper-vertex mapping array");
        abs_ids_table = null;

        //clear and free allocated mem
		long readGraphendtime=System.currentTimeMillis();
        LOG.info("Read in Graph Data with time: " + (readGraphendtime - readGraphbegintime) + " ms");

        // ------------------------- read in template data -------------------------
        // from HDFS to cpp data structure by using libhdfs
        // create graph data structure at daal side
		LOG.info("Start read Template Data");
        LinkedList<String> tFilesPaths = new LinkedList<>();
        tFilesPaths.add(templateFile);

        HomogenNumericTable[] t_tables = load_data_names(tFilesPaths);
        scAlgorithm.input.set(InputId.tfilenames, t_tables[0]);
        scAlgorithm.input.set(InputId.tfileoffset, t_tables[1]);

        scAlgorithm.input.readTemplate();
        scAlgorithm.input.initTemplate();

        LOG.info("Finish load templateFile, num_verts: " + scAlgorithm.input.getTVNum() + "; edges: " + scAlgorithm.input.getTENum());

		// ---------------  main computation ----------------------------------
        colorcount_HJ graph_count = new colorcount_HJ();
        graph_count.init(this, context, scAlgorithm, max_v_id, numThreads, numCores, tpc, affinity, false, false, true);

        // // ------------------- generate communication information -------------------
        // // send/recv num and verts 
        
        if (this.getNumWorkers() > 1)
        {
            graph_count.init_comm(mapper_id_vertex, send_array_limit, rotation_pipeline);
            LOG.info("Finish graph_count initialization");
        }

        // --------------------- start counting ---------------------
		long computation_start = System.currentTimeMillis();
        double full_count = 0.0;
        // full_count = graph_count.do_full_count(numIteration);
        //
		long computation_end = System.currentTimeMillis();
        long local_count_time = (computation_end - computation_start);
        long local_comm_time = graph_count.get_comm_time();
        long local_sync_time = graph_count.get_sync_time();
        //
        Table<DoubleArray> time_table = new Table<>(0, new DoubleArrPlus());
        DoubleArray time_array = DoubleArray.create(3, false);
        time_array.get()[0] = (double)local_count_time;
        time_array.get()[1] = (double)local_comm_time;
        time_array.get()[2] = (double)local_sync_time;
        //
        time_table.addPartition(new Partition<>(0, time_array));
        //
        this.allreduce("sc", "get-time", time_table);
        //
        double global_count_time = time_table.getPartition(0).get().get()[0]/this.getNumWorkers(); 
        double global_comm_time = time_table.getPartition(0).get().get()[1]/this.getNumWorkers(); 
        double global_sync_time = time_table.getPartition(0).get().get()[2]/this.getNumWorkers(); 
        //
        LOG.info("Total Counting time: " + global_count_time + " ms" + "; Avg per itr: " + 
                (global_count_time/(double)numIteration) + " ms");
        //  
        //        
        LOG.info("Total comm time: " + global_comm_time + " ms" + "; Avg per itr: "
                + (global_comm_time/(double)numIteration) + " ms");
        //
        LOG.info("Total sync time: " + global_sync_time + " ms" + "; Avg per itr: "
                + (global_sync_time/(double)numIteration) + " ms");
        //
        LOG.info("Time Ratio: Comm: " + (global_comm_time/global_count_time)*100 + " %; Waiting: " 
                + (global_sync_time - global_comm_time)/global_count_time*100 + " %; Local Computation: "
                + (global_count_time - global_sync_time)/global_count_time*100 + " %");

        // // --------------- allreduce the final count from all mappers ---------------
        // //
        Table<DoubleArray> final_count_table = new Table<>(0, new DoubleArrPlus());
        DoubleArray final_count_array = DoubleArray.create(1, false);
        //
        final_count_array.get()[0] = full_count;
        final_count_table.addPartition(new Partition<>(0, final_count_array));
        //
        this.allreduce("sc", "get-final-count", final_count_table);
        //
        full_count = final_count_table.getPartition(0).get().get()[0];
        //
        //formula to compute the prob 
        int t_num_vert = scAlgorithm.input.getTVNum();
        int num_colors = t_num_vert;
        boolean calculate_automorphisms = true;
        //
        double prob_colorful = Util.factorial(num_colors) /
                ( Util.factorial(num_colors - t_num_vert) * Math.pow(num_colors, t_num_vert) );
        //
        int num_auto = calculate_automorphisms ? scAlgorithm.input.getMorphism() : 1;
        double final_count = Math.floor(full_count / (prob_colorful * num_auto) + 0.5);
        //
        LOG.info("Finish counting local color count: " + full_count + "; final alll count: " + final_count);

		//-------------------------------------------------------------------
        scAlgorithm.input.freeInput();
    }

    private LinkedList<String>
        getVFiles(final KeyValReader reader)
        throws IOException, InterruptedException {
        final LinkedList<String> vFiles =
            new LinkedList<>();
        while (reader.nextKeyValue()) {
            final String value =
                reader.getCurrentValue();
            LOG.info("File: " + value);
            vFiles.add(value);
        }
        return vFiles;
    }
    
    /**
     * @brief read in data and store them in Fascia structure
     *
     * @param conf
     * @param vFilePaths
     *
     * @return 
     */
	private void readGraphDataMultiThread( Configuration conf, List<String> vFilePaths, Graph g){

			LOG.info("[BEGIN] SCDaalCollectiveMapper.readGraphDataMultiThread" );

			Table<IntArray> graphData = new Table<>(0, new IntArrPlus());
		    List<GraphLoadTask> tasks = new LinkedList<>();

		    for (int i = 0; i < harpThreads; i++) {
		    	tasks.add(new GraphLoadTask(conf));
		    }

		    DynamicScheduler<String, ArrayList<Partition<IntArray>>, GraphLoadTask> compute
		    	= new DynamicScheduler<>(tasks);

		    compute.start();

		    for (String filename : vFilePaths) {
		    	compute.submit(filename);
		    }

		    ArrayList<Partition<IntArray>> output=null;
		    while (compute.hasOutput()) {
		    	output = compute.waitForOutput();
		    	if(output != null){
		    		ArrayList<Partition<IntArray>> partialGraphDataList = output;
		    		for(Partition<IntArray> partialGraph:partialGraphDataList )
		    		{
		    			graphData.addPartition(partialGraph);
		    		}
		    	}
		    }

		    compute.stop();
		    LOG.info("[END] SCDaalCollectiveMapper.readGraphDataMultiThread" );

            //get num vert and size of adjacent array
            vert_num_count = 0;
            adj_len = 0;
            max_v_id = 0;

            for(int p=0;p<tasks.size();p++)
            {
                vert_num_count += tasks.get(p).get_vert_num_local();
                adj_len += tasks.get(p).get_adjacent_local_num();

                int max_id_local = tasks.get(p).get_max_v_id();
                max_v_id = max_id_local > max_v_id ? max_id_local: max_v_id; 
            }

            //debug
            LOG.info("vert num count local: " + vert_num_count + " total partitions: " 
                    + graphData.getNumPartitions() + "; local max v id: " + max_v_id);

            vert_num_count = graphData.getNumPartitions();
		 
            //allreduce to get the total vert number 
            Table<IntArray> table_max_id = new Table<>(0, new IntArrMax());
            IntArray array_max_id = IntArray.create(1, false);
    
            array_max_id.get()[0] = max_v_id;
            table_max_id.addPartition(new Partition<>(0, array_max_id));
    
            this.allreduce("sc", "get-max-v-id", table_max_id);

            max_v_id = table_max_id.getPartition(0).get().get()[0];

            LOG.info("Max vertex id of full graph: " + max_v_id);
    
            table_max_id.release();
            table_max_id = null;

            //initialize the Graph class
            g.initGraph(vert_num_count, max_v_id, adj_len, graphData);

            //communication allgather to get all global ids
            abs_ids_table = new Table<>(0, new IntArrPlus());
            IntArray abs_ids_array = new IntArray(g.get_abs_v_ids(), 0, g.num_vertices());
            abs_ids_table.addPartition(new Partition<>(this.getSelfID(), abs_ids_array));

            this.allgather("sc", "collect all abs ids", abs_ids_table);

            //create an label array to store mapper ids info
            mapper_id_vertex = new int[max_v_id+1];
            for(int p=0; p<this.getNumWorkers();p++)
            {
                // mapper_vertex_array stores abs ids from each mapper
                int[] mapper_vertex_array = abs_ids_table.getPartition(p).get().get();
                for(int q=0;q<mapper_vertex_array.length;q++)
                    mapper_id_vertex[mapper_vertex_array[q]] = p;
            }

            LOG.info("Finish creating mapper-vertex mapping array");
            abs_ids_table = null;

	}

    /**
     * @brief read in template file
     *
     * @param template
     *
     * @return 
     */
    private void readTemplate(String templateFile, Context context, Graph t) 
    {

        FSDataInputStream in = null;
        BufferedReader fReader = null;

        try {

		    Configuration conf = context.getConfiguration();


            LOG.info("Raw Template url: " + templateFile);
            Path path = new Path(templateFile);
            String template_file = path.toUri().toString();
            LOG.info("Template url: " + template_file);

            Path template_input = new Path(template_file);

            FileSystem fs = template_input.getFileSystem(conf);
            in = fs.open(template_input);
            fReader = new BufferedReader(new InputStreamReader(in), 1048576);

            // BufferedReader fReader = new BufferedReader(new FileReader(f_t));

            int n_g, m_g;

            //get number of nodes (vertice)
            n_g = Integer.parseInt( fReader.readLine() );
            //get number of edges
            m_g = Integer.parseInt( fReader.readLine() );

            LOG.info("templateFile verts: " + n_g + "; edges: " + m_g);

            int[] srcs_g = new int[m_g];
            int[] dsts_g = new int[m_g];

            for(int p = 0; p < m_g; ++p){
                String[] src_dst = fReader.readLine().split("\\s+");
                srcs_g[p] = Integer.parseInt(src_dst[0]  );
                dsts_g[p] = Integer.parseInt(src_dst[1] );
            }

            fReader.close();
            in.close();

            t.initTemplate(n_g, m_g, srcs_g, dsts_g);

            srcs_g = null;
            dsts_g = null;

        } catch (IOException e) {
            e.printStackTrace();
            if (fReader != null)
            {
                try {
                    fReader.close();
                } catch (Exception e1) {
                }

            }

            if (in != null)
            {

                try {
                    in.close();
                } catch (Exception e1) {
                }

            }

            LOG.info("Failed in reading in templateFile");
        }

    }

    /**
     * @brief convert vertice partitioned filenames into DAAL numericTable
     * First HomogenNumericTable stores the chars of file names
     * Second HomogenNumericTable stores the offset of filenames in the 
     * first HomogenNumericTable
     *
     * @param vFilePaths
     *
     * @return 
     */
    private HomogenNumericTable[] load_data_names(LinkedList<String> vFilePaths)
    {
        // LOG.info("Create file array");
        int[][] file_int = new int[vFilePaths.size()][];
        int[] file_offset = new int[vFilePaths.size()+1];

        int itr = 0;
        file_offset[itr] = 0;
        for (String filename : vFilePaths) 
        {
            LOG.info("FileName: " + filename);
            char[] file_char = filename.toCharArray();
            int[] ascii_val_array = new int[file_char.length];
            for(int i=0;i<file_char.length;i++)
            {
                ascii_val_array[i] = (int)(file_char[i]);
                // System.out.print(ascii_val_array[i]+" ");  
            }

            // System.out.println(" ");  

            file_int[itr] = ascii_val_array;
            file_offset[itr+1] = file_offset[itr] + file_char.length;
            itr++;
        }

        int[] ascii_array_total = new int[file_offset[vFilePaths.size()]];
        for(int i=0;i<vFilePaths.size();i++)
           System.arraycopy(file_int[i], 0, ascii_array_total, file_offset[i], file_offset[i+1]-file_offset[i]); 

        LOG.info("Finish file array convert");

        HomogenNumericTable[] file_tables = new HomogenNumericTable[2];
        file_tables[0] = new HomogenNumericTable(daal_Context, ascii_array_total, ascii_array_total.length, 1);
        file_tables[1] = new HomogenNumericTable(daal_Context, file_offset, file_offset.length, 1);

        LOG.info("Finish filename daal table creation");
        return file_tables;

    }
	
    
}