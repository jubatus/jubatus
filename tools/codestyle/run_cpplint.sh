#!/bin/sh

find ${@} '(' \
      -name "*.cpp" -or \
      -name "*.cc" -or \
      -name "*.hpp" -or \
      -name "*.h" \
    ')' -and -not '(' \
      -path 'jubatus/server/third_party/*' -or \
      -path 'jubatus/server/server/*_impl.cpp' -or \
      -path 'jubatus/server/server/*_proxy.cpp' -or \
      -path 'jubatus/server/server/*_client.hpp' -or \
      -path 'jubatus/server/server/*_types.hpp' -or \
      -path 'jubatus/client/*_client.hpp' -or \
      -path 'jubatus/client/*_types.hpp' \
    ')' \
| xargs "$(dirname ${0})/cpplint/cpplint.py" --filter=-runtime/references,-runtime/rtti 2>&1 \
| grep -v '^Done processing '

"$(dirname ${0})/check_include_guard.sh" $(find ${@} -name "*.hpp")
