==============
pfi::text::csv
==============

概要
====

CSVパーザ。

使い方
======

parser作成
----------

* char型を返すイテレータを渡す

.. code-block:: c++

  vector<char> s;
  csv_parser p(s.begin(), s.end());

* istreamを渡す

.. code-block:: c++

  ifstream ifs("hoge");
  csv_parser p(ifs);

* stringを渡す

.. code-block:: c++

  csv_parser p("a,b,c\nd,e,f");

イテレーション
--------------

* next()で回す

.. code-block:: c++

  while(p.next())
    for (size_t i=0; i<p.get().size(); i++)
      cout<<p.get()[i]<<endl;

csv_parser::get() は vector<const char*> を返す。

* イテレータで回す

.. code-block:: c++

  for (csv_iterator it(p), jt; it!=jt; ++it)
    for (size_t i=0; i<it->size(); i++)
      cout<<(*it)[i]<<endl;

お手軽インターフェース
----------------------

.. code-block:: c++

  void parse_csv(const string &input, vector<vector<string> > &ret);

サンプルコード
==============

.. code-block:: c++

  #include <iostream>
  #include <fstream>
  #include <pficommon/text/csv.h>
  using namespace std;
  
  int main(int argc, char *argv[])
  {
    ifstream ifs(argv[1]);
  
    if (!ifs) return 0;
  
    pfi::text::csv_parser psr(ifs);
    for (pfi::text::csv_iterator p(psr), q; p!=q; ++p){
      for (size_t i=0; i<p->size() ;i++)
        cout<<(*p)[i]<<'$'<<endl;
      cout<<"====="<<endl;
    }
  }
