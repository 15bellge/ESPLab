#! /usr/bin/awk -f

BEGIN{
    for(i = 0; i < ARGC; i++){
        print "ARGV[" i "] = " ARGV[i]
    }
}