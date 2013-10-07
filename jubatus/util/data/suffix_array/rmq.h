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

/**
   Range minimum query (rmq) in O(log N) time and O(N) precomputed information.
*/

/*
  Implementation is based on 
  
  Johannes Fischer, Volker Heun. 
  "Theoretical and Practical Improvements on the RMQ-Problem, with Applications to LCA and LCE",
  Proceedings of the 17th Annual Symposium on Combinatorial Pattern Matching (CPM'06), 
  Lecture Notes in Computer Science 4009, 36-48, Springer-Verlag, 2006.
  
  and
  
  Johannes Fischer, Volker Heun. 
  "A New Succinct Representation of RMQ-Information and Improvements in the Enhanced Suffix Array",
  Proceedings of the International Symposium on Combinatorics, Algorithms, Probabilistic and Experimental Methodologies (ESCAPE'07), 
  Lecture Notes in Computer Science 4614, 459-470, Springer-Verlag, 2007.

  but modified for simplicity.

  There are two differences in Fischer'07 and this implementation:
  * Blocks / superblocks are replaced by Catalan number representation.
  * Use precomputed Catalan number table for all blocks.
  * Use O(16/7 N) bits

  Drawback of this method is query time of order O(log(8)N).
  It takes about 10 recursion to calculate RMQ if N = 1e9.

  FIXME: compare speed with another implementation. 
  especially when RMQ is changed to Bender & Farach's (O(NlogN),O(1))
*/
/*
  Large block_size may cause overflow. FYI:
  
  C = 4^s / (sqrt(pi) * s^(3/2))
  
  s  | C
  ---+-----
  8  | 1635
  10 | 19K
  16 | 38M
  20 | 6.9G
 */

#include <vector>
#include <iterator>
#include <climits>

// new in C99, but
// nowadays almost all compilers should have it
#include <stdint.h>

namespace jubatus {
namespace util {
namespace data {
namespace suffix_array {

  namespace {
    enum {
      block_bits = 3,
      block_size = 1 << block_bits,
      block_size_sq = block_size * (block_size+1) / 2,
      block_access_depth = 3
    };

    // enough to fit in int16
    typedef int16_t block_type;

    /** 
        Ballot number table.
        Generated from balgen.hs, with s = 8        
    */
    const int ballot_table[block_size+1][block_size+1] = {
#include "ballot_num8.dat"
    };

    /** 
        Minimum position table for cartesian types
    */
    const unsigned char cartesian_table[][block_size_sq] = {
#include "cartesian_table8.dat"
    };

    /**
       RMQ(i,j) -> cartesian_table[foo][access_table[i][j]] conversion
    */
    const int access_table[block_size][block_size] = {
      {  0,  1,  2,  3,  4,  5,  6,  7, },
      { -1,  8,  9, 10, 11, 12, 13, 14, },
      { -1, -1, 15, 16, 17, 18, 19, 20, },
      { -1, -1, -1, 21, 22, 23, 24, 25, },
      { -1, -1, -1, -1, 26, 27, 28, 29, },
      { -1, -1, -1, -1, -1, 30, 31, 32, },
      { -1, -1, -1, -1, -1, -1, 33, 34, },
      { -1, -1, -1, -1, -1, -1, -1, 35, },
    };
    
    const int msb_table[256] = {
      0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
    };

    const int all_block = access_table[0][block_size - 1];
    
    int get_msb(unsigned int b){
      if(b >= (1 << 24)) return 24 + msb_table[b >> 24];
      if(b >= (1 << 16)) return 16 + msb_table[b >> 16];
      if(b >= (1 << 8))  return 8  + msb_table[b >> 8];
      return msb_table[b];
    }

