#! /usr/bin/awk -f

BEGIN{ FS = ","}
{
    if($1 >= 50 && $1 <= 70){
        print $5
    }
}