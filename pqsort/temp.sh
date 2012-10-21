for i in {1..1000}
do
    generate 100
    run 8 > temp
    if grep -q NOT temp
        then echo "FAILED"
        exit
    fi
done
