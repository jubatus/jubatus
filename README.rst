Jubatus
=======

.. image:: https://api.travis-ci.org/jubatus/jubatus.svg?branch=master
    :target: https://api.travis-ci.org/jubatus/jubatus

The Jubatus library is an online machine learning framework which runs in distributed environment.

See http://jubat.us/ for details.

Quick Start
-----------

We officially support Red Hat Enterprise Linux (RHEL) 6.2 or later (64-bit) and Ubuntu Server 14.04 LTS / 16.04 LTS (64-bit).
On supported systems, you can install all components of Jubatus using binary packages.

See `QuickStart <http://jubat.us/en/quickstart.html>`_ for detailed description.

Red Hat Enterprise Linux 6.2 or later (64-bit)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Run the following command to register Jubatus Yum repository to the system.

::

  // For RHEL 6
  $ sudo rpm -Uvh http://download.jubat.us/yum/rhel/6/stable/x86_64/jubatus-release-6-2.el6.x86_64.rpm

  // For RHEL 7
  $ sudo rpm -Uvh http://download.jubat.us/yum/rhel/7/stable/x86_64/jubatus-release-7-2.el7.x86_64.rpm

Then install ``jubatus`` and ``jubatus-client`` package.

::

  $ sudo yum install jubatus jubatus-client

Now Jubatus is installed in ``/usr/bin/juba*``.

::

  $ jubaclassifier -f /usr/share/jubatus/example/config/classifier/pa.json

Ubuntu Server (64-bit)
~~~~~~~~~~~~~~~~~~~~~~

Write the following line to ``/etc/apt/sources.list.d/jubatus.list`` to register Jubatus Apt repository to the system.

::

  // For Ubuntu 12.04 (Precise) - Deprecated (unsupported)
  deb http://download.jubat.us/apt/ubuntu/precise binary/

  // For Ubuntu 14.04 (Trusty)
  deb http://download.jubat.us/apt/ubuntu/trusty binary/

  // For Ubuntu 16.04 (Xenial)
  deb http://download.jubat.us/apt/ubuntu/xenial binary/

Now install ``jubatus`` package.

::

  $ sudo apt-get update
  $ sudo apt-get install jubatus

Now Jubatus is installed in ``/opt/jubatus/bin/juba*``.

::

  $ source /opt/jubatus/profile
  $ jubaclassifier -f /opt/jubatus/share/jubatus/example/config/classifier/pa.json

Other Platforms
~~~~~~~~~~~~~~~

For other platforms, refer to the `documentation <http://jubat.us/en/developers/build.html>`_.

License
-------

LGPL 2.1

Third-party libraries included in Jubatus
-----------------------------------------

Jubatus source tree includes following third-party library.

- cmdline_ (under BSD 3-Clause License)

.. _cmdline: https://github.com/tanakh/cmdline

Jubatus requires `jubatus_core <https://github.com/jubatus/jubatus_core/>`_ library. jubatus_core contains Eigen and fork of pficommon. Eigen is licensed under MPL2 (partially in LGPL 2.1 or 2.1+). The fork of pficommon is licensed under New BSD License.

Update history
--------------

Update history can be found from `ChangeLog <https://github.com/jubatus/jubatus/blob/master/ChangeLog.rst>`_ or `WikiPage <https://github.com/jubatus/jubatus/wiki/ChangeLog>`_.

Contributors
------------

Patches contributed by `those people <https://github.com/jubatus/jubatus/contributors>`_.
