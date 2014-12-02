Jubatus
=======

The Jubatus library is an online machine learning framework which runs in distributed environment.

See http://jubat.us/ for details.

Quick Start
-----------

We officially support Ubuntu Server 12.04 LTS (x86_64) and Red Hat Enterprise Linux 6.2 or later (x86_64).

See `QuickStart <http://jubat.us/en/quickstart.html>`_ for detailed description.

Red Hat Enterprise Linux 6.2 or later (x86_64)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Run the following command to register Jubatus Yum repository to the system.

::

  $ sudo rpm -Uvh http://download.jubat.us/yum/rhel/6/stable/x86_64/jubatus-release-6-1.el6.x86_64.rpm

Then install ``jubatus`` and ``jubatus-client`` package.

::

  $ sudo yum install jubatus jubatus-client

Now Jubatus is installed in ``/usr/bin/juba*``.

::

  $ jubaclassifier -f /usr/share/jubatus/example/config/classifier/pa.json

Ubuntu Server 12.04 LTS (x86_64)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Write the following line to ``/etc/apt/sources.list.d/jubatus.list`` to register Jubatus Apt repository to the system.

::

  deb http://download.jubat.us/apt binary/

Now install ``jubatus`` package.

::

  $ sudo apt-get update
  $ sudo apt-get install -y --force-yes jubatus

Now Jubatus is installed in ``/opt/jubatus/bin/juba*``.

::

  $ source /opt/jubatus/profile
  $ jubaclassifier -f /opt/jubatus/share/jubatus/example/config/classifier/pa.json

Other Platforms
~~~~~~~~~~~~~~~

For other platforms, refer to the `documentation <http://jubat.us/en/build.html>`_.

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
