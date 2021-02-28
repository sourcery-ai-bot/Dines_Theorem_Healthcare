//
//  Dines_Theorem_1927.cpp
//  Clinton_Global_Initiative_University
//
//  Created by Veronica Backer-Peral on 11/27/20.
//

#include "Dines_Theorem_1927.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <string>
//#include <gmp.h>
//#include <gmpxx.h>
#include "gmp.h"
//#include "gmpxx.h"

using namespace std;
using namespace Dines_Theorem;


unsigned long long int gcd(unsigned long long int a, unsigned long long int b) { // greatest common denominator function
   if (b == 0)
   return a;
   return gcd(b, a % b);
}


//constructor
Sparse::Sparse(int _M, mpz_t _N, unsigned long long int _size,  mpz_t* _indexes) : M(_M),size(_size) {
    
//    mpz_t N;
    mpz_init(N);
    mpz_set(N,_N);
    
    values = new double*[M];
    if (_indexes == NULL) {
        indexes = new mpz_t[size];
        
        for (unsigned long long int i=0; i<size; i++) {
            mpz_init(indexes[i]);
        }
        
    } else {
        indexes = _indexes;
    }

    for(int i=0; i<M; i++) {
        values[i] = new double[size]();
    }
    positive_indexes=NULL;
    negative_indexes=NULL;
};


void Sparse::populate(double** matrix) { // populate an empty sparse matrix using a two dimensional array
    
    mpz_t _j;
    mpz_init(_j);
    
    for (int i=0; i<M; i++) {
        for (unsigned long long int j=0; j<size; j++) {
            values[i][j] = matrix[i][j];
            mpz_set_si(_j,(long) j);
            mpz_set(indexes[j],_j);
        }
    }
    
    mpz_clear(_j);
};
void Sparse::reduce(bool last) { // divide all values in each row of the matrix by the last value in the row
    double divide_value = 0;
    if (last) {
        divide_value = values[0][size-1];
    }
    for (int i=0; i < M; i++) {
        if (!last) {
            divide_value = values[i][0];
            for (unsigned long long int j=1; j<size; j++) {
                if (values[i][j] < divide_value && values[i][j] > 1) {
                    divide_value = values[i][j];
                }
            }
        }
        for (unsigned long long int j=0; j < size; j++) {
            values[i][j] = values[i][j]/divide_value;
        }
    }
};

void Sparse::display() {    // print matrix values and indexes
    
    for (int i=0; i < M; i++) {
        for (unsigned long long int j=0; j < size; j++) {
//            cout << "i = " << i << ", j = " << j << "\n";
            //char* index = mpz_get_str(index,10,indexes[j]);
            
            //cout << "index: " << index << "\n";
            
            cout << "[" << values[i][j] << "] at (" << i << ", ";
            gmp_printf("%Zd", indexes[j]);
            cout << ")\n";
        }
    }
};

void Sparse::find_index(mpz_t i, mpz_t j, mpz_t* result) {  // like sub_to_index but taking into accound that we only use the upper half of the matrix
    
    mpz_t one,two,product1,product2,sum1,sum2,sum3,quotient1;
    
    mpz_init(product1);
    mpz_init(product2);
    mpz_init(sum1);
    mpz_init(sum2);
    mpz_init(sum3);
    mpz_init(quotient1);
    
    mpz_init_set_str(one, "1", 10);
    mpz_init_set_str(two, "2", 10);
    
    mpz_mul(product1,i,N);  // i*N
    mpz_add(sum1,j,product1);   // j + i*N
    mpz_add(sum2,i,one);    // i + 1
    mpz_add(sum3,i,two);    // i + 2
    mpz_mul(product2,sum2,sum3);    // (i+1)*(i+2)
    mpz_div(quotient1,product2,two);    // (i+1)*(i+2)/2
    mpz_sub(*result,sum1,quotient1);;   // j + i*N - (i+1)*(i+2)/2
    
    mpz_clear(one);
    mpz_clear(two);
    mpz_clear(product1);
    mpz_clear(product2);
    mpz_clear(sum1);
    mpz_clear(sum2);
    mpz_clear(sum3);
    mpz_clear(quotient1);
};

