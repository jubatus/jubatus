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

#include "sparse_matrix.h"

#include <cstring>
#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>

#include "../../system/file.h"
#include "../../system/sysstat.h"

using namespace std;
using namespace jubatus::util::data::code;
using namespace jubatus::util::system::file;

namespace jubatus {
namespace util {
namespace data {
namespace sparse_matrix {


  ////////////////////////////////////////////////////////////////
  //
  // MatrixOffsets
  //

  enum MATRIX_IO_MODE {
    MATRIX_IO_NONE,   // undefined
    MATRIX_IO_READ,   // "r"
    MATRIX_IO_WRITE,  // "w"
  };

  struct RowInfo {
    int num,size;
  };

  class matrix_offsets
  {
  public:
    matrix_offsets();
    int open(string fnOffsets, MATRIX_IO_MODE mode);
    void close();
    int flush() const;
    int offsets_num() const;
    
    int num(int index) const;
    int size(int index) const;
    int offset(int index) const;
    
    void append_row(int size, int num);
  private:
    string fnOffsets;
    vector<RowInfo> rowInfos;
    vector<int> offsets;
    MATRIX_IO_MODE ioMode;
  };

  matrix_offsets::matrix_offsets()
    :ioMode(MATRIX_IO_NONE)
  {
  }

  int matrix_offsets::open(string fn, MATRIX_IO_MODE mode)
  {
    close();

    ioMode=mode;
    fnOffsets=fn;

    if (mode==MATRIX_IO_READ) {
      ifstream ifs(fnOffsets.c_str());
      if (!ifs) return -1;
      
      int rowNum;
      ifs.read((char*)&rowNum,sizeof(int));
      rowInfos=vector<RowInfo>(rowNum);
      offsets=vector<int>(rowNum);
      if (rowNum > 0)
        ifs.read((char*)&rowInfos[0],rowNum*sizeof(RowInfo));
      if (offsets.size()>0) {
        offsets[0]=0;
        for (int i=1;i<(int)offsets.size();++i) offsets[i]=offsets[i-1]+rowInfos[i-1].size;
      }
    } 
    return 0;
  }

  void matrix_offsets::close() {
    fnOffsets="";
    rowInfos.clear();
    offsets.clear();
    ioMode=MATRIX_IO_NONE;
  }

  int matrix_offsets::flush() const
  {
    if (fnOffsets.size()==0) return -1;

    if (ioMode==MATRIX_IO_WRITE) {
      ofstream ofs(fnOffsets.c_str());
      if (!ofs) {
        fprintf(stderr,"matrix_offsets::flush: cannot open %s\n",fnOffsets.c_str());
        return -1;
      }
      int rowNum=rowInfos.size();
      ofs.write((char*)&rowNum,sizeof(int));
      if (rowNum>0) ofs.write((char*)&rowInfos[0],rowNum*sizeof(RowInfo));
    }
    return 0;
  }

  int matrix_offsets::offsets_num() const
  {
    return rowInfos.size();
  }

  int matrix_offsets::num(int index) const
  {
    if (index>=(int)rowInfos.size()){
      fprintf(stderr,"matrix_offsets::Num: Try to access out of bounds of row index\n");
      fprintf(stderr,"Row size: %d, but you try to access to %d\n",(int)rowInfos.size(),index);
      return -1;
    }
    return rowInfos[index].num;
  }

  int matrix_offsets::size(int index) const
  {
    if (index>=(int)rowInfos.size()){
      fprintf(stderr,"matrix_offsets::Size: Try to access out of bounds of row index\n");
      fprintf(stderr,"Row size: %d, but you try to access to %d\n",(int)rowInfos.size(),index);
      return -1;
    }
    return rowInfos[index].size;
  }

  int matrix_offsets::offset(int index) const
  {
    if (index>=(int)offsets.size()){
      fprintf(stderr,"matrix_offsets::Offset: Try to access out of bounds of row index\n");
      fprintf(stderr,"Row size: %d, but you try to access to %d\n",(int)rowInfos.size(),index);
      return -1;
    }
    return offsets[index];
  }

  void matrix_offsets::append_row(int size, int num)
  {
    RowInfo rowInfo;
    rowInfo.size=size;
    rowInfo.num=num;
    rowInfos.push_back(rowInfo);

    int offset=offsets.size()==0?0:offsets.back()+size;
    offsets.push_back(offset);
  }


