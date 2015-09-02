[ -n "$CIRCLE_BUILD_NUM" ] &&
   echo $(( 5658 + $CIRCLE_BUILD_NUM )) >version-int.txt && 
   git add version-int.txt && 
   git commit --no-edit -m "Update version, circle build $CIRCLE_BUILD_NUM" &&
   echo version-int.txt now contains `cat version-int.txt`
