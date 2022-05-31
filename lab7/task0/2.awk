#! /usr/bin/awk -f

BEGIN { FS = " " }
      {
        print $3 "\t" $4
      }