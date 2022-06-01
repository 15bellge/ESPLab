#! /usr/bin/awk -f

BEGIN{ FS = ","}
{
    if(/spring/){
        print $2 "---" $3 "---" $4
    }
}