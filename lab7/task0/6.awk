#! /usr/bin/awk -f

BEGIN { FS = " " }
{
    if($4 > 87)
        print $1 " " $2 " " $3 " " $4
}