void Sparse::find_sub_indexes(mpz_t* i, mpz_t* j, mpz_t k, mpz_t n) {
    ;
    
//    cout << "i: ";
//    gmp_printf("%Zd\n", *i);
//    
//    cout << "j: ";
//    gmp_printf("%Zd\n", *j);
//    
//    cout << "k: ";
//    gmp_printf("%Zd\n", k);
//    
//    cout << "n: ";
//    gmp_printf("%Zd\n", n);
    
    // FIND I
    
    mpf_t half_,one_,two_,four_,seven_,eight_,root_,product1_,product2_,product3_,diff1_,diff2_,diff3_,diff4_,quotient_,k_,n_;
    
    mpf_init_set_str(half_, ".5", 10);
    mpf_init_set_str(one_, "1", 10);
    mpf_init_set_str(two_, "2", 10);
    mpf_init_set_str(four_, "4", 10);
    mpf_init_set_str(seven_, "7", 10);
    mpf_init_set_str(eight_, "8", 10);
    
    mpf_init(root_);
    mpf_init(product1_);
    mpf_init(product2_);
    mpf_init(product3_);
    mpf_init(diff1_);
    mpf_init(diff2_);
    mpf_init(diff3_);
    mpf_init(diff4_);
    mpf_init(quotient_);
    mpf_init(k_);
    mpf_init(n_);
    
    mpf_set_z(k_,k);
    mpf_set_z(n_,n);
    
    mpf_mul(product1_,eight_,k_);
    mpf_mul(product2_,four_,n_);
    mpf_sub(diff1_,n_,one_);
    mpf_mul(product3_,product2_,diff1_);
    mpf_sub(diff2_,product3_,seven_);
    mpf_sub(diff3_,diff2_,product1_);
    mpf_sqrt(root_,diff3_);
    mpf_div(quotient_,root_,two_);
    mpf_sub(diff4_,quotient_,half_);
    
    mpz_t total;
    mpz_init(total);
    mpz_set_f(total,diff4_);
    
    mpz_t one, two, diff;
    mpz_init_set_str(one, "1", 10);
    mpz_init_set_str(two,"2",10);
    mpz_init(diff);
    
    mpz_sub(diff,n,two);
    mpz_sub(*i,diff,total);
    
//    cout << "i: ";
//    gmp_printf("%Zd, ", *i);
    
    //Clear MPFs
    
    mpf_clear(half_);
    mpf_clear(one_);
    mpf_clear(two_);
    mpf_clear(four_);
    mpf_clear(seven_);
    mpf_clear(eight_);
    mpf_clear(root_);
    mpf_clear(product1_);
    mpf_clear(product2_);
    mpf_clear(product3_);
    mpf_clear(diff1_);
    mpf_clear(diff2_);
    mpf_clear(diff3_);
    mpf_clear(diff4_);
    mpf_clear(quotient_);
    mpf_clear(k_);
    mpf_clear(n_);
    
    // FIND J
    
    mpz_t product1,product2,sum2,sum3,diff1,quotient1;
    
    mpz_init(product1);
    mpz_init(product2);
    mpz_init(sum2);
    mpz_init(sum3);
    mpz_init(diff1);
    mpz_init(quotient1);
    
    mpz_mul(product1,*i,n);
    mpz_sub(diff1,k,product1);
    mpz_add(sum2,*i,one);
    mpz_add(sum3,*i,two);
    mpz_mul(product2,sum2,sum3);
    mpz_div(quotient1,product2,two);
    mpz_add(*j,diff1,quotient1);;

//    cout << "j: ";
//    gmp_printf("%Zd\n", *j);
    
    // Clear MPZs
    
    mpz_clear(total);
    mpz_clear(one);
    mpz_clear(two);
    mpz_clear(diff);
    mpz_clear(product1);
    mpz_clear(product2);
    mpz_clear(sum2);
    mpz_clear(sum3);
    mpz_clear(diff1);
    mpz_clear(quotient1);
    
}


void Sparse::delete_all() {   // after reducing the following matrix, we never use any of the rows except for the first row. Deleting all the other rows to save memory.
    
    for (int i=0; i < M; i++) {
        delete [] values[i];
    }
    delete [] values;
    delete [] indexes;
    
    if (positive_indexes!=NULL)
        delete [] positive_indexes;
    if (negative_indexes!=NULL)
        delete [] negative_indexes;
    
}

void Sparse::delete_all_rows_except_first() {   // after reducing the following matrix, we never use any of the rows except for the first row. Deleting all the other rows to save memory.
    for (int i=1; i < M; i++) {
        delete [] values[i];
    }
}

void Sparse::delete_first_row() {   // after reducing the following matrix, we never use any of the rows except for the first row. Deleting all the other rows to save memory.
    delete [] values[0];
    
    for (unsigned long long int i=0; i<size; i++) {
        mpz_clear(indexes[i]);
    }
    
    delete [] indexes;
    delete [] positive_indexes;
    delete [] negative_indexes;
    delete [] values;
    
    mpz_clear(N);
}

void Sparse::delete_first_row_except_indexes() {   // after reducing the following matrix, we never use any of the rows except for the first row. Deleting all the other rows to save memory.
    delete [] values[0];
//    delete [] indexes;
    delete [] positive_indexes;
    delete [] negative_indexes;
    delete [] values;
    mpz_clear(N);
}

