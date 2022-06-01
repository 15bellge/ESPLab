#! /usr/bin/awk -f

BEGIN{ FS = ","}
{
    if($2 ~ /happy | happy| happy /){
        print $2 "---" $3 "---" $4
    }
}