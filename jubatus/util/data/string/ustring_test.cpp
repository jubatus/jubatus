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

#include "./ustring.h"

#include <algorithm>
#include <vector>

using namespace std;
using namespace jubatus::util::data::string;

ustring hiragana=string_to_ustring("ぁあぃいぅうぇえぉおかがきぎくぐけげこごさざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわ");
ustring katakana=string_to_ustring("ァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾタダチヂッツヅテデトドナニヌネノハバパヒビピフブプヘベペホボポマミムメモャヤュユョヨラリルレロヮワ");
ustring hankaku_kana=string_to_ustring("ｧｱｨｲｩｳｪｴｫｵｶｶﾞｷｷﾞｸｸﾞｹｹﾞｺｺﾞｻｻﾞｼｼﾞｽｽﾞｾｾﾞｿｿﾞﾀﾀﾞﾁﾁﾞｯﾂﾂﾞﾃﾃﾞﾄﾄﾞﾅﾆﾇﾈﾉﾊﾊﾞﾊﾟﾋﾋﾞﾋﾟﾌﾌﾞﾌﾟﾍﾍﾞﾍﾟﾎﾎﾞﾎﾟﾏﾐﾑﾒﾓｬﾔｭﾕｮﾖﾗﾘﾙﾚﾛヮﾜ");
ustring kanji=string_to_ustring("仏説摩訶般若波羅蜜多心経観自在菩薩行深般若波羅蜜多時照見五蘊皆空度一切苦厄舎利子色不異空空不異色色即是空空即是色受想行識亦復如是舎利子是諸法空相不生不滅不垢不浄不増不減是故空中無色無受想行識無眼耳鼻舌身意無色声香味触法無眼界乃至無意識界無無明亦無無明尽乃至無老死亦無老死尽無苦集滅道無智亦無得以無所得故菩提薩埵依般若波羅蜜多故心無罣礙無罣礙故無有恐怖遠離一切顛倒夢想究竟涅槃三世諸仏依般若波羅蜜多故得阿耨多羅三藐三菩提故知般若波羅蜜多是大神呪是大明呪是無上呪是無等等呪能除一切苦真実不虚故説般若波羅蜜多呪即説呪曰羯諦羯諦波羅羯諦波羅僧羯諦菩提薩婆訶般若心経");
ustring alphabet=string_to_ustring("abcdefghijklmnopqrstuvwxyz");
ustring zenkaku_alphabet=string_to_ustring("ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ");
ustring basic_latin=string_to_ustring(" !\"#$%&\'()*+,-./0123456789:;<=>\?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
ustring zenkaku_latin=string_to_ustring("　！＂＃＄％＆＇（）＊＋，－．／０１２３４５６７８９：；＜＝＞？＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～");

TEST(string_test, cast) {
  ustring us(string_to_ustring("hello"));
  ustring vs=us.substr(1, 2);
  basic_string<uchar> ws=vs;
  ustring xs=ws;
  EXPECT_TRUE(string_to_ustring("el")==xs);
}

TEST(ustring_test, basic) {
  const char* p="私の名前は中野です。";
  string s=p;
  EXPECT_EQ(ustring_to_string(string_to_ustring(p)),s);
  EXPECT_EQ(ustring_to_string(string_to_ustring(s)),s);

  EXPECT_EQ(string_to_uchar(uchar_to_string(hiragana[0])),hiragana[0]);
  EXPECT_EQ(string_to_uchar(uchar_to_string(katakana[0])),katakana[0]);
  EXPECT_EQ(string_to_uchar(uchar_to_string(hankaku_kana[0])),hankaku_kana[0]);
  EXPECT_EQ(string_to_uchar(uchar_to_string(kanji[0])),kanji[0]);
  EXPECT_EQ(string_to_uchar(uchar_to_string(alphabet[0])),alphabet[0]);
  EXPECT_EQ(string_to_uchar(uchar_to_string(zenkaku_alphabet[0])),zenkaku_alphabet[0]);
}

TEST(ustring_test, utility) {
  for (int i=0;i<(int)hiragana.size();++i) {
    EXPECT_TRUE(is_hiragana(hiragana[i]));
    EXPECT_FALSE(is_katakana(hiragana[i]));
    EXPECT_FALSE(is_kanji(hiragana[i]));
  }
  for (int i=0;i<(int)katakana.size();++i) {
    EXPECT_FALSE(is_hiragana(katakana[i]));
    EXPECT_TRUE(is_katakana(katakana[i]));
    EXPECT_FALSE(is_kanji(katakana[i]));
  }
  for (int i=0;i<(int)kanji.size();++i) {
    EXPECT_FALSE(is_hiragana(kanji[i]));
    EXPECT_FALSE(is_katakana(kanji[i]));
    EXPECT_TRUE(is_kanji(kanji[i]));
  }
}

TEST(ustring_test, conversion) {
  EXPECT_EQ(hankaku_to_zenkaku(hankaku_kana),katakana);
  EXPECT_EQ(zenkaku_latin_to_basic_latin(zenkaku_alphabet),alphabet);
}

TEST(ustring_test, zenkaku_latin_to_basic_latin_uchar) {
  ASSERT_EQ(basic_latin.size(), zenkaku_latin.size());
  for (size_t i=0;i<zenkaku_latin.size();++i) {
    EXPECT_EQ(basic_latin[i],zenkaku_latin_to_basic_latin(zenkaku_latin[i]));
  }
}

TEST(ustring_test, zenkaku_latin_to_basic_latin_ustring) {
  EXPECT_EQ(basic_latin,zenkaku_latin_to_basic_latin(zenkaku_latin));
}

TEST(ustring_test, basic_latin_to_zenkaku_latin_uchar) {
  ASSERT_EQ(zenkaku_latin.size(), basic_latin.size());
  for (size_t i=0;i<basic_latin.size();++i) {
    EXPECT_EQ(zenkaku_latin[i],basic_latin_to_zenkaku_latin(basic_latin[i]));
  }
}

TEST(ustring_test, basic_latin_to_zenkaku_latin_ustring) {
  EXPECT_EQ(zenkaku_latin,basic_latin_to_zenkaku_latin(basic_latin));
}
