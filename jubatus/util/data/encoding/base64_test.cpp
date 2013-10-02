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

#include "./base64.h"

#include "../../math/random.h"
#include "../../math/random/mersenne_twister.h"

using namespace pfi::data::encoding;

TEST(base64, encode)
{
  std::vector<std::string> tests;
  tests.push_back("You are Red Bull!");
  tests.push_back("え、おれ今エンコードしてた？完全に無意識だったわー");
  tests.push_back("我去图书馆看书");

  std::vector<std::string> answers;
  answers.push_back("WW91IGFyZSBSZWQgQnVsbCE=");
  answers.push_back("44GI44CB44GK44KM5LuK44Ko44Oz44Kz44O844OJ44GX44Gm44Gf77yf5a6M5YWo44Gr54Sh5oSP6K2Y44Gg44Gj44Gf44KP44O8");
  answers.push_back("5oiR5Y675Zu+5Lmm6aaG55yL5Lmm");

  for (size_t i = 0; i < tests.size(); i++) {
    std::string encoded = base64_encode(tests[i]);
    EXPECT_EQ(answers[i], encoded);
  }
}

TEST(base64, decode)
{
  std::vector<std::string> tests;
  tests.push_back("UHJlZmVycmVkIEluZnJhc3RydWN0dXJl");
  tests.push_back("Qysr44Gu6YWN5YiX44Gv44Go44Gm44KC5Y2x6Zm677yB");
  tests.push_back("0YDRg9GB0YHQutC40Lkg0Y/Qt9GL0Lo=");

  std::vector<std::string> answers;
  answers.push_back("Preferred Infrastructure");
  answers.push_back("C++の配列はとても危険！");
  answers.push_back("русский язык");

  for (size_t i = 0; i < tests.size(); i++) {
    std::string decoded = base64_decode(tests[i]);
    EXPECT_EQ(answers[i], decoded);
  }
}

TEST(base64, encode_and_decode)
{
  pfi::math::random::mtrand r;

  // Generate random 10 test cases
  std::vector<std::string> tests;
  for (size_t i = 0; i < 10; i++) {
    size_t length = r.next_int(20, 50);
    std::string text;
    for (size_t j = 0; j < length; j++)
      text += r.next_int();
    tests.push_back(text);
  }

  for (size_t i = 0; i < tests.size(); i++) {
    std::string result = base64_decode(base64_encode(tests[i]));
    EXPECT_EQ(tests[i], result);
  }
}
