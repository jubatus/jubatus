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

#include <gtest/gtest.h>

#include <iostream>

#include "../lang/function.h"

#define TEST_ASSOCIATIVITY3(CAPTION,N,T1,T2,T3,G1,G2,G3,OP12,OP23,EXPECT_EQ) \
  TEST(CAPTION,associativity){						\
    for(int i=0;i<N;++i){						\
      T1 a=G1();							\
      T2 b=G2();							\
      T3 c=G3();							\
      if(!EXPECT_EQ(a OP12 (b OP23 c),(a OP12 b) OP23 c)){		\
        std::cerr << "associativity broken for"				\
                  << a << #OP12 << b << #OP23 << c << endl;		\
      }									\
    }									\
  }

#define TEST_ASSOCIATIVITY(CAPTION,N,T,G,OP,EXPECT_EQ)		\
  TEST_ASSOCIATIVITY3(CAPTION,N,T,T,T,G,G,G,OP,OP,EXPECT_EQ)	\

#define TEST_IDENTITY(CAPTION,N,T,G,ID,OP,EXPECT_EQ)		\
  TEST(CAPTION,identity){						\
    for(int i=0;i<N;++i){						\
      T a=G();							\
      if(!EXPECT_EQ(a OP ID,a)){					\
        std::cerr << ID << " is not right identity in"			\
                  << a << #OP << ID << endl;				\
      }									\
      if(!EXPECT_EQ(ID OP a,a)){					\
        std::cerr << ID << " is not left identity in"			\
                  << ID << #OP << a << endl;				\
      }									\
    }									\
  }
  
#define TEST_INVERSE(CAPTION,N,TYPE,GEN,ID,OP,OPI,EXPECT_EQ)	\
  TEST(CAPTION,inverse){						\
    for(int i=0;i<N;++i){						\
      TYPE a=GEN();							\
      if(!EXPECT_EQ(a OPI a,ID)){					\
        std::cerr << #OPI << " is not inverse operator in"		\
                  << a << #OPI << a << endl;				\
      }									\
      if(!EXPECT_EQ(a OP (ID OPI a),ID)){				\
        std::cerr << #OPI << " is not inverse operator because "	\
                  << ID << #OPI << a << " is not " << a << endl;	\
      }									\
    }									\
  }


  
#define TEST_COMMUTATIVITY2(CAPTION,N,T1,T2,G1,G2,OP,EXPECT_EQ)		\
  TEST(CAPTION,commutativity){						\
    for(int i=0;i<N;++i){						\
      T1 a=G1();							\
      T2 b=G2();							\
      if(!EXPECT_EQ(a OP b,b OP a)){					\
        std::cerr << "commutativity broken for"				\
                  << a << #OP << b << endl;				\
      }									\
    }									\
  }

#define TEST_COMMUTATIVITY(CAPTION,N,T,G,OP,EXPECT_EQ)			\
  TEST_COMMUTATIVITY2(CAPTION,N,T,T,G,G,OP,EXPECT_EQ)



#define TEST_DISTRIBUTIVITY3(CAPTION,N,T1,T2,T3,G1,G2,ADD,MUL,EXPECT_EQ) \
  TEST(CAPTION,distributivity){						\
    for(int i=0;i<N;++i){						\
      T1 a=G1();							\
      T1 b=G1();							\
      T2 c=G2();							\
      T3 lhs=a MUL c ADD b MUL c,rhs=(a ADD b) MUL c;			\
      if(!EXPECT_EQ(lhs,rhs)){						\
        std::cerr << "distributivity broken for " << endl;		\
        std::cerr << "a=" << a << endl;					\
        std::cerr << "b=" << b << endl;					\
        std::cerr << "c=" << c << endl;					\
        std::cerr << "a" #MUL "c" #ADD "b" #MUL "c=" << lhs << " ; "	\
                  << "(a" #ADD "b)" #MUL "c=" << rhs << endl;		\
      }									\
      T3 lhs2=c MUL a ADD c MUL b,rhs2=c MUL (a ADD b);			\
      if(!EXPECT_EQ(lhs2,rhs2)){						\
        std::cerr << "distributivity broken for " << endl;		\
        std::cerr << "a=" << a << endl;					\
        std::cerr << "b=" << b << endl;					\
        std::cerr << "c=" << c << endl;					\
        std::cerr << "c" #MUL "a" #ADD "c" #MUL "b=" << lhs << " ; "	\
                  << "c" #MUL "(a" #ADD "b)=" << rhs << endl;		\
      }									\
    }									\
  }

#define TEST_DISTRIBUTIVITY(CAPTION,N,T,G,ADD,MUL,EXPECT_EQ)	\
  TEST_DISTRIBUTIVITY3(CAPTION,N,T,T,T,G,G,ADD,MUL,EXPECT_EQ)
  

/** is TYPE commutative ring
    CAPTION     unique caption
    N           random test number
    TYPE        test target type
    GEN         generator function
    ZERO        zero for addtion
    ONE         unit for multiplication
    EXPECT_EQ   equality functino
 */