  ////////////////////////////////////////////////////////////////
  //
  // sparse_matrix_writer
  //

  sparse_matrix_writer::sparse_matrix_writer()
    :offsets(NULL)
  {
  }

  sparse_matrix_writer::~sparse_matrix_writer()
  {
    if (offsets) delete offsets;
  }

  int sparse_matrix_writer::open(const string& fnMat)
  {
    close();

    ofs.open(fnMat.c_str());
    if (!ofs) {
      fprintf(stderr,"sparse_matrix_writer::open: cannot open %s\n",fnMat.c_str());
      return -1;
    }

    offsets=new matrix_offsets;
    if (offsets->open(fnMat+".offset",MATRIX_IO_WRITE)<0) {
      fprintf(stderr,"sparse_matrix_writer::open: cannot open %s.offset\n",fnMat.c_str());
      ofs.close();
      return -1;
    }
    return 0;
  }

  void sparse_matrix_writer::close()
  {
    ofs.close();
    if (offsets) {
      offsets->flush();
      delete offsets;
      offsets=NULL;
    }
  }

  void sparse_matrix_writer::append_row(const std::map<int,unsigned char>& row)
  {
    if (!ofs.is_open()||!offsets) return;

    encoder ec;

    int termNum=0;
    int prevTerm=-1;
    std::map<int,unsigned char>::const_iterator it;
    for (it=row.begin();it!=row.end();++it) {
      ec.prefix_code(it->first-prevTerm);
      ec.byte((unsigned char)it->second);
      prevTerm=it->first;
      ++termNum;
    }
    int columnSize=ec.flush(ofs);
    offsets->append_row(columnSize,termNum);
  }

  void sparse_matrix_writer::append_row(const vector<pair<int,unsigned char> >& row) {
    if (!ofs.is_open()||!offsets) return;

    encoder ec;
    int termNum=0;
    int prevTerm=-1;
    for (int i=0;i<(int)row.size();++i) {
      ec.prefix_code(row[i].first-prevTerm);
      ec.byte(row[i].second);
      prevTerm=row[i].first;
      ++termNum;
    }
    int columnSize=ec.flush(ofs);
    offsets->append_row(columnSize,termNum);
  }

  int sparse_matrix_writer::row_num() {
    return offsets->offsets_num();
  }


  ////////////////////////////////////////////////////////////////
  //
  // sparse_matrix_reader
  //

  sparse_matrix_reader::sparse_matrix_reader()
    :offsets(NULL)
  {

  }

  sparse_matrix_reader::~sparse_matrix_reader()
  {
    close();
    //    if (buf) delete[] buf;
    //    if (offsets) delete offsets;
  }


  int sparse_matrix_reader::open(const string& fnMat)
  {
    close();
    dc.attach(fnMat);

    offsets=new matrix_offsets;
    if (offsets->open(fnMat+".offset",MATRIX_IO_READ)<0){
      fprintf(stderr,"sparse_matrix_reader::open: cannot open %s.offset\n",fnMat.c_str());    
      return -1;
    }
    return 0;
  }

  void sparse_matrix_reader::close()
  {
    dc.detach();
    if (offsets) {
      delete offsets;
      offsets=NULL;
    }
  }

  int sparse_matrix_reader::row_num() const
  {
    if (!offsets) return 0;
    return offsets->offsets_num();
  }

  int sparse_matrix_reader::get_row_size(int row) const
  {
    if (!offsets) return 0;
    return offsets->num(row);
  }

  void sparse_matrix_reader::get_row(int row, vector<pair<int,unsigned char> >& data)
  {
    if (!dc.is_open()||!offsets) return;

    int num=offsets->num(row);
    if (num<0){
      fprintf(stderr,"sparse_matrix_reader::get_row: offsets.Num error\n");
      return;
    }
    dc.seek(offsets->offset(row));
  
    int col=-1;
    data=vector<pair<int,unsigned char> >(num);
    for (int i=0;i<num;++i) {
      col+=dc.prefix_code();
      data[i].first=col;
      data[i].second=dc.byte();
    }
  }

