#! /usr/bin/awk -f

BEGIN{ FS = ","}
{
    OFS = " | "
    print $4, $3, $2
}