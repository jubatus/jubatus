#!/bin/bash

##################################
# Include Guard Test
##################################

# Hashtable to store include guard to file mapping.
declare -A guard_table

# Do test for each header files.
for FILE in $(find "${@}" -name "*.hpp" -or -name "*.h"); do

  # Generate the expected include guard from path.
  # Plug-ins need some special tweaks.
  GUARD="$(echo ${FILE} | perl -pe 's|^plugin/src/|jubatus/plugin/|' | tr a-z/. A-Z__)_"

  # Test if the include guard is in the header file.
  RESULT="$(grep -e "^#ifndef ${GUARD}\$" -e "^#define ${GUARD}\$" -e "^#endif  // ${GUARD}\$" ${FILE})"

  # Must have above 3 lines.
  if [ $(echo "${RESULT}" | wc -l) != 3 ]; then

    # Calculate the line no; if not found, use 1.
    LINE_NO="$( ( grep -hn '#ifndef' "${FILE}" || echo 1 ) | head -n 1 | cut -d: -f1)"

    # Display the error message (in cpplint style).
    echo "${FILE}:${LINE_NO}: Include guard ${GUARD} is expected  [jubatus/include_guard] [5]"
  fi

  # Test if the include guard is already used in somewhere else.
  if [ ! -z "${guard_table[${GUARD}]}" ]; then
    echo "${FILE}:${LINE_NO}: Duplicate include guard ${GUARD} with ${guard_table[${GUARD}]}  [jubatus/include_guard] [5]"
  else
    guard_table[${GUARD}]="${FILE}"
  fi
done
