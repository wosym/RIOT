if [ $# -gt 0 ]
then
    DELAY=$1
else
    DELAY=0.5
fi
echo "Spamming status changes over CAN"
while :
do
    echo "01 - IDLE"
    cansend can0 002#01
    sleep $DELAY
    echo "02 - HEATING"
    cansend can0 002#02
    sleep $DELAY
    echo "03 - OVERHEAT"
    cansend can0 002#03
    sleep $DELAY
    echo "04 - ERROR"
    cansend can0 002#04
    sleep $DELAY
done

