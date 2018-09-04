TODO: Need to document what the hell is going on with the git subtrees
configuration we're using.  At present it is still a bit of rough ride
with some unpredictable behaviors that are a bit difficult to work
around so we'll hold off on documenting anything in too much detail
here until things stabilize.



 $ git remote add clang git@github.com:wsmoses/Tapir-clang.git 
 $ git fetch clang master
 $ git subtree -d pull --prefix=llvm/tools/kitsune/clang --squash -m "adding tapir-clang" clang master 


