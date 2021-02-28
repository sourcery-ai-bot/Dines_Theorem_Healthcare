//
//  main.cpp
//  Clinton_Global_Initiative_University
//
//  Created by Veronica Backer-Peral on 11/27/20.
//

#include <iostream>
#include "Dines_Theorem_1927.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <algorithm>
//#include <gmp.h>
//#include <gmpxx.h>
#include "gmp.h"
//#include "gmpxx.h"

using namespace std;

using namespace Dines_Theorem;



void n_choose_2(mpz_t n, mpz_t* result) {    // combination algorithm (n choose k) --> adjusted since we know k is always 2 for this problem
    
    mpz_t one,two,n_minus_one;
    mpz_init_set_str(one, "1", 10);
    mpz_init_set_str(two, "2", 10);
    mpz_init(n_minus_one);
    
    mpz_set(*result,n);
    
    mpz_sub(n_minus_one,n,one);
    
    mpz_mul(*result,*result,n_minus_one);
    mpz_div(*result,*result,two);
    
    mpz_clear(one);
    mpz_clear(two);
    mpz_clear(n_minus_one);
};



Sparse *reduce_matrix(Sparse *matrix) {   // corresponds to step 2 in Dines: the reduction from m equaltions to m-1 equations
    cout << ">>> Reducing...\n";
    
    mpz_t combination;
    mpz_init(combination);
    n_choose_2(matrix->N, &combination);
    
    cout << "Positive count: " << matrix->positive_count << "\n";
    cout << "Negative count: " << matrix->negative_count << "\n";
    cout << "Upcoming size: " << matrix->positive_count*matrix->negative_count << "\n";
    
    if (matrix->positive_count*matrix->negative_count < 0) {
        cout << "Error: int overflow \n";
    }
 
    Sparse *output_matrix = new Sparse(matrix->M-1, combination, matrix->positive_count*matrix->negative_count, NULL);
    
//    cout << "Output Matrix Size: " << output_matrix->size << "\n";
    
    mpz_clear(combination);
   
    unsigned long long int pos0 = 0;
    unsigned long long int neg0 = 0;
    unsigned long long int index = 0;
    
    mpz_t g;
    mpz_init(g);
    
    unsigned long long int count=0;
    
    for (unsigned long long int i=0; i < matrix->size; i++) {
        
       // cout << "i = " << i << "\n";
        
        if (matrix->values[0][i] >= 0) {
            
            for (unsigned long long int j=neg0; j < matrix->negative_count; j++) {
                
//                cout << "j_neg = " << j << "\n";
                
//                if (output_matrix->size > 1000000) {
//                    cout << "g = ";
//                    gmp_printf("%Zd \n", g);
//                }
                
                matrix->find_index(matrix->indexes[matrix->positive_indexes[pos0]],matrix->indexes[matrix->negative_indexes[j]],&g);
                
                mpz_set(output_matrix->indexes[index],g);
                for (unsigned long long int h=0; h < matrix->M - 1; h++) {
                    
//                    if (count%10000000 == 0 && count != 0) {
//                        cout << ">> [" << count << "/" << (matrix->positive_count * matrix->negative_count *  matrix->M) << "] --> " << (100*(count/(matrix->positive_count * matrix->negative_count *  matrix->M))) << "% \n";
//                    }
                    if (count%10000000 == 0 && count != 0) {
                        cout << ">> [" << count << "/" << to_string(matrix->positive_count * matrix->negative_count *  matrix->M) << "]\n";
                    }
                    count++;
                    double result = matrix->values[0][matrix->positive_indexes[pos0]] * matrix->values[h+1][matrix->negative_indexes[j]] - matrix->values[0][matrix->negative_indexes[j]] * matrix->values[h+1][matrix->positive_indexes[pos0]];   // corresponds to the calculation in Section 2, step (5) of Dines
                    output_matrix->values[h][index] = result;
                }
                index++;
            }
            pos0++;
        } else {
            for (unsigned long long int j=pos0; j < matrix->positive_count; j++) {
                
                matrix->find_index(matrix->indexes[matrix->negative_indexes[neg0]],matrix->indexes[matrix->positive_indexes[j]],&g);
                mpz_set(output_matrix->indexes[index],g);
                for (unsigned long long int h=0; h < matrix->M - 1; h++) {
                    
                    if (count%10000000 == 0 && count != 0) {
                        cout << ">> [" << count << "/" << to_string(matrix->positive_count * matrix->negative_count *  matrix->M) << "]\n";
                    }
                    count++;
                    
//                    cout << "h = " << h << "\n";
                    double result = matrix->values[0][matrix->positive_indexes[j]] * matrix->values[h+1][matrix->negative_indexes[neg0]] - matrix->values[0][matrix->negative_indexes[neg0]] * matrix->values[h+1][matrix->positive_indexes[j]];   // corresponds to the calculation in Section 2, step (5) of Dines
                    output_matrix->values[h][index] = result;
                }
                index++;
            }
            neg0++;
        }
        
        
    }
    
    mpz_clear(g);
    
    output_matrix->reduce(false);
    return output_matrix;
};

