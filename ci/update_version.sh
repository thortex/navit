MSG="Update version, circle build"
SUFFIX="_track_version"
if [ -z "$CIRCLE_BUILD_NUM" -o $CIRCLE_PROJECT_USERNAME != "navit-gps" ] ; then
  exit
fi

echo "Testing if we are in versioned directory..."
if ! git log -n 1 || [ ! -f version-int.txt ] ; then
  echo "This script should be ran from the root of versioned directory"
  exit 1
fi

if [ "$CIRCLE_BRANCH" != "trunk$SUFFIX"] ; then
  exit
fi

if [ "$1" == "prepare" ] ; then
  if git log -n 1 | grep "$MSG" ; then 
     echo "Last commit is version update, refusing to repeat it..."
     exit
  fi

  if echo $(( 5658 + $CIRCLE_BUILD_NUM )) >version-int.txt && 
    git config user.email "navit@navit-project.org" &&
    git config user.name "update_version.sh script" &&
    git add version-int.txt && 
    git commit --no-edit -m "$MSG $CIRCLE_BUILD_NUM"
  then
    echo version-int.txt now contains `cat version-int.txt`
  else
    echo "Failed to update version-int.txt"
    exit 1
  fi
  exit
fi

if [ "$1" == "push" ] ; then
  if git log -n 1 | grep "$MSG" ; then 
	git push origin HEAD:refs/heads/trunk$SUFFIX
	git push origin HEAD:refs/heads/master$SUFFIX
  fi
fi