    /**
       Calculate cartesian tree's "type".
       See J. Fischer and V. Heun, "New succinct ... "
       
       @param b Beginning of iterator that points to the sequence of number
       @return cartesian tree type
    */
    const int infty = INT_MAX;
    const int neg_infty = INT_MIN;
    template<typename IT>
    int cartesian_type(IT b){
      const size_t s = block_size;
      // FIXME: remove this "R" reconstruction for speeding up
      std::vector<int> R(s+1);
      R[0] = neg_infty;
      int q = s;
      int N = 0;
      for(size_t i = 0; i < s; ++i){
        while(R[q + i - s] > *(b+i)){
          N += ballot_table[s-(i+1)][q];
          --q;
        }
        R[q+i+1-s] = *(b+i);
      }
      return N;
    }
    
    /**
       This routine is used if s != distance(b,e).
    */
    template<typename IT>
    int cartesian_type(IT b, IT e){
      const size_t real_s = std::distance(b, e);
      const size_t s = block_size;
      std::vector<int> R(s+1);
      R[0] = neg_infty;
      int q = s;
      int N = 0;
      for(size_t i = 0; i < real_s; ++i){
        while(R[q + i - s] > *(b+i)){
          N += ballot_table[s-(i+1)][q];
          --q;
        }
        R[q+i+1-s] = *(b+i);
      }
      return N;
    }
  }
  
  /** Range-minimum-query data structure */
  template<typename CONT>
  class rmq{
    typedef typename CONT::value_type T;

  public:
    /**
       RMQ constructor.
       takes container v and precompute data structure
       
       @param v target container
    */
    rmq(const CONT &v): orig(v), types(block_access_depth), log_rmq_table(){
      construct_type(v.begin(), v.end(), 0);
    }
    /* TODO: load/save from file */

    /**
       RMQ query in the range [l,r]. (right inclusive)
       
       @param l leftmost range
       @param r rightmost range (inclusive)
       @return position of minimum element
    */
    int query(int l, int r){
      int buf[(block_access_depth << 1) + 1];
      for(size_t i = 0; i < (block_access_depth << 1) + 1; ++i)
        buf[i] = -1;

      return query_iter(l, r, 0, buf);
    }
    
    /**
       RMQ query in the range [l,r). (right exclusive)
       
       @param l leftmost range
       @param r rightmost range (exclusive)
       @return position of minimum element
    */
    int query_exclusive(int l, int r){
      return query(l, r-1);
    }
      
  private:
    const CONT &orig;
    std::vector<std::vector<block_type> > types;
    std::vector<std::vector<int> > log_rmq_table;

    template<typename IT>
    void construct_type(IT b, IT e, int level){
      if(level >= block_access_depth) { construct_log_table(b, e); return; }

      size_t s = std::distance(b, e);
      size_t blks = (s + block_size - 1) >> block_bits;
      size_t blks_p = s >> block_bits;
      std::vector<block_type> &tys = types[level];
      std::vector<T> rm(blks);
      tys.resize(blks);        

      // cartesian type calculation
      {
        IT it(b);
        for(size_t i = 0; i < blks_p; ++i){
          int ty = cartesian_type(it);
          tys[i] = ty;
          rm[i] = *(it + cartesian_table[ty][all_block]);
          it += block_size;
        }
        if(blks != blks_p){
          int ty = cartesian_type(it, e);
          tys[blks_p] = ty;
          rm[blks_p] = *(it + cartesian_table[ty][all_block]);
        }
      }
      // FIXME: is this tail-recursive?
      construct_type(rm.begin(), rm.end(), level + 1);
    }

