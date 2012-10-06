require 'formula'

class Pficommon < Formula
  # url 'https://github.com/pfi/pficommon/tarball/1.3.1.0'
  head 'https://github.com/pfi/pficommon.git'
  homepage 'http://pfi.github.com/pficommon/'
  # md5 'cc3cabc99c646d1a952efe2e563dfdc9'

  depends_on 'msgpack'

  def install
    system "./configure", "--prefix=#{prefix}"
    system "make"
    system "make", "install"
  end

  def test
    # This test will fail and we won't accept that! It's enough to just
    # replace "false" with the main program this formula installs, but
    # it'd be nice if you were more thorough. Test the test with
    # `brew test pficommon`. Remove this comment before submitting
    # your pull request!
    system "false"
  end
end