unsigned long long int Sparse::get_sparse_index ( mpz_t index) {    // in the final step, we need a way to find the corresponding index for a given value.
    
    mpf_t one,diff1,diff2,diff3,quotient1,product1,sum1;
    mpf_init(one);
    mpf_set_str(one,"1",10);
    
    mpf_t max,min,product,k,index_min,indexes_k,index_,N_;
    
    mpf_init(diff1);
    mpf_init(diff2);
    mpf_init(diff3);
    mpf_init(quotient1);
    mpf_init(product1);
    mpf_init(sum1);
    mpf_init(max);
    mpf_init(min);
    mpf_init(product);
    mpf_init(k);
    mpf_init(index_min);
    mpf_init(indexes_k);
    mpf_init(index_);
    mpf_init(N_);

    mpf_set_d(max,double(size));
    mpf_set_z(index_,index);
    mpf_set_z(N_,N);
    
    mpf_mul(product,max,index_);
    mpf_div(k,product,N_);
    
    unsigned long long int k_int = (int) mpf_get_si(k);
    
//    gmp_printf("Max: %Ff\n", max);
//    gmp_printf("Min: %Ff\n", min);
//    gmp_printf("Index Min: %Ff\n", index_min);
//    gmp_printf("Indexes_k: %Ff\n", indexes_k);
//    gmp_printf("index: %Zd\n", index);
//    gmp_printf("indexes[k]: %Zd\n", indexes[k_int]);
//    gmp_printf("k: %Ff\n\n", k);
    
//    cout << mpz_cmp(index,indexes[k_int]) << "\n";
    
    while(mpz_cmp(index,indexes[k_int]) != 0) {
        
        mpf_set_z(indexes_k,indexes[k_int]);
        
//        gmp_printf("Max: %Ff\n", max);
//        gmp_printf("Min: %Ff\n", min);
//        gmp_printf("Index Min: %Ff\n", index_min);
//        gmp_printf("k: %Ff\n", k);
//        gmp_printf("Indexes[k]: %Ff\n", indexes_k);
//        gmp_printf("index: %Zd\n\n", index);
        
        if (mpz_cmp(index,indexes[k_int]) > 0) {
            
            mpf_set(min,k);
            mpf_set(index_min,indexes_k);
            
            mpf_sub(diff1,max,k);
            mpf_sub(diff2,N_,indexes_k);
            mpf_sub(diff3,index_,indexes_k);
            mpf_div(quotient1,diff1,diff2);
            mpf_mul(product1,quotient1,diff3);
            mpf_add(sum1,product1,k);
            
            mpf_ceil(k,sum1);
            k_int = (int) mpf_get_si(k);
            
        } else {
            mpf_set(max,k);
            
            mpf_sub(diff1,min,k);
            mpf_sub(diff2,index_min,indexes_k);
            mpf_sub(diff3,index_,index_min);
            mpf_div(quotient1,diff1,diff2);
            mpf_mul(product1,quotient1,diff3);
            mpf_add(sum1,product1,min);
            
            mpf_floor(k,sum1);
            k_int = (int) mpf_get_si(k);
        }
    }
    return k_int;
    
    mpf_clear(one);
    mpf_clear(diff1);
    mpf_clear(diff2);
    mpf_clear(diff3);
    mpf_clear(quotient1);
    mpf_clear(product1);
    mpf_clear(sum1);
    mpf_clear(max);
    mpf_clear(min);
    mpf_clear(product);
    mpf_clear(k);
    mpf_clear(index_min);
    mpf_clear(indexes_k);
    mpf_clear(index_);
    mpf_clear(N_);
}

bool Sparse::all_same_sign_coefficients() {    // if at any point all of the coefficients are of the same sign, there is no solution (Dines 389)
    for (unsigned long long int i=0; i < M; i++) {
        unsigned long long int a = 0;
        unsigned long long int k = 0;
        while (a == 0 && k < size) {
            a = values[i][k];
            k++;
        }
        for (unsigned long long int j=0; j < size; j++) {
            if (a * values[i][j] < 0) {
                return false;
            }
        }
    }
    return true;
};

void Sparse::get_pos_neg_indexes() {    // get the positive and negative indexes
    positive_indexes = new unsigned long long int[size];
    negative_indexes = new unsigned long long int[size];
    positive_count =0;
    negative_count =0;
    for (unsigned long long int j=0; j<size; j++) {
        if (values[0][j] >= 0) {  // positive indexes include when the value zero (which means that in another row there is a non zero element)
            positive_indexes[positive_count] = j;
            positive_count ++;
        }
        else if (values[0][j] < 0){
            negative_indexes[negative_count] = j;
            negative_count ++;
        }
    }
};


void Sparse::single_solution(double* pos_solution, double* neg_solution) { // find a single solution for all of the positive (LHS) coefficients of the most reduced matrix, and a single solution for all the negative (RHS) coefficients of the most reduced matrix. Corresponds to Section 1: The case of a single equation (Dines).
    
    for (unsigned long long int i=0; i<positive_count; i++) {
       *pos_solution += values[0][positive_indexes[i]];
    }
    for (unsigned long long int j=0; j<negative_count; j++) {
        *neg_solution -= values[0][negative_indexes[j]];
    }
    unsigned long long int g = 1;
    try {
        unsigned long long int p = (int) *pos_solution;
        unsigned long long int n = (int) *neg_solution;
        g = gcd(p,n);
    } catch (unsigned long long int e) {}
    *pos_solution = *pos_solution/g;
    *neg_solution = *neg_solution/g;
    
};
