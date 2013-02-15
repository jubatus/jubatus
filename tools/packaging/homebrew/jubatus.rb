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
    return nil if satisfied?
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
  url 'https://github.com/jubatus/jubatus/tarball/0.4.1'
  head 'https://github.com/jubatus/jubatus.git'
  homepage 'http://jubat.us/'
  md5 'ffeef232b84615fc6b74ac50566e21e4'
  version '0.4.0'

  option 'enable-zookeeper', 'Using zookeeper for distributed environemnt'
  option 'enable-mecab', 'Using mecab for Japanese NLP'
  option 'enable-re2', 'Using re2 for regx'

  depends_on 'glog'
  depends_on 'pkg-config'
  depends_on 'pficommon'
  depends_on 'jubatus-msgpack-rpc'

  depends_on ZooKeeperLib.new if build.include? 'enable-zookeeper'
  depends_on 'mecab' if build.include? 'enable-mecab'
  depends_on 're2' if build.include? 'enable-re2'
  # snow leopard default gcc version is 4.2
  depends_on 'gcc' if build.include? 'snow-leopard'

  def install
    if ENV.compiler == :gcc
      gcc = Formula.factory('gcc')
      version = '4.7'

      if File.exist?(gcc.bin)
        bin = gcc.bin.to_s
        ENV['CC'] = bin+"/gcc-#{version}"
        ENV['LD'] = bin+"/gcc-#{version}"
        ENV['CXX'] = bin+"/g++-#{version}"
      end
    end

    STDERR.puts ENV['CC'], ENV['CXX']
    args = []
    args << "--prefix=#{prefix}"
    args << "--disable-re2" unless build.include? "enable-re2"
    args << "--enable-mecab" if build.include? "enable-mecab"
    args << "--enable-zookeeper" if build.include? "enable-zookeeper"
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
