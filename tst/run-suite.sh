
function usage() {
    echo "$0 <png-plte> <suite folder>"
}

if [ $# -ne 2 ]; then
    usage
    exit 1
fi


PLTE_EXE=$1
TST_SUITE=$2


echo "$PLTE_EXE --bmp=$TST_SUITE/*.bmp $TST_SUITE/*.png"

for file in $TST_SUITE/*.png
do
    output=${file/.png/.bmp}
    ./$PLTE_EXE --bmp=$output $file > /dev/null

    if [ $? -eq 0 ]; then
        echo -n "."
    else
        echo -n "x"
    fi    
done

echo
echo "Done"
