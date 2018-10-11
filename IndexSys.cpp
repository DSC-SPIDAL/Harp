#include "IndexSys.hpp"
#include "Graph.hpp"
#include <cassert>

using namespace std;

void IndexSys::initialization(int color_num, int sub_len, Graph** sub_tps, DivideTemplates* divider)
{
    _color_num = color_num;
    _sub_len = sub_len;
    _sub_tps = sub_tps;
    _divider = divider;

    // create the comb choose table
    _comb_table = new int*[_color_num+1];
    for(int i=0;i<_color_num+1;i++)
        _comb_table[i] = new int[_color_num+1];

    for(int i=0;i<_color_num+1;i++)
        for(int j=0;j<_color_num+1;j++)
            _comb_table[i][j] = comb_calc(i,j);

    // create the table to hold the sub-template vert num
    _sub_vert_num = new int[_sub_len];
    for(int i=0;i<_sub_len;i++)
        _sub_vert_num[i] = (*_sub_tps)[i].get_vert_num();

    // create tmp index table 
    gen_index();

    // create tmp color table
    gen_colors();

    // create the hash table
    gen_comb_hash_table();
}

void IndexSys::release()
{

}

void IndexSys::gen_index()
{/*{{{*/
    // for each color val
    _index_tmp = new int***[_color_num];

    for(int i=0;i<(_color_num-1); i++)
    {
        int cur_val = i+2;
        _index_tmp[i] = new int**[(cur_val-1)];
        for(int j=0;j<(cur_val-1); j++)
        {
            int cur_set_len = j+1;
            int cur_comb_val = comb_calc(cur_val, cur_set_len);
            _index_tmp[i][j] = new int*[cur_comb_val];

            int* perm_set = new int[cur_set_len];
            perm_set_init(perm_set, cur_set_len);

            for(int k=0;k<cur_comb_val;k++)
            {
                _index_tmp[i][j][k] = new int[cur_set_len];
                for(int q=0;q<cur_set_len;q++)
                    _index_tmp[i][j][k][q] = perm_set[q] -1;

                // update the next perm_set
                perm_set_next(perm_set, cur_set_len, cur_val);
            }

            delete[] perm_set;

        }

    }

}/*}}}*/

//calculate the val of {n\choose k} 
int IndexSys::comb_calc(int n, int k)
{/*{{{*/
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;

    int result = n;
    for( int i = 2; i <= k; ++i  ) {
        result *= (n-i+1);
        result /= i;

    }

    return result;
}/*}}}*/

void IndexSys::perm_set_init(int*& perm_set, int size)
{/*{{{*/
    for(int i=0;i<size;i++)
        perm_set[i] = i+1;
}/*}}}*/

void IndexSys::perm_set_next(int*& perm_set, int size, int colors)
{/*{{{*/
    for(int i=size-1;i>=0;i--)
    {
        if (perm_set[i] < colors - (size - i - 1))
        {
            perm_set[i]++;
            for(int j=i+1; j<size;j++)
                perm_set[j] = perm_set[j-1] + 1;


            break;
        }
    }

}/*}}}*/

void IndexSys::gen_colors()
{/*{{{*/
    _colors_tmp = new int****[_sub_len];

    for(int s=0; s<_sub_len; s++)
    {
        int cur_sub_size = (*_sub_tps)[s].get_vert_num();

        if (cur_sub_size > 1)
        {

            int comb_num = comb_calc(_color_num, cur_sub_size);
            _colors_tmp[s] = new int***[comb_num];

            int* perm_colors = new int[cur_sub_size];
            perm_set_init(perm_colors, cur_sub_size);

            for(int n=0;n<comb_num;n++)
            {
                int comb_node_num = cur_sub_size - 1;
                _colors_tmp[s][n] = new int**[comb_node_num];

                for(int c = 0; c<comb_node_num;c++)
                {
                    int split_main_num = c + 1;
                    int split_aux_num = cur_sub_size - split_main_num;
                    int** colors_split_main = _index_tmp[(cur_sub_size-2)][(split_main_num-1)];
                    int** colors_split_aux = _index_tmp[(cur_sub_size-2)][(split_aux_num-1)];

                    int set_node_num = comb_calc(cur_sub_size, (c + 1));
                    _colors_tmp[s][n][c] = new int*[set_node_num];

                    for(int i=0;i<set_node_num;i++)
                    {
                        _colors_tmp[s][n][c][i] = new int[cur_sub_size];

                        for(int j=0;j< split_main_num;j++)
                            _colors_tmp[s][n][c][i][j] = perm_colors[colors_split_main[i][j]];

                        for(int j=0;j< split_aux_num;j++)
                            _colors_tmp[s][n][c][i][j + split_main_num] = perm_colors[colors_split_aux[i][j]];

                    }

                }

                perm_set_next(perm_colors, cur_sub_size, _color_num);

            }

            delete[] perm_colors;

        }

    }

}/*}}}*/