  void sparse_matrix_reader::get_row(int row, std::map<int,unsigned char>& data)
  {
    if (!dc.is_open()||!offsets) return;

    int num=offsets->num(row);
    if (num<0){
      fprintf(stderr,"sparse_matrix_reader::get_row: offsets.Num error\n");
      return;
    }
    dc.seek(offsets->offset(row));
  
    int col=-1;
    for (int i=0;i<num;++i) {
      col+=dc.prefix_code();
      data[col]=dc.byte();
    }
  }

  void sparse_matrix_reader::get_row(int row, jubatus::util::data::unordered_map<int,unsigned char>& data)
  {
    if (!dc.is_open()||!offsets) return;

    int num=offsets->num(row);
    if (num<0){
      fprintf(stderr,"sparse_matrix_reader::get_row: offsets.Num error\n");
      return;
    }
    dc.seek(offsets->offset(row));
  
    int col=-1;
    for (int i=0;i<num;++i) {
      col+=dc.prefix_code();
      data[col]=dc.byte();
    }
  }

  int sparse_matrix_reader::get_row_nonzero_value_num(int row) const
  {
    if (!offsets) return 0;
    return offsets->num(row);
  }


  ////////////////////////////////////////////////////////////////
  //
  // MatrixTranspose
  //

  int matrix_transpose(const string& fnMat, const string& fnMatT)
  {
    fputs("transpose start\n",stderr);
    vector<std::map<int,unsigned char> > hash;

    sparse_matrix_reader index;
    fprintf(stderr,"index.open(%s)\n",fnMat.c_str());
    if (index.open(fnMat)<0){
      fprintf(stderr,"MatrixTranspose cannot open %s\n",fnMat.c_str());
      return -1;
    }
    sparse_matrix_writer indexT;
    fputs("indexT.open()\n",stderr);
    if (indexT.open(fnMatT)<0){
      fprintf(stderr,"MatrixTranspose cannot open %s\n",fnMatT.c_str());
      return -1;
    }
    int docNum=index.row_num();
    fprintf(stderr,"index.row_num() = %d\n",docNum);

    // count how much number of element not equal to zero contains for each column
    vector<int> termCount;
    for (int i=0;i<docNum;++i) {
      vector<pair<int,unsigned char> > docs;
      index.get_row(i,docs);
      for (int j=0;j<(int)docs.size();++j) {
        int term=docs[j].first;
        if (term>=(int)termCount.size()) termCount.resize(term+1);
        ++termCount[term];
      }
    }
    int termNum=termCount.size();

    fputs("term count finished\n",stderr);

    // for memory usage, split columns to small pieces
    uint64_t sum=0;
    vector<int> boundary;
    jubatus::util::system::sysstat::sysstat_ret stat;
    int stat_ret = jubatus::util::system::sysstat::get_sysstat(stat);
    if (stat_ret<0){
      fprintf(stderr,"get_sysstat failed\n");
      return -1;
    }
    uint64_t limit=stat.free_memory/20;

    for (int i=0;i<(int)termCount.size();++i) {
      sum+=termCount[i];
      if (sum>=limit) {
        fprintf(stderr,"boundary: %d\n",i);
        boundary.push_back(i+1);
        sum=0;
      }
    }
    if (sum>0) {
      fprintf(stderr,"boundary: %d\n",termNum);
      boundary.push_back(termNum);
    }

    fputs("split rows finished\n",stderr);

    // transpose for each column sets
    int st=0;
    for (int i=0;i<(int)boundary.size();++i) {
      fprintf(stderr,"transpose: %d / %d\n",i,(int)boundary.size());

      int en=boundary[i];
      vector<std::map<int,unsigned char> > hash(en-st);

      for (int j=0;j<docNum;++j) {
        vector<pair<int,unsigned char> > docs;
        index.get_row(j,docs);
        for (int k=0;k<(int)docs.size();++k) 
          if (st<=(int)docs[k].first&&(int)docs[k].first<en) 
            hash[docs[k].first-st][j]=docs[k].second;
      }
      for (int i=st;i<en;++i) indexT.append_row(hash[i-st]);
      st=en;
    }

    index.close();
    indexT.close();
    return 0;
  }

} // sparse_matrix
} // data
} // util
} // jubatus
