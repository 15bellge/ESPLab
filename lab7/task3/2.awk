#! /usr/bin/awk -f

BEGIN{ FS = ","
       counter = 0
     }
{
    if(/fun | fun| fun |fan | fan| fan /){
        counter++
    }
}

END {print "The number of poems that have fun or fan is: " counter}