int IndexSys::get_color_hash(int* perm_set, int size)
{/*{{{*/
    int count = 0;
    for (int i = 0; i < size; i++)
    {
        int n = perm_set[i] - 1;
        int k = i + 1;
        count += comb_calc(n, k);
    }
    return count;
}/*}}}*/

void IndexSys::gen_comb_hash_table()
{/*{{{*/

    _i_sub_c_split_to_counts = new int***[2];

    _i_sub_c_split_to_counts[0] = new int**[_sub_len];
    _i_sub_c_split_to_counts[1] = new int**[_sub_len];

    _i_sub_precomp_to_counts = new int*[_sub_len];

    _i_sub_c_to_counts = new int*[_sub_len];

    for(int s=0;s<_sub_len; s++)
    {
        int sub_vert_num = (*_sub_tps)[s].get_vert_num();
        int sub_comb_num = comb_calc(_color_num, sub_vert_num);

        int main_count_len = 0;
        int aux_count_len = 0;
        int precomp_len = 0;

        if (sub_vert_num > 1)
        {
            _i_sub_c_split_to_counts[0][s] = new int*[sub_comb_num];
            _i_sub_c_split_to_counts[1][s] = new int*[sub_comb_num];
        }

        _i_sub_c_to_counts[s] = new int[sub_comb_num];
        int* perm_colors = new int[sub_vert_num];
        perm_set_init(perm_colors, sub_vert_num);

        int split_main_num = 0;
        int split_aux_num = 0;

        if (sub_vert_num > 1)
        {
            split_main_num = _divider->get_main_node_vert_num(s); 
            split_aux_num = _divider->get_aux_node_vert_num(s);

            _i_sub_precomp_to_counts[s] = (int*) malloc(comb_calc(sub_vert_num, split_main_num)*
                    sub_comb_num*sizeof(int));

            main_count_len = comb_calc(_color_num, (*_sub_tps)[_divider->get_main_node_idx(s)].get_vert_num());
            aux_count_len = comb_calc(_color_num, (*_sub_tps)[_divider->get_aux_node_idx(s)].get_vert_num());
            precomp_len = main_count_len*aux_count_len;

        }

        for(int n=0;n< sub_comb_num; n++)
        {
            _i_sub_c_to_counts[s][n] = get_color_hash(perm_colors, sub_vert_num);
            if (sub_vert_num > 1)
            {
                int* colors_main = NULL;
                int* colors_aux = NULL;

                //debug
                // printf("cur_colorset: %d, %d, %d, %d\n", s, n, (split_main_num-1), (sub_vert_num -1));
                // std::fflush(stdout);

                // assert( (split_main_num-1) >=0 && (split_main_num -1) < (sub_vert_num -1));
                // assert(_colors_tmp[s][n][split_main_num-1] != NULL);

                int** cur_colorset = _colors_tmp[s][n][split_main_num-1];
                int split_main_comb = comb_calc(sub_vert_num, split_main_num);

                _i_sub_c_split_to_counts[0][s][n] = new int[split_main_comb];
                _i_sub_c_split_to_counts[1][s][n] = new int[split_main_comb];

                int aux_itr = split_main_comb -1 ;
                for(int main_itr = 0; main_itr < split_main_comb; ++main_itr, --aux_itr)
                {
                    colors_main = cur_colorset[main_itr];
                    colors_aux = cur_colorset[aux_itr] + split_main_num;

                    int color_index_main = get_color_hash(colors_main, split_main_num);
                    int color_index_aux = get_color_hash(colors_aux, split_aux_num);

                    _i_sub_c_split_to_counts[0][s][n][main_itr] = color_index_main;
                    _i_sub_c_split_to_counts[1][s][n][aux_itr] = color_index_aux;

                    _i_sub_precomp_to_counts[s][n*split_main_comb + aux_itr] = 
                        (color_index_main*aux_count_len + color_index_aux);
                }


            }

            // permutate color set
            perm_set_next(perm_colors, sub_vert_num, _color_num);

        }

        delete[] perm_colors;

    }

}/*}}}*/























