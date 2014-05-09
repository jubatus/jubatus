===============
pfi::text::json
===============

概要
====

JSON(JavaScript Object Notation)を扱うためのライブラリである。
JSONのオブジェクトの操作、文字列への変換、パーザ、pretty printer、
C++ネイティブ型への相互自動変換などを備えている。

JSONの仕様はこちらを参照のこと。

http://www.ietf.org/rfc/rfc4627.txt

使い方
======

jsonデータ型の操作
------------------

.. code-block:: c++

  class json_object
  class json_array
  class json_integer
  class json_float
  class json_bool
  class json_null

それぞれjsonのデータ型を表す
(これらを直接扱うことはあまりない)。

.. code-block:: c++

  enum json_type_t {
    Null,
    Integer,
    Float,
    Bool,
    String,
    Array,
    Object,
  };

それぞれのjsonのデータの型を表すためのタグ。

.. code-block:: c++

  class json_value [#j1c13c05]

上の型の基底クラス。

.. code-block:: c++

  class json

jsonオブジェクトをハンドルするための型
（実際には、これはスマートポインタのような働きをする）。

.. code-block:: c++

  json::json(json_value *)

ハンドルにオブジェクトをセットする。
セットしたポインタはjsonクラスが管理するようになるので、
外では使用したり、解放したりしてはいけない。

.. code-block:: c++

  json::operator[](const string &name)

保持しているjson_objectのメンバを参照する。
保持しているものがjson_objectでなければ、bad_cast例外が投げられる。

.. code-block:: c++

  json::operator[](size_t ix)

保持しているjson_arrayのメンバを参照する。
保持しているものがjson_arrayでなければ、bad_cast例外が投げられる。

.. code-block:: c++

  json::add(const string &name, const json &js)

保持しているjson_objectに要素を追加する
(例外は上と同様)。

.. code-block:: c++

  json::add(const json &js)

保持しているjson_arrayに要素を追加する
(例外は上と同様)。

.. code-block:: c++

  json json::merge(json &js)

保持しているjson_objectに与えられたjson_objectをマージしたjson_objectを返す
(例外は上と同様)。
マージは非破壊であるが、シャローコピーであるため返されたjson_objectへの変更が元のjson_objectに影響を与えることがある。

.. code-block:: c++

  json::size() const

保持しているjson_arrayのサイズを取得する
(例外は上と同様)。

.. code-block:: c++

  iterator json::begin()

保持しているjson_objectの要素の開始イテレータを取得する。
要素の型はpair<string, json>である
(例外は同様)。

.. code-block:: c++

  iterator json::end()

保持しているjson_objectの要素の終端イテレータを取得する
(例外は同様)。

.. code-block:: c++

  json_value *json::get() const

保持しているjson_valueのポインタを返す。

.. code-block:: c++

  json_type_t type() const

保持しているjson_valueの型を示すタグを返す。

.. code-block:: c++

  json::print(ostream &os) const

保持しているjson_valueをストリームに書き出す。
文字数が最小になるように書き出す。

.. code-block:: c++

  json::pretty(ostream &os) const

保持しているjson_valueをストリームに書き出す。
読みやすい形で出力される。

.. code-block:: c++

  json json::clone() const

保持しているjson_valueをディープコピーし、jsonハンドルを返す。

例
---

* オブジェクトを組み立てる。

.. code-block:: c++

  json js(new json_object());
  js["abc"]=json(new json_integer(123));
  js["bcd"]=json(new json_flaot(3.14));
  js["cde"]=json(new json_string("appuppupuepue"));
  js["def"]=json(new json_bool(true));

* 配列を組み立てる。

.. code-block:: c++

  json js(new json_array());
  js.add(json(new json_string("hoge")));
  js.add(json(new json_bool(false)));

文字列との相互変換
------------------

operator<<およびoperator>>がオーバーライドされているので、
それで文字列への相互変換が行える。

.. code-block:: c++

  json js(new json_array());
  js.add(new json_integer(1));
  js.add(new json_integer(2));
  js.add(new json_integer(3));
  
  stringstream ss;
  ss<<js; // [1,2,3]
  
  json ks;
  ss>>ks; // js==ks

読み込みに失敗するとpfi::lang::parse_errorが投げられる。
istreamは読まれた分だけ消費されるので注意すること。

pretty printもできる。

.. code-block:: c++

  json js(new json_array());
  js.add(new json_integer(1));
  js.add(new json_integer(2));
  js.add(new json_integer(3));
  
  cout<<pretty(js);

次のように表示される。

.. code-block:: javascript

  [
    1,
    2,
    3
  ]

C++型との相互変換
-----------------

jsonオブジェクトとC++の型との間で自動的に相互変換ができる。

* json_integer <=> int
* json_float <=> double,float
* json_string <=> string
* json_bool <=> bool
* json_object <=> map<string, T>
* json_array <=> vector<T>

ユーザ定義型との相互変換もできる(後述)。

template <class T> json to_json(const T &v)
-------------------------------------------

jsonへ変換可能な型をjsonに変換する。

.. code-block:: c++

  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  
  json js=to_json(v);
  cout<<js; // [1,2,3]

template <class T> void from_json(const json &js, T &v)
-------------------------------------------------------

jsonからjsonに変換可能な型に変換する。
jsが型Tに合わないオブジェクトを含む場合、bad_cast例外が投げられる。

.. code-block:: c++

  json js(new json_array());
  js.add(new json_integer(1));
  js.add(new json_integer(2));
  js.add(new json_integer(3));
  
  vector<int> v;
  from_json(js, v); // {1, 2, 3}

ユーザ定義型との相互変換
------------------------

ユーザ定義型とjson_objectとを相互変換させることができる。
相互変換できるようにするには、そのクラスをserializableにする必要がある。
ただし、メンバのシリアライズに必ずMEMBERマクロを用いて、
シリアライザにメンバの名前を教えてやる必要がある。

.. code-block:: c++

  struct foo{
    int a;
    string b;
    struct bar{
      vector<double> x;
  
      template <class Archive>
      void serialize(Archive &ar){
        ar & MEMBER(x);
      }
    } c;
  
    template <class Archive>
    void serialize(Archive &ar){
      ar & MEMBER(a) & MEMBER(b) & MEMBER(c);
    }
  };
  
  int main(){
    foo f;
    f.a=123;
    f.b="hoge";
    f.c.x.push_back(1.4142);
    f.c.x.push_back(3.1415);
  
    cout<<to_json(f)<<endl; // {"a":123,"b":"hoge","c":{"x":[1.4142,3.1415]}}
  }

template <class T> T json_cast<T>(const json &js)
-------------------------------------------------

jsをそれと変換可能な型Tに変換する。
jsが型に合わない場合はbad_cast例外が投げられる。

.. code-block:: c++

  json js(new json_integer(123));
  cout<<json_cast<int>(js)<<endl; // 123

via_json
--------

streamからjsonで記述された型Tのオブジェクトをダイレクトに取り出す。

.. code-block:: c++

  foo f;
  cin>>via_json(f);
