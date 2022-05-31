#! /usr/bin/awk -f

BEGIN { FS = " " }
      {
        if($2 ~ /a/)
            print $2 "\t" $4
      }