#! /usr/bin/awk -f

function average(math, reading, writing){
    return (math+reading+writing)/3
}

BEGIN{ FS = ","}
{
    if($4 == "standard"){
        print average($6, $7, $8)
    }
}