require 'formula'

class ZooKeeperLib < Requirement
  def initialize
    @zk = Formula.factory('zookeeper')
  end

  def fatal?
    true
  end

  def satisfied?
    @zk.installed? and File.exist?(@zk.lib + 'libzookeeper_mt.dylib')
  end

  def message
    if @zk.installed?
      <<-EOS.undent
        ZooKeeper build was requested, but Zookeeper was already built without `--c` option.
        You will need to `brew uninstall zookeeper; brew install zookeeper --c` first.
      EOS
    else
      <<-EOS.undent
        ZooKeeper build was requested, but Zookeeper is not installed.
        You will need to `brew install zookeeper --c` first.
      EOS
    end
  end
end

class Jubatus < Formula
  url 'https://github.com/jubatus/jubatus/tarball/jubatus-0.3.3'
  head 'https://github.com/jubatus/jubatus.git'
  homepage 'http://jubat.us/'
  md5 'c24152f6b75c4c95217bbca8c6771b29'

  option 'enable-zookeeper', 'Using zookeeper for distributed environemnt'

  depends_on 'glog'
  depends_on 'mecab' unless ARGV.include? "--disable-mecab"
  depends_on 'pkg-config'
  depends_on 'pficommon'
  depends_on 're2' unless ARGV.include? "--disable-re2"

  if build.include? 'enable-zookeeper'
    depends_on ZooKeeperLib.new
  end

  def options
    [
     [ '--disable-mecab', 'if not nesseary' ],
     [ '--disable-re2', 'if not nessesary']
    ]
  end

  def install
    args = []
    args << "--prefix=#{prefix}"
    args << "--disable-re2" if ARGV.include? "--disable-re2"
    args << "--enable-mecab" unless ARGV.include? "--disable-mecab"
    system "./waf", "configure", *args
    system "./waf", "build"
    system "./waf", "install"
  end

  def test
    # This test will fail and we won't accept that! It's enough to just
    # replace "false" with the main program this formula installs, but
    # it'd be nice if you were more thorough. Test the test with
    # `brew test jubatus`. Remove this comment before submitting
    # your pull request!
    system "false"
  end
end
