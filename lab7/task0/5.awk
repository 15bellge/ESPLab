#! /usr/bin/awk -f

BEGIN { FS = " "
        counter = 0
      }
      {
        if(/English/)
            counter++
      }
END   { print "Count = " counter }