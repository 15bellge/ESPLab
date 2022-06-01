#! /usr/bin/awk -f

BEGIN{ FS = ","
       print "========"
       print "Success Student List"
       print "========"
       counter = 0
     }
{
    if($6 >= 80 && $7 >= 80 && $8 >= 80){
        print $0
        counter++
    }
}
END{ print "The number of students: " counter }