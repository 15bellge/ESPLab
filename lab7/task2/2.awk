#! /usr/bin/awk -f

BEGIN{ FS = ","
       counter = 0
     }
{
    if($3 == "bachelor's degree"){
        counter++
    }
}
END{print counter}