#! /usr/bin/awk -f

BEGIN{ FS = ","}
{
    if($2 >= 1970){
        print "Actor Name: " $4
        print "Movie Name: " $5
        print "---------------------------"
    }
}