    template<typename IT>
    void construct_log_table(IT b, IT e){
      // four-russian-trick
      size_t s = std::distance(b, e);

      log_rmq_table.clear();
      {
        // first pass: fill k = 1 case 
        log_rmq_table.push_back(std::vector<int>(s-1));
        std::vector<int> &log_cur_table = log_rmq_table.back();
          
        int lmin_ = -1;
        IT it(b);
        for(size_t i = 0; i < s-1; ++i, ++it){
          if(*it < *(it+1)){
            log_cur_table[i] = (lmin_ >= 0 ? lmin_: query_super_cartesian_block(i));
            lmin_ = -1;
          }else{
            int rmin = query_super_cartesian_block(i+1);
            log_cur_table[i] = rmin;
            lmin_ = rmin;
          }
        }
      }

      for(size_t k = 2; (1U<<k) < s; ++k){
        int prev_size = 1 << (k-1);
        int cur_size = 1 << k;
        log_rmq_table.push_back(std::vector<int>(s-cur_size+1));
        std::vector<int> &prev = log_rmq_table[k-2];
        std::vector<int> &cur = log_rmq_table.back();

        for(size_t i = 0; i < s - cur_size + 1; ++i){
          // try not to call query_super_cartesian_block again
          int lm_ = prev[i];
          int lm = lm_ >> (block_bits * block_access_depth); 
          int rm_ = prev[i + prev_size];
          int rm = rm_ >> (block_bits * block_access_depth);

          cur[i] = (*(b+lm) < *(b+rm) ? lm_ : rm_);
        }
      }
    }
      
    int query_in_cartesian_block(int level, int blockpos){
      int ty = types[level][blockpos];
      return cartesian_table[ty][all_block];
    }

    int query_cartesian_block(int level, int super_blockpos){
      int pos = super_blockpos;
      for(int l = level; l >= 0; --l){
        pos = (pos << block_bits) + query_in_cartesian_block(l, pos);
      }
      return pos;
    }

    int query_super_cartesian_block(int super_blockpos){
      return query_cartesian_block(block_access_depth - 1, super_blockpos);
    }

    int query_iter(int l, int r, int level, int* buf){
      if(level >= block_access_depth) return query_iter_super_block(l, r, buf);
        
      int lb = l >> block_bits;
      int lh = l & (block_size - 1);
      int rb = r >> block_bits;
      int rh = r & (block_size - 1);

      std::vector<block_type> &tys = types[level];

      if(lb >= rb){
        // in-block query
        if(l <= r){
          buf[block_access_depth] = 
            query_cartesian_block(level - 1, // level = 0 -> ok
                                  (lb << block_bits)
                                  + cartesian_table[tys[lb]][access_table[lh][rh]]);
        }
        
        return best_in_buf(buf);
      }else{
        // out-of-block query
        if(lh != 0){
          buf[level] =
            query_cartesian_block(level-1,
                                  (lb << block_bits)
                                  + cartesian_table[tys[lb]][access_table[lh][block_size-1]]);
          lb++;
        }
        if(rh != block_size - 1){
          buf[(block_access_depth << 1) - level] = 
            query_cartesian_block(level-1,
                                  (rb << block_bits)
                                  + cartesian_table[tys[rb]][access_table[0][rh]]);
          rb--;
        }
          
        return query_iter(lb, rb, level + 1, buf);
      }
    }
      
    int query_iter_super_block(int l, int r, int *buf){
      r++;
      int s = r - l;
      if(s <= 0) return best_in_buf(buf);
      if(s == 1) { 
        buf[block_access_depth] = query_cartesian_block(block_access_depth - 1, l); 
        return best_in_buf(buf); 
      }
      int b = get_msb(s);
      int bl = 1 << (b-1);
      int leftix  = log_rmq_table[b-2][l];
      int rightix = log_rmq_table[b-2][r-bl];
      buf[block_access_depth] = (orig[leftix] < orig[rightix] ? leftix : rightix);
      return best_in_buf(buf);
    }

    int best_in_buf(int *buf){
      int cur_min_ix = -1;
      T cur_min = T();
      
      for(int i = 0; i < (block_access_depth << 1) + 1; ++i){
        int m = buf[i];
        if(m == -1) continue;
        T mval = orig[m];
        if(cur_min_ix == -1 || mval < cur_min){
          cur_min_ix = m;
          cur_min = mval;
        }
      }
      return cur_min_ix;
    }
  };
} // suffix_array
} // data
} // util
} // jubatus
