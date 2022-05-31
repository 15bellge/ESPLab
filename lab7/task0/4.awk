#! /usr/bin/awk -f

BEGIN { FS = " " }
      {
        print $1 " " $4 " " $3 " " $2
      }