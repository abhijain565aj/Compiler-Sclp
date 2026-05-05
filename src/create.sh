current_dir="."
dest_dir="../sclp-submit/"

rm -rf $dest_dir
mkdir $dest_dir

allfiles=$(find $current_dir -type f | sort)
for f in $allfiles; do
    echo creating link for $f
    ln -f $f $dest_dir
    done