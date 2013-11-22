// Copyright (c)2008-2011, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef JUBATUS_UTIL_DATA_SPARSE_MATRIX_SPARSE_MATRIX_H_
#define JUBATUS_UTIL_DATA_SPARSE_MATRIX_SPARSE_MATRIX_H_

#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <fstream>

#include "../code/code.h"
#include "../unordered_map.h"

namespace jubatus {
namespace util {
namespace data {
namespace sparse_matrix {

  class matrix_offsets;

  /**
   * @brief class for building matrix which compressed by rows
   */
  class sparse_matrix_writer
  {
  public:
    sparse_matrix_writer();
    ~sparse_matrix_writer();

    int open(const std::string& fn);
    void close();

    /**
     * @brief add row
     * @param row
     * 
     * row is vector of pair of ('row number', 'value')
     * 'row number' must be >=0, must not be dup.
     * 'value' must be >=1.
     */
    void append_row(const std::map<int,unsigned char>& row);
    void append_row(const std::vector<std::pair<int,unsigned char> >& row);

    int row_num();
  private:
    std::ofstream ofs;
    matrix_offsets* offsets;
  };

  /**
   * @brief class for reading rows
   */
  class sparse_matrix_reader
  {
  public:
    sparse_matrix_reader();
    ~sparse_matrix_reader();

    int open(const std::string& fn);
    void close();

    /**
     * @brief number of rows
     * number of columns is not preserved.
     */
    int row_num() const;

    /**
     * @brief number of non-zero elements in specified row.
     */
    int get_row_size(int row) const;

    /**
     * @brief get row
     */
    void get_row(int row, std::vector<std::pair<int,unsigned char> >& data);
    void get_row(int row, std::map<int,unsigned char>& data);
    void get_row(int row, jubatus::util::data::unordered_map<int,unsigned char>& data);

    /**
     * @brief number of non-zero elements in whole matrix
     */
    int get_row_nonzero_value_num(int row) const;
  private:
    jubatus::util::data::code::decoder dc;
    matrix_offsets* offsets;
  };

  /**
   * @brief transpose matrix
   */
  int matrix_transpose(const std::string& fn, const std::string& fnT);
} // sparse_matrix
} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_SPARSE_MATRIX_SPARSE_MATRIX_H_
