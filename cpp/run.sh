

echo $LD_LIBRARY_PATH | grep -q "/usr/local/lib"

if [ $? -ne 0 ]; then
        if [ "$LD_LIBRARY_PATH" == "" ]; then
                export LD_LIBRARY_PATH=/usr/local/lib
        else
                export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
        fi
fi

echo "LD_LIBRARY_PATH:$LD_LIBRARY_PATH"

./OptCalcServer resources/config.json



