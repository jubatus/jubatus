require 'formula'

class JubatusMpio < Formula
  url 'https://github.com/jubatus/jubatus-mpio/tarball/0.4.1'
  head 'https://github.com/jubatus/jubatus-mpio.git'
  homepage 'https://github.com/jubatus/jubatus-mpio'
  md5 'd9479520de73375fa076f738bd896c45'

  depends_on 'libtool'
  depends_on 'automake'

  def patches
    # it fails preprocessing with clang
    DATA
  end

  def install
    system "./bootstrap"
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

__END__
diff --git a/mpsrc/wavy_kernel.h b/mpsrc/wavy_kernel.h
index 075b4a4..26dc6c0 100644
--- a/mpsrc/wavy_kernel.h
+++ b/mpsrc/wavy_kernel.h
@@ -41,7 +41,7 @@
 #endif
 
 #define MP_WAVY_KERNEL_HEADER(sys) \
-	MP_PP_HEADER(., wavy_kernel_, sys, )
+	MP_PP_HEADER(.,wavy_kernel_,sys,)
 
 #ifndef MP_WAVY_KERNEL_BACKLOG_SIZE
 #define MP_WAVY_KERNEL_BACKLOG_SIZE 1024
