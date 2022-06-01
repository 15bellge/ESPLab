#! /usr/bin/awk -f

BEGIN { FS = " "
        counter = 0
      }
      {
        if($3 == "English")
            counter++
      }
END   { print "Count = " counter }