Sparse *substitute(Sparse *matrix) {  // using the two solutions found for the RHS and LHS in single_solution, we now create a 'substitute' matrix which substitutes these values into their corresponding index.
    double pos_solution,neg_solution;
    matrix->single_solution(&pos_solution, &neg_solution);
    
    Sparse *output_matrix = new Sparse(matrix->M, matrix->N, matrix->size, NULL);
    
    for (unsigned long long int i=0; i<matrix->M; i++) {
        for (unsigned long long int j=0; j<matrix->size; j++) {
            if (matrix->values[i][j] >= 0) {
                output_matrix->values[i][j] = neg_solution;
                mpz_set(output_matrix->indexes[j],matrix->indexes[j]);
            }
            else if (matrix->values[i][j] < 0) {
                output_matrix->values[i][j] = pos_solution;
                mpz_set(output_matrix->indexes[j],matrix->indexes[j]);
            }
        }
    }
    return output_matrix;
};

Sparse *simplify_matrix(Sparse *matrix, Sparse *substitutes) {     // this corresponds to Step 3: The algorithm for a system of m equations (Dines 388).
    
    //cout << ">>> Simplifying...\n";
    
    double duration_sub_indexes = 0;
    double duration_get_indexes = 0;
    
    Sparse *output_matrix = new Sparse(1, matrix->N, matrix->size, matrix->indexes);
    
    mpz_t i,j;
    mpz_init(i);
    mpz_init(j);
    
    for (unsigned long long int position=0; position < substitutes->size; position++) {

       if (position%100000 == 0 && position != 0) {
            cout << ">> [" << to_string(position) << "/" << to_string(substitutes->size) << "]\n";
       }
        clock_t start_sub_indexes = clock();
        
        substitutes->find_sub_indexes(&i,&j,substitutes->indexes[position],output_matrix->N);
        
//        gmp_printf("index: %Zd \n", substitutes->indexes[position]);
//        gmp_printf("i: %Zd, j: %Zd\n\n", i, j);
        
        duration_sub_indexes += (clock() - start_sub_indexes)/(double) CLOCKS_PER_SEC;
        
        clock_t start_get_indexes = clock();
        unsigned long long int i_position = matrix->get_sparse_index(i);
        unsigned long long int j_position = matrix->get_sparse_index(j);
        duration_get_indexes += (clock() - start_get_indexes)/(double) CLOCKS_PER_SEC;
        
        output_matrix->values[0][i_position] += abs( matrix->values[0][j_position] *  substitutes->values[0][position] );
        output_matrix->values[0][j_position] += abs( matrix->values[0][i_position] *  substitutes->values[0][position] );

        }
    
    cout << "Total time finding sub indexes: " << duration_sub_indexes << "\n";
    cout << "Total time getting indexes: " << duration_get_indexes << "\n";
    
    mpz_clear(i);
    mpz_clear(j);
    
    output_matrix->reduce(false);
    return output_matrix;
};

