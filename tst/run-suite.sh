
function usage() {
    echo "$0 <png-plte> <suite folder>"
}


if [ $# -ne 2 ]; then
    usage
    exit 1
fi


PLTE_EXE=$1
TST_SUITE=$2


for file in $TST_SUITE/*.png
do
    output=${file/.png/.bmp}
    ./$PLTE_EXE --bmp=$output $file > /dev/null

    if [ $? -eq 0 ]; then
        COUNT_BMP=$((COUNT_BMP+1))
    fi
done

echo `ls $TST_SUITE/*.bmp | wc -l` / `ls $TST_SUITE/*.png | wc -l`
