require 'formula'

class Jubatus < Formula
  url 'https://github.com/jubatus/jubatus/tarball/jubatus-0.3.2'
  head 'https://github.com/jubatus/jubatus.git'
  homepage 'http://jubat.us/'
  md5 '53122d27aa5889d14917338171e3b396'

  depends_on 'glog'
  depends_on 'libevent'
  depends_on 'mecab' unless ARGV.include? "--disable-mecab"
  depends_on 'pkg-config'
  depends_on 'pficommon'
  depends_on 're2' unless ARGV.include? "--disable-re2"

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