void Dines_Theorem_1927 (double** initial_matrix, unsigned long long int N, int M, bool verbose, string file) {
    cout << "M = " << M << "\n";
    cout << "N = " << N << "\n";
    
    mpz_t N_gmp;
    mpz_init(N_gmp);
    mpz_set_si(N_gmp,N);
    
    Sparse initial_sparse = Sparse(M, N_gmp, N, NULL);    // convert array to Sparse matrix
    
    initial_sparse.populate(initial_matrix);
    
    cout << "Step [0/3] complete.\n";
    
    if (verbose) {
        cout << "\ninitial sparse\n";
        initial_sparse.display();
        cout << "beginning to reduce... \n";
    }

    Sparse** reduced_list = new Sparse*[M];
    
    reduced_list[0] = &initial_sparse;
    
    for (unsigned long long int i=1; i<M; i++) {                   // step 2: The reduction from m equations to m-1 equations (Dines)
        reduced_list[i-1]->get_pos_neg_indexes();
        reduced_list[i] = reduce_matrix(reduced_list[i-1]);
//        reduced_list[i-1].delete_all_rows_except_first();
        if (verbose) {
            cout << "\nreduced_list[" << i << "]\n";
            reduced_list[i]->display();
        }
        
        if (reduced_list[i]->all_same_sign_coefficients()) {
            cout << "No solution.\n";
            exit(0);
        }
    };
    
    cout << "Step [1/3] complete.\n";
    
    reduced_list[M-1]->get_pos_neg_indexes();
    
    Sparse** simplified_list = new Sparse*[M];
    simplified_list[0] = substitute(reduced_list[M-1]);     // step 1: the case of a single equation (Dines)

    cout << "Step [2/3] complete.\n";
    
    if (verbose) {
        cout << "\nsubstitute \n";
        simplified_list[0]->display();
        cout << "beginning to simplify... \n";
    }

    for (unsigned long long int i=1; i<M; i++) {                     // step 3: the algorithm for a system of m equations (Dines)
        simplified_list[i] = simplify_matrix(reduced_list[M-i-1], simplified_list[i-1]);

        if (verbose) {
            cout << "\nsimplified_list[" << i << "]\n";
            simplified_list[i]->display();
        }
    }
    
    cout << "Step [3/3] complete.\n";
    
    simplified_list[M-1]->reduce(true);
    Sparse solution = *simplified_list[M-1];

    if (verbose) {
        cout << "\nFinal solution!\n";
        solution.display();
    }

    bool check = true;

    for (unsigned long long int m=0; m<M; m++) {
        double sum = 0;
        for (unsigned long long int n=0; n<N; n++) {
            sum += solution.values[0][n] * initial_matrix[m][n];
        }
       //cout << sum << "\n";
        if (round(sum) != 0) {
            check = false;
            //break;
        }
    }
    if (check) {
        cout << "Success.\n\n";
    } else {
        cout << "Failure.\n";
    }
    
    ofstream output_file (file);
    if (check && output_file.is_open()) {
        for (unsigned long long int j=0; j < solution.size; j++) {
            output_file << solution.values[0][j] << " ";
        }
    }
     
    for (unsigned long long int i=0; i < M; i++) {
        reduced_list[i]->delete_all_rows_except_first();
        reduced_list[i]->delete_first_row();
        simplified_list[i]->delete_first_row_except_indexes();
    }
    delete [] simplified_list[0]->indexes;
    
    delete[] reduced_list;
    delete[] simplified_list;
    mpz_clear(N_gmp);
}

int factorial(int n) {
    int result = n;
    for (int i=1; i<n; i++) {
        result *= i;
    }
    return result;
}

void permute(double** initial_matrix, int current, int goal) {
    if (current == goal) {
        return;
    } else {
        current++;
        // do stuff
        
        permute (initial_matrix,current,goal);
    }
}

void swap(double*** initial_matrix, int col1, int col2, int N) {

    for (int i=0; i<N; i++) {
        double tmp = *initial_matrix[col1][i];
        *initial_matrix[col1][i] = *initial_matrix[col2][i];
        *initial_matrix[col2][i] = tmp;
    }
}

void dimensions(string data, int* M, int* N) { // find dimensions of a matrix in a string
    
    for (int i=0; i<data.length(); i++) {
        if (data[i] == ' ') {
            *N = *N + 1;
        }
        if (data[i] == '|') {
            *M = *M + 1;
        }
    }
    
    *M = *M + 1;
    *N = *N / *M;
}

int main(int argc, char **argv) {
    double duration = 0;
    clock_t start = clock();
    
    string input;
    int number;
    if (argv[1] && argv[2]) {
        input = argv[1];
        number = stoi(argv[2]);
    } else {
        cout << "Please enter values in the form a string, with rows delineated with the symbol '|'.\n";
        exit(0);
    }
    
    int N = 0;
    int M = 0;
    dimensions(input, &M, &N);
    
    // CREATE INITIAL MATRIX
    
//    input.erase(remove(input.begin(), input.end(), '|'), input.end());
//    input.erase(remove(input.begin(), input.end(), ' '), input.end());
    
    double** initial_matrix = new double*[M];

    for (int i=0; i<M; i++) {
        initial_matrix[i] = new double[N];
    }
    
    int i=0;
    int j=0;
    
    string value = "";
    for (int k=0; k < input.length(); k++) {
        if (input[k] == ' ') {
            initial_matrix[i][j] = stod(value);
            j++;
            value = "";
            continue;
        }
        if (input[k] == '|') {
            j=0;
            i++;
            value = "";
            continue;
        }
        value += input[k];
    }

//    for (int i=0; i<M; i++) {
//        for (int j=0; j<N; j++) {
//            cout << initial_matrix[i][j] << " ";
//        }
//        cout << "\n";
//    }

    Dines_Theorem_1927(initial_matrix,N,M,false,"solution.txt");

    for (int i=0; i<M; i++) {
        delete[] initial_matrix[i];
    }

    delete[] initial_matrix;

    duration = (clock() - start)/(double) CLOCKS_PER_SEC;
    cout << "Total duration: " << duration << "\n";
};