#define TEST_COMMUTATIVE_RING(CAPTION,N,TYPE,GEN,ZERO,ONE,EXPECT_EQ) \
  TEST_ASSOCIATIVITY(CAPTION ## _add,N,TYPE,GEN,+,EXPECT_EQ)		\
  TEST_IDENTITY(CAPTION ## _add,N,TYPE,GEN,ZERO,+,EXPECT_EQ)		\
  TEST_INVERSE(CAPTION ## _add,N,TYPE,GEN,ZERO,+,-,EXPECT_EQ)		\
  TEST_COMMUTATIVITY(CAPTION ## _add,N,TYPE,GEN,+,EXPECT_EQ)		\
                                                                        \
  TEST_ASSOCIATIVITY(CAPTION ## _mul,N,TYPE,GEN,*,EXPECT_EQ)		\
  TEST_IDENTITY(CAPTION ## _mul,N,TYPE,GEN,ONE,*,EXPECT_EQ)		\
  TEST_COMMUTATIVITY(CAPTION ## _mul,N,TYPE,GEN,*,EXPECT_EQ)		\
                                                                        \
  TEST_DISTRIBUTIVITY(CAPTION ## _add_mul,N,TYPE,GEN,+,*,EXPECT_EQ)	\
  TEST_DISTRIBUTIVITY(CAPTION ## _sub_mul,N,TYPE,GEN,-,*,EXPECT_EQ)

/** is TYPE field?
    CAPTION     unique caption
    N           random test number
    TYPE        target type
    GEN         random generator function
    ZERO        zero for addition
    ONE         unit for multiplication
    EXPECT_EQ   equality function
*/
#define TEST_FIELD(CAPTION,N,TYPE,GEN,ZERO,ONE,EXPECT_EQ) \
  TEST_ASSOCIATIVITY(CAPTION ## _add,N,TYPE,GEN,+,EXPECT_EQ)		\
  TEST_IDENTITY(CAPTION ## _add,N,TYPE,GEN,ZERO,+,EXPECT_EQ)		\
  TEST_INVERSE(CAPTION ## _add,N,TYPE,GEN,ZERO,+,-,EXPECT_EQ)		\
  TEST_COMMUTATIVITY(CAPTION ## _add,N,TYPE,GEN,+,EXPECT_EQ)		\
                                                                        \
  TEST_ASSOCIATIVITY(CAPTION ## _mul,N,TYPE,GEN,*,EXPECT_EQ)		\
  TEST_IDENTITY(CAPTION ## _mul,N,TYPE,GEN,ONE,*,EXPECT_EQ)		\
  TEST_INVERSE(CAPTION ## _mul,N,TYPE,GEN,ONE,*,/,EXPECT_EQ)		\
  TEST_COMMUTATIVITY(CAPTION ## _mul,N,TYPE,GEN,*,EXPECT_EQ)		\
                                                                        \
  TEST_DISTRIBUTIVITY(CAPTION ## _add_mul,N,TYPE,GEN,+,*,EXPECT_EQ)	\
  TEST_DISTRIBUTIVITY(CAPTION ## _sub_mul,N,TYPE,GEN,-,*,EXPECT_EQ)	\
  /*
  TEST_DISTRIBUTIVITY(CAPTION ## _add_div,N,TYPE,GEN,+,/,EXPECT_EQ)	\
  TEST_DISTRIBUTIVITY(CAPTION ## _sub_div,N,TYPE,GEN,-,/,EXPECT_EQ)	\
  */

/** is (ST,VT) vector space?
    CAPTION     unique caption
    N           number of random tests
    ST          scalar type of test target
    VT          vector type of test target
    SG          random generator function for ST
    VG          random generator function for VT
    S1          unit for ST's multiplication
    V0          zero for VT's addition
    S_EQ        equality function for ST
    V_EQ        equality function for VT
*/
#define TEST_VECTOR_SPACE(CAPTION,N,ST,VT,SG,VG,S1,V0,S_EQ,V_EQ)	\
  TEST_ASSOCIATIVITY(CAPTION ## _vadd,N,VT,VG,+,V_EQ)			\
  TEST_COMMUTATIVITY(CAPTION ## _vadd,N,VT,VG,+,V_EQ)			\
  TEST_COMMUTATIVITY2(CAPTION ## _smul,N,ST,VT,SG,VG,*,V_EQ)		\
  TEST_IDENTITY(CAPTION ## _vadd,N,VT,VG,V0,+,V_EQ)			\
  TEST_INVERSE(CAPTION ## _vadd,N,VT,VG,V0,+,-,V_EQ)			\
  TEST_DISTRIBUTIVITY3(CAPTION ## _vadd_smul,N,VT,ST,VT,VG,SG,+,*,V_EQ)	\
  TEST_DISTRIBUTIVITY3(CAPTION ## _sadd_smul,N,ST,VT,VT,SG,VG,+,*,V_EQ)	\
  TEST_ASSOCIATIVITY3(CAPTION ## _compatibility,N,ST,ST,VT,SG,SG,VG,*,*,V_EQ) \
  TEST_IDENTITY(CAPTION ## _scalar_id,N,VT,VG,S1,*,V_EQ)
