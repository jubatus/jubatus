Jubatus
=======

The Jubatus library is an online machine learning framework which runs in distributed environment.

See http://jubat.us for details.

QuickStart
----------

Jubatus officially supports Ubuntu >= 12.04 , Red Hat Enterprise Linux >= 6.2 . On other platforms, some may work if they have gcc >= 4.4 .

For minimal standalone mode, Jubatus requires pficommon >= 1.3.1 (commit 8fde51454af897cc971bab9033e217ff83b12f78 or later), google-glog , libmsgpack, jubatus-mpio, jubatus-msgpack-rpc, python >= 2.6 . Please install these software with headers before installation.

::

  $ git clone git://github.com/jubatus/jubatus.git
  $ cd jubatus
  $ ./waf configure --disable-re2
  $ ./waf build
  $ sudo ./waf install
  $ jubaclassifier -f /usr/local/share/jubatus/example/config/classifier/pa.json

Here Jubatus classifier server has started. Enjoy!

License
-------

LGPL 2.1

Update history
--------------

Update history can be found `here <https://github.com/jubatus/jubatus/wiki/Release-Note>`_ .
