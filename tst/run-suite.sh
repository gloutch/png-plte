
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
    # .bmp file with the same name as .png 
    output=${file/.png/.bmp}
    ./$PLTE_EXE --bmp=$output $file > /dev/null
done

COUNT_BMP=`ls $TST_SUITE/*.bmp | wc -l`
COUNT_PNG=`ls $TST_SUITE/*.png | wc -l`
PERCENT=`expr $COUNT_BMP \* 100 / $COUNT_PNG`

echo $PERCENT% "  :  " $COUNT_BMP / $COUNT_PNG
