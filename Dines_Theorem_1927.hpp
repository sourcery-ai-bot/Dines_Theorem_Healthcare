//
//  Dines_Theorem_1927.hpp
//  Clinton_Global_Initiative_University
//
//  Created by Veronica Backer-Peral on 11/27/20.
//

#ifndef Dines_Theorem_1927_hpp
#define Dines_Theorem_1927_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
//#include <gmp.h>
//#include <gmpxx.h>
#include "gmp.h"
//#include "gmpxx.h"

namespace Dines_Theorem {
    class Sparse {
        public:
            double ** values;
            mpz_t * indexes;
            unsigned long long int * positive_indexes; // positive_indexes is an array of indexes for values that are greater than or equal to zero
            unsigned long long int * negative_indexes; // negative_indexes is an array of indexes for values that are less than zero
            int M; // number of rows
            mpz_t N; // number of columns if the matrix were not sparse
            unsigned long long int size; // number of columns in the sparse matrix
            unsigned long long int positive_count; // size of positive_indexes
            unsigned long long int negative_count; // size of negative_indexes

            //constructor
            Sparse(int _M, mpz_t _N, unsigned long long int _size,  mpz_t* _indexes);

            void populate(double** matrix);
            void find_index(mpz_t i, mpz_t j, mpz_t* result);
            void find_sub_indexes(mpz_t* i, mpz_t* j, mpz_t position, mpz_t N_);
            void reduce(bool last);
            void display();
            void get_pos_neg_indexes();
            unsigned long long int get_sparse_index ( mpz_t index);
            void delete_all_rows_except_first();
            void delete_first_row();
            void delete_first_row_except_indexes();
            bool all_same_sign_coefficients();
            void delete_all();
            void single_solution(double* pos_solution, double* neg_solution);
    };
}

#endif /* Dines_Theorem_1927_hpp */
